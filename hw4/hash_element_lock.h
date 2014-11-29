
#ifndef HASH_ELEMENT_LOCK_H
#define HASH_ELEMENT_LOCK_H

#include <stdio.h>
#include "list_element.h"

#define HASH_INDEX(_addr,_size_mask) (((_addr) >> 2) & (_size_mask))

template<class Ele, class Keytype> class hash;

template<class Ele, class Keytype> class hash {
 private:
  unsigned my_size_log;
  unsigned my_size;
  unsigned my_size_mask;
  list<Ele,Keytype> *entries;
  list<Ele,Keytype> *get_list(unsigned the_idx);

 public:
  void setup(unsigned the_size_log=5);
  void insert(Ele *e);
  Ele *lookup(Keytype the_key);
  void print(FILE *f=stdout);
  void reset();
  void cleanup();

  void lookup_and_insert_if_absent(Keytype the_key);
  void lock_list(unsigned int idx);
  void unlock_list(unsigned int idx);

};

template<class Ele, class Keytype> 
void 
hash<Ele,Keytype>::setup(unsigned the_size_log){
  my_size_log = the_size_log;
  my_size = 1 << my_size_log;
  my_size_mask = (1 << my_size_log) - 1;
  entries = new list<Ele,Keytype>[my_size];
  mutexes = new* pthread_mutex_t[my_size];

//  for (int i = 0; i < my_size; i++) {
//    mutexes[i] = PTHREAD_MUTEX_INITIALIZER;
//  }
}

template<class Ele, class Keytype> 
list<Ele,Keytype> *
hash<Ele,Keytype>::get_list(unsigned the_idx){
  if (the_idx >= my_size){
    fprintf(stderr,"hash<Ele,Keytype>::list() public idx out of range!\n");
    exit (1);
  }
  return &entries[the_idx];
}

#if 0
template<class Ele, class Keytype> 
Ele *
hash<Ele,Keytype>::lookup(Keytype the_key){
  list<Ele,Keytype> *l;
  unsigned int idx = HASH_INDEX(the_key,my_size_mask);
  lock_list(idx);
  l = &entries[idx];
  Ele* tmp = l->lookup(the_key);
  unlock_list(idx);
  return tmp;

}
#endif

template<class Ele, class Keytype> 
void 
hash<Ele,Keytype>::print(FILE *f){
  unsigned i;

  for (i=0;i<my_size;i++){
    entries[i].print(f);
  }
}

template<class Ele, class Keytype> 
void 
hash<Ele,Keytype>::reset(){
  unsigned i;
  for (i=0;i<my_size;i++){
    entries[i].cleanup();
  }
}

template<class Ele, class Keytype> 
void 
hash<Ele,Keytype>::cleanup(){
  unsigned i;
  reset();
  delete [] entries;
  delete [] mutexes;
}

#if 0
template<class Ele, class Keytype> 
void 
hash<Ele,Keytype>::insert(Ele *e){
  unsigned int idx = HASH_INDEX(e->key(),my_size_mask);
  lock_list(idx);
  entries[idx].push(e);
  unlock_list(idx);
}
#endif

template<class Ele, class Keytype> 
void
hash<Ele,Keytype>::lookup_and_insert_if_absent(Keytype the_key){
  list<Ele,Keytype> *l;
  unsigned int idx = HASH_INDEX(the_key,my_size_mask);
  lock_list(idx);
  l = &entries[idx];
  Ele* tmp;
  if ((tmp = l->lookup(the_key)) == NULL) {
    tmp = new Ele(the_key);
    l->push(tmp);
  }
  pthread_mutex_lock(&tmp->mutex);
  tmp->count++;
  unlock_list(idx);
  pthread_mutex_unlock(&tmp->mutex);
  //return tmp;
  //return l->lookup(the_key);
}  

template<class Ele, class Keytype> 
void 
hash<Ele,Keytype>::lock_list(unsigned int idx){
  pthread_mutex_lock(&mutexes[idx]);
}

template<class Ele, class Keytype> 
void 
hash<Ele,Keytype>::unlock_list(unsigned int idx){
  pthread_mutex_unlock(&mutexes[idx]);
}

#endif
