/********************************************************
 * Kernels to be optimized for the CS:APP Performance Lab
 ********************************************************/

#include <stdlib.h>
#include "defs.h"

/* 
 * ECE454 Students: 
 * Please fill in the following team struct 
 */
team_t team = {
    "The Optimal Optimizers",              /* Team name */

    "Timmy Rong Tian Tse (998182657)",     /* First member full name */
    "timmy.tse@mail.utoronto.ca",  /* First member email address */

    "Yuan Xue (998275851)",                   /* Second member full name (leave blank if none) */
    "yuan.xue@mail.utoronto.ca"                    /* Second member email addr (leave blank if none) */
};

/***************
 * ROTATE KERNEL
 ***************/

/******************************************************
 * Your different versions of the rotate kernel go here
 ******************************************************/

/* 
 * naive_rotate - The naive baseline version of rotate 
 */
char naive_rotate_descr[] = "naive_rotate: Naive baseline implementation";
void naive_rotate(int dim, pixel *src, pixel *dst) 
{
    int i, j;

    for (i = 0; i < dim; i++)
	for (j = 0; j < dim; j++)
	    dst[RIDX(dim-1-j, i, dim)] = src[RIDX(i, j, dim)];

}

/*
 * ECE 454 Students: Write your rotate functions here:
 */ 

/* 
 * rotate - Your current working version of rotate
 * IMPORTANT: This is the version you will be graded on
 */
char rotate_descr[] = "rotate: Current working version";
void rotate(int dim, pixel *src, pixel *dst) 
{
  int i, j;
  int k, l;
  int m, n;
  int o, p;

  int lim1, lim2;

  int mem_x = 512;
  int mem_y = 256;
  int l2 = 32;
  int l1 = dim > 512 ? 8 : 16;

  for (j = 0; j < dim; j += mem_x) {
    lim1 = j + mem_x < dim ? j + mem_x: dim;
    for (i = 0; i < dim; i += mem_y) {
      lim2 = i + mem_y < dim ? i + mem_y: dim;
      for (l = j; l < lim1; l += l2){
        for (k = i; k < lim2; k += l2) {
          for (m = l; m < l + l2; m += l1) {
            for (n = k; n < k + l2; n += l1) {
              for (o = m; o < m + l1; o++) {
                for (p = n; p < n + l1; p++) {
                  dst[RIDX(dim-1-o, p, dim)] = src[RIDX(p, o, dim)];
                }
              }
            }
          }
        }
      }
    }
  }
}
 

/* 
 * second attempt
*/
char rotate_two_descr[] = "second attempt";
void attempt_two(int dim, pixel *src, pixel *dst) 
{
  int i, j;
  int k, l;

  int mac = 16;

//  for (j = 0; j < dim; j += mac) {
//    for (i = 0; i < dim; i += mac) {
//      for (l = 0; l < mac; l++) {
//        for (k = 0; k < mac; k++) {
//	        dst[RIDX(dim-1-(j + l), i + k, dim)] = src[RIDX(i + k, j + l, dim)];
//        }
//      }
//    }
//  }

//  int jl, ik;
  for (j = 0; j < dim; j += mac) {
    for (i = 0; i < dim; i += mac) {
      for (l = 0; l < mac; l++) {
        for (k = 0; k < mac; k++) {
	        dst[RIDX(dim-1-(j + l), i + k, dim)] = src[RIDX(i + k, j + l, dim)];
        }
      }
    }
  }

}

/* 
 * third attempt
*/
char rotate_three_descr[] = "third attempt";
void attempt_three(int dim, pixel *src, pixel *dst) 
{

  int i, j, k, l, m, n;
  int l1 = 16;
  int l2 = 73;

  for (i = 0; i < dim; i += l2) {
    for (j = 0; j < dim; j += l2) {

      for (k = i; k < i + l2; k += l1) {
        for (l = j; l < j + l2; l += l1) {
        //if (k >= dim) break;
          //if (l >= dim) break;

          for (m = k; m < k + l1; m++) {
            if (m >= dim) break;
            for (n = l; n < l + l1; n++) {
            if (n >= dim) break;
	            dst[RIDX(dim-1-n, m, dim)] = src[RIDX(m, n, dim)];
            }
          }

        }
      }

    }
  }

}

/* 
 * fourth attempt
*/
char rotate_four_descr[] = "fourth attempt";
void attempt_four(int dim, pixel *src, pixel *dst) 
{
  int i, j;
  int k, l;
  int ik, jl;
  int mac = 64;

    for (j = 0; j < dim; j += mac) {
  for (i = 0; i < dim; i += mac) {
      for (k = 0; k < mac; k++) {
        ik = i + k;
        if (ik >= dim) break;
        for (l = 0; l < mac; l++) {
          jl = j + l;
          if (jl >= dim) break;
	        dst[RIDX(dim-1-jl, ik, dim)] = src[RIDX(ik, jl, dim)];
        }
      }
    }
  }
}

/* 
 * fifth attempt
*/
char rotate_five_descr[] = "fifth attempt";
void attempt_five(int dim, pixel *src, pixel *dst) 
{
  if (dim <= 512) {
    int i, j;
    int k, l;
    int m, n;

    int l2 = 32;
    int l1 = 16;

    for (j = 0; j < dim; j += l2) {
      for (i = 0; i < dim; i += l2) {
        for (l = j; l < j + l2; l += l1){
          for (k = i; k < i + l2; k += l1) {
            for (m = l; m < l + l1; m++) {
              for (n = k; n < k + l1; n++) {
                dst[RIDX(dim-1-m, n, dim)] = src[RIDX(n, m, dim)];
              }
            }
          }
        }
      }
    }
  } else {
  int i, j;
  int k, l;
  int m, n;
  int o, p;

  int l2 = 256;
  int l1 = 32;
  int l0 = 8;

  int size = dim/l2;
  for (j = 0; j < size*l2; j += l2) {
    for (i = 0; i < size*l2; i += l2) {
      for (l = j; l < j + l2; l += l1){
        for (k = i; k < i + l2; k += l1) {
          for (m = l; m < l + l1; m += l0) {
            for (n = k; n < k + l1; n += l0) {
              for (o = m; o < m + l0; o++) {
                for (p = n; p < n + l0; p++) {
                  dst[RIDX(dim-1-o, p, dim)] = src[RIDX(p, o, dim)];
                }
              }
            }
          }
        }
      }
    }
  }

  for (j = size*l2; j < dim; j += l1) {
    for (i = 0; i < dim; i += l1) {
      for (l = j; l < j + l1; l += l0) {
        for (k = i; k < i + l1; k += l0) {
          for (m = l; m < l + l0; m++) {
            for (n = k; n < k + l0; n++) {
              dst[RIDX(dim-1-m, n, dim)] = src[RIDX(n, m, dim)];
            }
          }
        }
      }
    }
  }

  for (j = 0; j < size*l2 ; j += l1) {
    for (i = size*l2; i < dim; i += l1) {
      for (l = j; l < j + l1; l += l0){
        for (k = i; k < i + l1; k += l0) {
          for (m = l; m < l + l0; m++) {
            for (n = k; n < k + l0; n++) {
              dst[RIDX(dim-1-m, n, dim)] = src[RIDX(n, m, dim)];
            }
          }
        }
      }
    }
  }





  }


}

/* 
 * sixth attempt
*/
char rotate_six_descr[] = "sixth attempt";
void attempt_six(int dim, pixel *src, pixel *dst) 
{
  // 2.9/3.0
  if (dim%128!= 0) {
    int i, j;
    int k, l;
    int m, n;

    int l2 = 32;
    int l1 = 16;

    for (j = 0; j < dim; j += l2) {
      for (i = 0; i < dim; i += l2) {
        for (l = j; l < j + l2; l += l1){
          for (k = i; k < i + l2; k += l1) {
            for (m = l; m < l + l1; m++) {
              for (n = k; n < k + l1; n++) {
                dst[RIDX(dim-1-m, n, dim)] = src[RIDX(n, m, dim)];
              }
            }
          }
        }
      }
    }
  } else {

    int i, j;
    int k, l;
    int m, n;
    int o, p;

    int l2 = 128;
    int l1 = 32;
    int l0 = 16;

    for (j = 0; j < dim; j += l2) {
      for (i = 0; i < dim; i += l2) {
        for (l = j; l < j + l2; l += l1){
          for (k = i; k < i + l2; k += l1) {
            for (m = l; m < l + l1; m += l0) {
              for (n = k; n < k + l1; n += l0) {
                for (o = m; o < m + l0; o++) {
                  for (p = n; p < n + l0; p++) {
                    dst[RIDX(dim-1-o, p, dim)] = src[RIDX(p, o, dim)];
                  }
                }
              }
            }
          }
        }
      }
    }
  }

#if 0
  if (j > dim) j -= l2;
  if (i > dim) i -= l2;
      for (l = j; l < j + l2; l += l1){
        for (k = i; k < i + l2; k += l1) {
          for (m = l; m < l + l1; m += l0) {
            for (n = k; n < k + l1; n += l0) {
              for (o = m; o < m + l0; o++) {
                for (p = n; p < n + l0; p++) {
	                dst[RIDX(dim-1-o, p, dim)] = src[RIDX(p, o, dim)];
                }
              }
            }
          }
        }
      }
#endif
}

/* 
 * seventh attempt
*/
char rotate_seven_descr[] = "seventh attempt";
void attempt_seven(int dim, pixel *src, pixel *dst) 
{
  int i, j;
  int k, l;
  int m, n;
  int o, p;

  int l2 = 256;
  int l1 = 32;
  int l0 = dim > 512 ? 8 : 16;

  int size = dim/l2;
  for (j = 0; j < size*l2; j += l2) {
    for (i = 0; i < size*l2; i += l2) {
      for (l = j; l < j + l2; l += l1){
        for (k = i; k < i + l2; k += l1) {
          for (m = l; m < l + l1; m += l0) {
            for (n = k; n < k + l1; n += l0) {
              for (o = m; o < m + l0; o++) {
                for (p = n; p < n + l0; p++) {
                  dst[RIDX(dim-1-o, p, dim)] = src[RIDX(p, o, dim)];
                }
              }
            }
          }
        }
      }
    }
  }

  for (j = size*l2; j < dim; j += l1) {
    for (i = 0; i < dim; i += l1) {
      for (l = j; l < j + l1; l += l0) {
        for (k = i; k < i + l1; k += l0) {
          for (m = l; m < l + l0; m++) {
            for (n = k; n < k + l0; n++) {
              dst[RIDX(dim-1-m, n, dim)] = src[RIDX(n, m, dim)];
            }
          }
        }
      }
    }
  }

  for (j = 0; j < size*l2 ; j += l1) {
    for (i = size*l2; i < dim; i += l1) {
      for (l = j; l < j + l1; l += l0){
        for (k = i; k < i + l1; k += l0) {
          for (m = l; m < l + l0; m++) {
            for (n = k; n < k + l0; n++) {
              dst[RIDX(dim-1-m, n, dim)] = src[RIDX(n, m, dim)];
            }
          }
        }
      }
    }
  }
}

/* 
 * eighth attempt
*/
char rotate_eight_descr[] = "eighth attempt";
void attempt_eight(int dim, pixel *src, pixel *dst) 
{
  int i, j;
  int k, l;
  int m, n;
  int o, p;

  int lim1, lim2;

  int l2_x = 512;
  int l2_y = 256;
  int l1 = 32;
  int l0 = 16;


  for (j = 0; j < dim; j += l2_x) {
    lim1 = j + l2_x < dim ? j + l2_x: dim;
    for (i = 0; i < dim; i += l2_y) {
      lim2 = i + l2_y < dim ? i + l2_y: dim;
      for (l = j; l < lim1; l += l1){
        for (k = i; k < lim2; k += l1) {
          for (m = l; m < l + l1; m += l0) {
            for (n = k; n < k + l1; n += l0) {
              for (o = m; o < m + l0; o++) {
                for (p = n; p < n + l0; p += 16) {
                  dst[RIDX(dim-1-o, p + 0, dim)] = src[RIDX(p + 0, o, dim)];
                  dst[RIDX(dim-1-o, p + 1, dim)] = src[RIDX(p + 1, o, dim)];
                  dst[RIDX(dim-1-o, p + 2, dim)] = src[RIDX(p + 2, o, dim)];
                  dst[RIDX(dim-1-o, p + 3, dim)] = src[RIDX(p + 3, o, dim)];
                  dst[RIDX(dim-1-o, p + 4, dim)] = src[RIDX(p + 4, o, dim)];
                  dst[RIDX(dim-1-o, p + 5, dim)] = src[RIDX(p + 5, o, dim)];
                  dst[RIDX(dim-1-o, p + 6, dim)] = src[RIDX(p + 6, o, dim)];
                  dst[RIDX(dim-1-o, p + 7, dim)] = src[RIDX(p + 7, o, dim)];
                  dst[RIDX(dim-1-o, p + 8, dim)] = src[RIDX(p + 8, o, dim)];
                  dst[RIDX(dim-1-o, p + 9, dim)] = src[RIDX(p + 9, o, dim)];
                  dst[RIDX(dim-1-o, p + 10, dim)] = src[RIDX(p + 10, o, dim)];
                  dst[RIDX(dim-1-o, p + 11, dim)] = src[RIDX(p + 11, o, dim)];
                  dst[RIDX(dim-1-o, p + 12, dim)] = src[RIDX(p + 12, o, dim)];
                  dst[RIDX(dim-1-o, p + 13, dim)] = src[RIDX(p + 13, o, dim)];
                  dst[RIDX(dim-1-o, p + 14, dim)] = src[RIDX(p + 14, o, dim)];
                  dst[RIDX(dim-1-o, p + 15, dim)] = src[RIDX(p + 15, o, dim)];
                }
              }
            }
          }
        }
      }
    }
  }
}



/*********************************************************************
 * register_rotate_functions - Register all of your different versions
 *     of the rotate kernel with the driver by calling the
 *     add_rotate_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_rotate_functions() 
{
    add_rotate_function(&naive_rotate, naive_rotate_descr);
    add_rotate_function(&rotate, rotate_descr);
    add_rotate_function(&attempt_two, rotate_two_descr);
    add_rotate_function(&attempt_three, rotate_three_descr);   
    add_rotate_function(&attempt_four, rotate_four_descr);   
    add_rotate_function(&attempt_five, rotate_five_descr);   
    add_rotate_function(&attempt_six, rotate_six_descr);   
    add_rotate_function(&attempt_seven, rotate_seven_descr);   
    add_rotate_function(&attempt_eight, rotate_eight_descr);   

    //add_rotate_function(&attempt_nine, rotate_nine_descr);   
    //add_rotate_function(&attempt_ten, rotate_ten_descr);   
    //add_rotate_function(&attempt_eleven, rotate_eleven_descr);   

    /* ... Register additional rotate functions here */
}

