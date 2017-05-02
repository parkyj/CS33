/********************************************************
 * Kernels to be optimized for the CS:APP Performance Lab
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include <pthread.h>
#include <semaphore.h>

/* 
 * Please fill in the following team struct 
 */
team_t team = {
    "1232131",            /* UID */

    "Your Name",          /* Full name */
    "yourname@ucla.edu",  /* Email */

    "",                   /* Leave Blank */
    ""                    /* Leave Blank */
};

//  You should modify only this file -- feel free to modify however you like!

/*
 * setup - This function runs one time, and will not be timed.
 *         You can do whatever initialization you need here, but
 *         it is not required -- don't use if you don't want to.
 */
void setup() {
  
}

/***************
 * SMOOTH KERNEL
 **************/

/***************************************************************
 * Various typedefs and helper functions for the smooth function
 * You may modify these any way you like.
 **************************************************************/

/* A struct used to compute averaged pixel value */
typedef struct {
    int red;
    int green;
    int blue;
    int num;
} pixel_sum;

/* Compute min and max of two integers, respectively */
static int min(int a, int b) { return (a < b ? a : b); }
static int max(int a, int b) { return (a > b ? a : b); }

/* 
 * initialize_pixel_sum - Initializes all fields of sum to 0 
 */
static void initialize_pixel_sum(pixel_sum *sum) 
{
    sum->red = sum->green = sum->blue = 0;
    sum->num = 0;
    return;
}

/* 
 * accumulate_sum - Accumulates field values of p in corresponding 
 * fields of sum 
 */
static void accumulate_sum(pixel_sum *sum, pixel p) 
{
    sum->red += (int) p.red;
    sum->green += (int) p.green;
    sum->blue += (int) p.blue;
    sum->num++;
    return;
}

/* 
 * assign_sum_to_pixel - Computes averaged pixel value in current_pixel 
 */
static void assign_sum_to_pixel(pixel *current_pixel, pixel_sum sum) 
{
    current_pixel->red = (unsigned short) (sum.red/sum.num);
    current_pixel->green = (unsigned short) (sum.green/sum.num);
    current_pixel->blue = (unsigned short) (sum.blue/sum.num);
    return;
}

/* 
 * avg - Returns averaged pixel value at (i,j) 
 */
static pixel avg(int dim, int i, int j, pixel *src) 
{
    int ii, jj;
    pixel_sum sum;
    pixel current_pixel;

    initialize_pixel_sum(&sum);
    for(ii = max(i-1, 0); ii <= min(i+1, dim-1); ii++) 
	for(jj = max(j-1, 0); jj <= min(j+1, dim-1); jj++) 
	    accumulate_sum(&sum, src[RIDX(ii, jj, dim)]);

    assign_sum_to_pixel(&current_pixel, sum);
    return current_pixel;
}




/******************************************************
 * Your different versions of the smooth kernel go here
 ******************************************************/
typedef struct{
  int dim;
  int start;
  int end;
  pixel *src;
  pixel *dst;
} thread_args;


/*
 * naive_smooth - The naive baseline version of smooth 
 */
char naive_smooth_descr[] = "naive_smooth: Naive baseline implementation";
void naive_smooth(int dim, pixel *src, pixel *dst) 
{
    int i, j;

    for (j = 0; j < dim; j++) {
       for (i = 0; i < dim; i++) {	
           dst[RIDX(i, j, dim)] = avg(dim, i, j, src);
       }
    }
}

/*
 * smooth - Your current working version of smooth. 
 * IMPORTANT: This is the version you will be graded on
 */
void* thread_smooth(void* arg)
{
  thread_args *ta = (thread_args*)arg;
  int dim = ta->dim;
  int start = ta->start;
  int end = ta->end;
  pixel *src = ta->src;
  pixel *dst = ta->dst;
  int i, j;

  for (i = start; i < end; i++) {
    for (j = 0; j < dim; j++) {
      dst[RIDX(i, j, dim)] = avg(dim, i, j, src);
    }
  }
  return NULL;
}


char smooth_descr[] = "smooth: Current working version";
void smooth(int dim, pixel *src, pixel *dst) 
{
  int NUM_THREADS = dim/16;
  pthread_t threads[NUM_THREADS];
  long rc;
  int i = 0;
  thread_args *th_a = malloc(sizeof(thread_args));
  (*th_a).dim = dim;
  (*th_a).src = src;
  (*th_a).dst = dst;
  for (i = 0; i < NUM_THREADS; i++) {
    (*th_a).start = (dim/NUM_THREADS) * i;
    (*th_a).end = (dim/NUM_THREADS) * (i + 1) - 1;
    rc = pthread_create(threads + i, NULL, thread_smooth, (void*) th_a);
    /*if (rc) {
      exit(-1);
      }*/
  }
  for (i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }
  pthread_exit(NULL);
}


/********************************************************************* 
 * register_smooth_functions - Register all of your different versions
 *     of the smooth kernel with the driver by calling the
 *     add_smooth_function() for each test function.  When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_smooth_functions() {
    add_smooth_function(&smooth, smooth_descr);
    add_smooth_function(&naive_smooth, naive_smooth_descr);
    /* ... Register additional test functions here */
}






// IGNORE EVERYTHING AFTER THIS POINT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!111
// You are not implementing rotate


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
 * rotate - Your current working version of rotate
 * IMPORTANT: This is the version you will be graded on
 */
char rotate_descr[] = "rotate: Current working version";
void rotate(int dim, pixel *src, pixel *dst) 
{
    naive_rotate(dim, src, dst);
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
    /* ... Register additional test functions here */
}



