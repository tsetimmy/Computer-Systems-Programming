/*****************************************************************************
 * life.c
 * Parallelized and optimized implementation of the game of life resides here
 ****************************************************************************/
#include <pthread.h>
#include <assert.h>
#include "life.h"
#include "util.h"

/**
 * Swapping the two boards only involves swapping pointers, not
 * copying values.
 */
#define SWAP_BOARDS( b1, b2 )  do { \
  char* temp = b1; \
  b1 = b2; \
  b2 = temp; \
} while(0)

#define BOARD( __board, __i, __j )  (__board[(__i) + LDA*(__j)])

/*****************************************************************************
 * Helper function definitions
 ****************************************************************************/
#define NUM_THREADS 4

// Argument structure
struct arg_struct {
  char* inboard;
  char* outboard;
  int rstart;
  int cstart;
  int len;
  int nrows;
  int ncols;
  int LDA;
};

/*
 * do_work is optimized by computing the middle chunk first
 * and then compute the borders. In the middle chunk--the
 * majority case--we do not have to use the mod MACRO which
 * speeds up the execution by a lot.
 */
void* do_work (void* arguments) {
  struct arg_struct *args = (struct arg_struct *)arguments;
  char* inboard = args->inboard;
  char* outboard = args->outboard;
  int rstart = args->rstart;
  int cstart = args->cstart;
  int len = args->len;
  int nrows = args->nrows;
  int ncols = args->ncols;
  int LDA = args->LDA;

  int inorth;
  int isouth;
  int jwest;
  int jeast;

  char neighbor_count;
  char ret;

  // Compute the middle chunk
  int i,j;
  for (i = rstart+1; i < rstart+len-1; i++) {
    for (j = cstart+1; j < cstart+len-1; j++) {
      neighbor_count = 
          BOARD (inboard, i-1, j-1) + 
          BOARD (inboard, i-1, j) + 
          BOARD (inboard, i-1, j+1) + 
          BOARD (inboard, i, j-1) +
          BOARD (inboard, i, j+1) + 
          BOARD (inboard, i+1, j-1) +
          BOARD (inboard, i+1, j) + 
          BOARD (inboard, i+1, j+1);

      ret =  alivep (neighbor_count, BOARD (inboard, i, j));
      if (BOARD(outboard, i, j) != ret)
        BOARD(outboard, i, j) = ret;

    }
  }
  // Compute the top border
  for (i = rstart; i < rstart+1; i++) {
    for (j = cstart; j < cstart+len; j++) {
      inorth = mod (i-1, nrows);
      isouth = mod (i+1, nrows);
      jwest = mod (j-1, ncols);
      jeast = mod (j+1, ncols);

      neighbor_count = 
          BOARD (inboard, inorth, jwest) + 
          BOARD (inboard, inorth, j) + 
          BOARD (inboard, inorth, jeast) + 
          BOARD (inboard, i, jwest) +
          BOARD (inboard, i, jeast) + 
          BOARD (inboard, isouth, jwest) +
          BOARD (inboard, isouth, j) + 
          BOARD (inboard, isouth, jeast);

      ret =  alivep (neighbor_count, BOARD (inboard, i, j));
      if (BOARD(outboard, i, j) != ret)
        BOARD(outboard, i, j) = ret;

    }
  }
  // Compute the bottom border
  for (i = rstart+len-1; i < rstart+len; i++) {
    for (j = cstart; j < cstart+len; j++) {
      inorth = mod (i-1, nrows);
      isouth = mod (i+1, nrows);
      jwest = mod (j-1, ncols);
      jeast = mod (j+1, ncols);

      neighbor_count = 
          BOARD (inboard, inorth, jwest) + 
          BOARD (inboard, inorth, j) + 
          BOARD (inboard, inorth, jeast) + 
          BOARD (inboard, i, jwest) +
          BOARD (inboard, i, jeast) + 
          BOARD (inboard, isouth, jwest) +
          BOARD (inboard, isouth, j) + 
          BOARD (inboard, isouth, jeast);

      ret =  alivep (neighbor_count, BOARD (inboard, i, j));
      if (BOARD(outboard, i, j) != ret)
        BOARD(outboard, i, j) = ret;

    }
  }
  // Compute the left border
  for (i = rstart; i < rstart+len-1; i++) {
    for (j = cstart; j < cstart+1; j++) {
      inorth = mod (i-1, nrows);
      isouth = mod (i+1, nrows);
      jwest = mod (j-1, ncols);
      jeast = mod (j+1, ncols);

      neighbor_count = 
          BOARD (inboard, inorth, jwest) + 
          BOARD (inboard, inorth, j) + 
          BOARD (inboard, inorth, jeast) + 
          BOARD (inboard, i, jwest) +
          BOARD (inboard, i, jeast) + 
          BOARD (inboard, isouth, jwest) +
          BOARD (inboard, isouth, j) + 
          BOARD (inboard, isouth, jeast);

      ret =  alivep (neighbor_count, BOARD (inboard, i, j));
      if (BOARD(outboard, i, j) != ret)
        BOARD(outboard, i, j) = ret;

    }
  }
  // Compute the right border
  for (i = rstart; i < rstart+len-1; i++) {
    for (j = cstart+len-1; j < cstart+len; j++) {
      isouth = mod (i+1, nrows);
      inorth = mod (i-1, nrows);
      jwest = mod (j-1, ncols);
      jeast = mod (j+1, ncols);

      neighbor_count = 
          BOARD (inboard, inorth, jwest) + 
          BOARD (inboard, inorth, j) + 
          BOARD (inboard, inorth, jeast) + 
          BOARD (inboard, i, jwest) +
          BOARD (inboard, i, jeast) + 
          BOARD (inboard, isouth, jwest) +
          BOARD (inboard, isouth, j) + 
          BOARD (inboard, isouth, jeast);

      ret =  alivep (neighbor_count, BOARD (inboard, i, j));
      if (BOARD(outboard, i, j) != ret)
        BOARD(outboard, i, j) = ret;

    }
  }
}


/*****************************************************************************
 * Game of life implementation
 ****************************************************************************/
char*
game_of_life (char* outboard, 
	      char* inboard,
	      const int nrows,
	      const int ncols,
	      const int gens_max) {

    const int LDA = nrows;
    int curgen, i, j;
    assert(nrows == ncols);

    for (curgen = 0; curgen < gens_max; curgen++) {
      struct arg_struct args [NUM_THREADS];
      pthread_t tid[NUM_THREADS];

      // Prepare the argument inputs
      for (i = 0; i < NUM_THREADS; i++) {
        args[i].inboard = inboard;
        args[i].outboard = outboard;
        args[i].len = nrows/2;
        args[i].nrows = nrows;
        args[i].ncols = ncols;
        args[i].LDA = LDA;
      }
      args[0].rstart = 0; args[0].cstart = 0;
      args[1].rstart = 0; args[1].cstart = nrows/2;
      args[2].rstart = nrows/2; args[2].cstart = 0;
      args[3].rstart = nrows/2; args[3].cstart = nrows/2;

      /*
       * Optimize by creating 4 threads (for 4 cores on the ug machines).
       */
      for (i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&tid[i], NULL, &do_work, (void*)&args[i]) != 0) {
          printf("thread creation error.\n");
          return -1;
        }
      }

      for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(tid[i], NULL);
      }

      SWAP_BOARDS( outboard, inboard );

    }
    return inboard;
}
