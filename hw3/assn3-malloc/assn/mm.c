/*
 * This implementation replicates the implicit list implementation
 * provided in the textbook
 * "Computer Systems - A Programmer's Perspective"
 * Blocks are never coalesced or reused.
 * Realloc is implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "SpaceSquids",
    /* First member's full name */
    "Timmy Rong Tian Tse (998182657)",
    /* First member's email address */
    "timmy.tse@mail.utoronto.ca",
    /* Second member's full name (leave blank if none) */
    "Yuan Xue (998275851)",
    /* Second member's email address (leave blank if none) */
    "yuan.xue@mail.utoronto.ca"
};

/*************************************************************************
 * Basic Constants and Macros
 * You are not required to use these macros but may find them helpful.
*************************************************************************/
#define WSIZE       sizeof(void *)            /* word size (bytes) */
#define DSIZE       (2 * WSIZE)            /* doubleword size (bytes) */
#define CHUNKSIZE   (1<<7)      /* initial heap size (bytes) */

#define MAX(x,y) ((x) > (y)?(x) :(y))

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc) ((size) | (alloc))

/* Read and write a word at address p */
#define GET(p)          (*(uintptr_t *)(p))
#define PUT(p,val)      (*(uintptr_t *)(p) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p)     (GET(p) & ~(DSIZE - 1))
#define GET_ALLOC(p)    (GET(p) & 0x1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp)        ((char *)(bp) - WSIZE)
#define FTRP(bp)        ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

void* heap_listp = NULL;

/**********************************************************
 * Variables for segregated list
 * *******************************************************/
#define DEBUG 0
#define SL_SIZE 16
#define NEXT_FREE(bp) ((char*) bp)
#define PREV_FREE(bp) ((char*) (bp + WSIZE))
static void* sl [SL_SIZE];

/**********************************************************
 * Functions for segregated list
 * *******************************************************/
void  sl_init()__attribute__((always_inline));
void* sl_place(void* bp, size_t asize)__attribute__((always_inline));
void*  sl_find_fit(size_t asize)__attribute__((always_inline));
void*  sl_split(void* bp, size_t asize)__attribute__((always_inline));
void  sl_remove(void* bp)__attribute__((always_inline));
void  sl_insert(void* bp)__attribute__((always_inline));
int  sl_get_index(size_t size)__attribute__((always_inline));

/* Functions for segregated list - BEGIN */

/**********************************************************
 * sl_init
 * Initializes the segregated list to NULL
 * *******************************************************/
void  sl_init () {
  int i;
  for (i = 0; i < SL_SIZE; i++)
    sl[i] = NULL;
}

/**********************************************************
 * sl_place
 * Splits a free block, removes it from the
 * segregated list and assigns it not free
 * *******************************************************/
void*  sl_place(void* bp, size_t asize) {
  size_t bsize = GET_SIZE(HDRP(bp));
#if DEBUG
  assert(bsize >= asize);
#endif
  bp = sl_split(bp, asize);
  bsize = GET_SIZE(HDRP(bp));
#if DEBUG
  assert(bsize >= asize);
#endif
  sl_remove(bp);
  PUT(HDRP(bp), PACK(bsize, 1));
  PUT(FTRP(bp), PACK(bsize, 1));
#if DEBUG
  mm_check();
#endif
  return bp;
}

/**********************************************************
 * sl_split
 * Splits a free block, removes it from the
 * segregated list and assigns it not free.
 * The logic attempts to improve utility by
 * grouping the blocks by their sizes. The logic
 * flow is kind of weird, but that is because of
 * some trial and error, this was the logic flow
 * that ran the fastest for us.
 * *******************************************************/
void*  sl_split(void* bp, size_t asize) {
  size_t bsize = GET_SIZE(HDRP(bp));
#if DEBUG
  assert(bsize >= asize);
#endif

  size_t remainder = bsize - asize;
  if (remainder < 2 * DSIZE) {
    return bp;
  }

  // Try to place big blocks beside other big blocks and small
  // blocks beside other small blocks to minimize fragmentation
  if (GET_SIZE(HDRP(NEXT_BLKP(bp))) >= GET_SIZE(HDRP(PREV_BLKP(bp)))) {
    if (asize >= remainder) {
      sl_remove(bp);
      PUT(HDRP(bp), PACK(remainder, 0));
      PUT(FTRP(bp), PACK(remainder, 0));
      PUT(HDRP(NEXT_BLKP(bp)), PACK(asize, 0));
      PUT(FTRP(NEXT_BLKP(bp)), PACK(asize, 0));
      sl_insert(bp);
      sl_insert(NEXT_BLKP(bp));
      return NEXT_BLKP(bp);
    } else {
      sl_remove(bp);
      PUT(HDRP(bp), PACK(asize, 0));
      PUT(FTRP(bp), PACK(asize, 0));
      PUT(HDRP(NEXT_BLKP(bp)), PACK(remainder, 0));
      PUT(FTRP(NEXT_BLKP(bp)), PACK(remainder, 0));
      sl_insert(bp);
      sl_insert(NEXT_BLKP(bp));
      return bp;
    }
  }
  sl_remove(bp);
  PUT(HDRP(bp), PACK(remainder, 0));
  PUT(FTRP(bp), PACK(remainder, 0));
  PUT(HDRP(NEXT_BLKP(bp)), PACK(asize, 0));
  PUT(FTRP(NEXT_BLKP(bp)), PACK(asize, 0));
  sl_insert(bp);
  sl_insert(NEXT_BLKP(bp));
  return NEXT_BLKP(bp);
}


/**********************************************************
 * sl_remove
 * This function removes an element from the
 * segregated list.
 * *******************************************************/
void  sl_remove (void* bp) {
#if DEBUG
  assert(bp);
  assert(check(bp) == 1);
#endif
  int index = sl_get_index(GET_SIZE(HDRP(bp)));
  if (sl[index] == bp) {
    sl[index] = GET(NEXT_FREE(bp));
  }
  void* next = (void*) GET(NEXT_FREE(bp));
  void* prev = (void*) GET(PREV_FREE(bp));
  if (prev) {
    (*(uintptr_t*)((char*)prev))=next;
  }
  if (next) {
    PUT((char*)(next + WSIZE), prev);
  }
#if DEBUG
  assert(check(bp) == 0);
  mm_check();
#endif
}

int  sl_get_index (size_t size) {
  int index = 0;
  size_t bsize = 63;
  while (bsize < size) {
    index++;
    if (index == SL_SIZE) {
      index--;
      break;
    }
    bsize = (bsize + 1)*2 - 1;
  }
#if DEBUG
  assert(index < SL_SIZE);
  mm_check();
#endif
  return index;
}

/**********************************************************
 * sl_insert
 * This function inserts an element from the
 * segregated list.
 * *******************************************************/
void  sl_insert (void* bp) {
  int index = sl_get_index(GET_SIZE(HDRP(bp)));
  if (!sl[index]) {
    sl[index] = bp;
    PUT((char*)bp, (uintptr_t)0);
    PUT((char*)bp + WSIZE, (uintptr_t)0);
    return;
  }
  void* curr;
  void* prev;

  prev = NULL;
  curr = sl[index];

  while (curr) {
    if (GET_SIZE(HDRP(bp)) >= GET_SIZE(HDRP(curr))) {
      break;
    }
    prev = curr;
    curr = (void*) GET((char*)curr);
  }

  if (!curr) {
    PUT((char*)prev, (uintptr_t)bp);
    PUT((char*)(bp + WSIZE), (uintptr_t)prev);
    PUT((char*)bp, (uintptr_t)0);
  } else if (!prev) {
    PUT((char*)(curr + WSIZE), (uintptr_t)bp);
    PUT((char*)bp, (uintptr_t)curr);
    PUT((char*)(bp + WSIZE), (uintptr_t)0);
    sl[index] = bp;
  } else {
    PUT((char*)prev, (uintptr_t)bp);
    PUT((char*)(curr + WSIZE), (uintptr_t)bp);
    PUT((char*)bp, (uintptr_t)curr);
    PUT((char*)(bp + WSIZE), (uintptr_t)prev);
  }
}

/**********************************************************
 * sl_find_fit
 * This function finds an element from the
 * segregated list that has at least the size
 * of "asize". Since the list is ordered from
 * biggest to smallest, we need only to look
 * at the first element indexed by the
 * segregated list.
 * *******************************************************/
void*  sl_find_fit(size_t asize) {
#if DEBUG
  mm_check();
  assert(asize >= 2 * DSIZE);
  assert(asize % 16 == 0);
#endif

  int i;
  void* bp = NULL;
  for (i = sl_get_index(asize); i < SL_SIZE; i++) {
    bp = sl[i];
//    while (bp != NULL && GET_SIZE(HDRP(bp)) < asize) {
//      bp = GET(NEXT_FREE(bp));
//    }
    if (bp != NULL && GET_SIZE(HDRP(bp)) >= asize) {
#if DEBUG
      assert(GET_SIZE(HDRP(bp)) >= asize);
#endif
      return bp;
    }
  }
  return NULL;
}
/* Functions for segregated list - END */

/**********************************************************
 * mm_init
 * Initialize the heap, including "allocation" of the
 * prologue and epilogue
 **********************************************************/
 int mm_init(void)
 {
   if ((heap_listp = mem_sbrk(4*WSIZE)) == (void *)-1)
         return -1;
     PUT(heap_listp, 0);                         // alignment padding
     PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1));   // prologue header
     PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1));   // prologue footer
     PUT(heap_listp + (3 * WSIZE), PACK(0, 1));    // epilogue header
     heap_listp += DSIZE;

     sl_init();

     return 0;
 }

/**********************************************************
 * coalesce
 * Covers the 4 cases discussed in the text:
 * - both neighbours are allocated
 * - the next block is available for coalescing
 * - the previous block is available for coalescing
 * - both neighbours are available for coalescing
 **********************************************************/
void *coalesce(void *bp)
{
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc) {       /* Case 1 */
      sl_insert(bp);
      return bp;
    }

    else if (prev_alloc && !next_alloc) { /* Case 2 */
      sl_remove(NEXT_BLKP(bp));
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
      sl_insert(bp);
        return (bp);
    }

    else if (!prev_alloc && next_alloc) { /* Case 3 */
      sl_remove(PREV_BLKP(bp));
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
      sl_insert(PREV_BLKP(bp));
        return (PREV_BLKP(bp));
    }

    else {            /* Case 4 */
      sl_remove(PREV_BLKP(bp));
      sl_remove(NEXT_BLKP(bp));
        size += GET_SIZE(HDRP(PREV_BLKP(bp)))  +
            GET_SIZE(FTRP(NEXT_BLKP(bp)))  ;
        PUT(HDRP(PREV_BLKP(bp)), PACK(size,0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size,0));
      sl_insert(PREV_BLKP(bp));
        return (PREV_BLKP(bp));
    }
}

/**********************************************************
 * extend_heap
 * Extend the heap by "words" words, maintaining alignment
 * requirements of course. Free the former epilogue block
 * and reallocate its new header
 **********************************************************/
void *extend_heap(size_t words)
{
    char *bp;
    size_t size;

    /* Allocate an even number of words to maintain alignments */
    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;
    if ( (bp = mem_sbrk(size)) == (void *)-1 )
        return NULL;

    /* Initialize free block header/footer and the epilogue header */
    PUT(HDRP(bp), PACK(size, 0));                // free block header
    PUT(FTRP(bp), PACK(size, 0));                // free block footer
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));        // new epilogue header

    /* Coalesce if the previous block was free */
    return coalesce(bp);
}


#if 0
/**********************************************************
 * find_fit
 * Traverse the heap searching for a block to fit asize
 * Return NULL if no free blocks can handle that size
 * Assumed that asize is aligned
 **********************************************************/
void * find_fit(size_t asize)
{
    void *bp;
    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp))
    {
        if (!GET_ALLOC(HDRP(bp)) && (asize <= GET_SIZE(HDRP(bp))))
        {
            return bp;
        }
    }
    return NULL;
}

/**********************************************************
 * place
 * Mark the block as allocated
 **********************************************************/
void place(void* bp, size_t asize)
{
  /* Get the current block size */
  size_t bsize = GET_SIZE(HDRP(bp));

  PUT(HDRP(bp), PACK(bsize, 1));
  PUT(FTRP(bp), PACK(bsize, 1));
}
#endif

/**********************************************************
 * mm_free
 * Free the block and coalesce with neighbouring blocks
 **********************************************************/
void mm_free(void *bp)
{
    if(bp == NULL){
      return;
    }
    size_t size = GET_SIZE(HDRP(bp));
    PUT(HDRP(bp), PACK(size,0));
    PUT(FTRP(bp), PACK(size,0));
    coalesce(bp);
}


/**********************************************************
 * mm_malloc
 * Allocate a block of size bytes.
 * The type of search is determined by find_fit
 * The decision of splitting the block, or not is determined
 *   in place(..)
 * If no block satisfies the request, the heap is extended
 **********************************************************/
void *mm_malloc(size_t size)
{
    size_t asize; /* adjusted block size */
    size_t extendsize; /* amount to extend heap if no fit */
    char * bp;

    /* Ignore spurious requests */
    if (size == 0)
        return NULL;

    /* Adjust block size to include overhead and alignment reqs. */
    if (size <= DSIZE)
        asize = 2 * DSIZE;
    else
        asize = DSIZE * ((size + (DSIZE) + (DSIZE-1))/ DSIZE);
#if 0
    if (asize == 464) {
      asize = 512 + 16;
    } else if (asize == 128) {
      asize = 128 + 16;
    }
#endif

    /* Search the free list for a fit */
#if 0
    if ((bp = find_fit(asize)) != NULL) {
        place(bp, asize);
        return bp;
    }
#endif
#if 1
    if ((bp = sl_find_fit(asize)) != NULL) {
      bp = sl_place(bp, asize);
      return bp;
    }
#endif

    /* No fit found. Get more memory and place the block */
    extendsize = MAX(asize, CHUNKSIZE);
    if ((bp = extend_heap(extendsize/WSIZE)) == NULL)
        return NULL;
    //place(bp, asize);
    bp = sl_place(bp, asize);
    return bp;
}

/**********************************************************
 * split_realloc
 * This function splits the block and places any extra
 * memory back onto the segregated free list.
 * We attempted to improve util by grouping the blocks
 * by sizes, but that did not work.
 **********************************************************/
void* split_realloc (void* ptr, size_t tot_size, size_t asize) {
  size_t remainder = tot_size - asize;
#if DEBUG
  assert(tot_size == GET_SIZE(HDRP(ptr)));
  assert(remainder % DSIZE == 0);
#endif
  if (remainder < 2 * DSIZE) {
    return ptr;
  }
  PUT(HDRP(ptr), PACK(asize, 1));
  PUT(FTRP(ptr), PACK(asize, 1));
  PUT(HDRP(NEXT_BLKP(ptr)), PACK(remainder, 0));
  PUT(FTRP(NEXT_BLKP(ptr)), PACK(remainder, 0));
  sl_insert(NEXT_BLKP(ptr));
  return ptr;

  // Attempted to improve util by grouping the blocks by sizes. Didn't work.
#if 0
  void* prev = PREV_BLKP(ptr);
  void* non_alloc = NEXT_BLKP(ptr);
  void* next = NEXT_BLKP(NEXT_BLKP(ptr));

  size_t prev_bsize = GET_SIZE(prev);
  size_t next_bsize = GET_SIZE(next);

	size_t biggest_adj_bsize = MAX(prev_bsize, next_bsize);
	size_t avg_size = (prev_bsize + next_bsize) / 2;

  //if((GET_SIZE(prev) > GET_SIZE(next) && GET_SIZE(ptr) < GET_SIZE(non_alloc)) ||
     //(GET_SIZE(prev) < GET_SIZE(next) && GET_SIZE(ptr) > GET_SIZE(non_alloc))) {
  if (asize >= avg_size && biggest_adj_bsize == next_bsize) {
    memmove(ptr - WSIZE + remainder, ptr - WSIZE, asize);
    PUT(HDRP(ptr), PACK(remainder, 0));
    PUT(FTRP(ptr), PACK(remainder, 0));
    sl_insert(ptr);
    return (ptr + remainder);
  }

  sl_insert(NEXT_BLKP(ptr));
  return ptr;
#endif
}

/**********************************************************
 * mm_realloc
 * This function is implemented very closely like coalesce.
 * It checks if the neighbours are free. If they are free,
 * then it checks whether the total size (size with itself
 * plus the neighbours) are large enough to hold the 
 * if it is, then reallocate. If not, reallocate through
 * malloc and free.
 **********************************************************/
void *mm_realloc(void *ptr, size_t size)
{
#if 1
    /* If size == 0 then this is just free, and we return NULL. */
    if(size == 0){
      mm_free(ptr);
      return NULL;
    }
    /* If oldptr is NULL, then this is just malloc. */
    if (ptr == NULL)
      return (mm_malloc(size));


    size_t asize; /* adjusted block size */
    /* Adjust block size to include overhead and alignment reqs. */
    if (size <= DSIZE)
      asize = 2 * DSIZE;
    else
      asize = DSIZE * ((size + (DSIZE) + (DSIZE-1))/ DSIZE);

    size_t ptr_size = GET_SIZE(HDRP(ptr));
    if (asize <= ptr_size) {
      ptr = split_realloc(ptr, ptr_size, asize);
      return ptr;
    }

    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(ptr)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(ptr)));
    size_t tot_size = ptr_size;
    if (prev_alloc && !next_alloc) {
      tot_size += GET_SIZE(HDRP(NEXT_BLKP(ptr)));
      if (tot_size >= asize) {
        sl_remove(NEXT_BLKP(ptr));
        PUT(HDRP(ptr), PACK(tot_size, 1));
        PUT(FTRP(ptr), PACK(tot_size, 1));
        ptr = split_realloc(ptr, tot_size, asize);
        return ptr;
      }

    } else if (!prev_alloc && next_alloc) {
      tot_size += GET_SIZE(HDRP(PREV_BLKP(ptr)));
      if (tot_size >= asize) {
        sl_remove(PREV_BLKP(ptr));
        void* prev = PREV_BLKP(ptr);
        PUT(HDRP(prev), PACK(tot_size, 1));
        PUT(FTRP(prev), PACK(tot_size, 1));
        memmove(prev, ptr, asize);
        prev = split_realloc(prev, tot_size, asize);
        return prev;
      }
    } else if (!prev_alloc && !next_alloc) {
      tot_size += GET_SIZE(HDRP(NEXT_BLKP(ptr)))
                + GET_SIZE(HDRP(PREV_BLKP(ptr)));
      if (tot_size >= asize) {
        sl_remove(NEXT_BLKP(ptr));
        sl_remove(PREV_BLKP(ptr));
        void* prev = PREV_BLKP(ptr);
        PUT(HDRP(prev), PACK(tot_size, 1));
        PUT(FTRP(prev), PACK(tot_size, 1));
        memmove(prev, ptr, asize);
        prev = split_realloc(prev, tot_size, asize);
        return prev;
      }
    }
#endif

#if 1
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;

    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;

    /* Copy the old data. */
    copySize = GET_SIZE(HDRP(oldptr));
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
#endif
}

/**********************************************************
 * mm_check
 * Check the consistency of the memory heap
 * Return nonzero if the heap is consistant.
 *********************************************************/
int mm_check(void){
  // Checking if every block in the free list is marked as free.
  void* curr;
  int i;
  for (i = 0; i < SL_SIZE; i++) {
    curr = sl[i];
    while (curr) {
      assert(GET_ALLOC(curr) == 0);
      assert(GET_SIZE(HDRP(curr)) % DSIZE == 0);
      curr = (void*) GET((char*)curr);
    }
  }

  // Checks that all blocks on the heap are a multiple of 16.
  char* bp;
  for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
    assert(GET_SIZE(HDRP(bp)) % DSIZE == 0);
  }

  // Checks that all blocks on the heap are greater or equal to 2 * DSIZE.
  for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
    assert(GET_SIZE(HDRP(bp)) >= 2 * DSIZE);
  }

  // Check that all elements in the segregated list are in the right block.
  size_t lower_lim, upper_lim;
  lower_lim = 0;
  upper_lim = 63;
  void* head;
  for (i = 0; i < SL_SIZE; i++) {
    head = sl[i];
    if (i != SL_SIZE - 1) {
      while (head) {
        assert(GET_SIZE(HDRP(head)) >= lower_lim && GET_SIZE(HDRP(head)) <= upper_lim);
        head = (void*) GET((char*)head);
      }
    } else {
      while (head) {
        assert(GET_SIZE(HDRP(head)) >= lower_lim);
        head = (void*) GET((char*)head);
      }
    }
    lower_lim = upper_lim + 1;
    upper_lim = (upper_lim + 1)*2 - 1;
  }

}

#if 0
int check (void* bp) {
  int i;
  for (i = 0; i < SL_SIZE; i++) {
    void* curr = sl[i];
    while (curr) {
      if (bp == curr) return 1;
      curr = (void*)GET((char*)curr);
    }
  }
  return 0;
}
#endif
