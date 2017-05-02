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
    "204287974",            /* UID */

    "Youngjun Park",          /* Full name */
    "parkyj2350@gmail.com",  /* Email */

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

    int starti = max(i-1,0);
    int endi = min(i+1, dim-1);
    int startj = max(j-1,0);
    int endj = min(j+1, dim-1);

    initialize_pixel_sum(&sum);
    for(ii = starti; ii <= endi; ii++) 
	for(jj = startj; jj <= endj; jj++) 
	    accumulate_sum(&sum, src[RIDX(ii, jj, dim)]);
    assign_sum_to_pixel(&current_pixel, sum);
    return current_pixel;
}




/******************************************************
 * Your different versions of the smooth kernel go here
 ******************************************************/
typedef struct{
  pthread_t thread_id;
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
    for (i = 0; i < dim; i++) {
       for (j = 0; j < dim; j++) {	
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
  int i,j;
  for (i = start; i < end; i++) {
    for (j = 0; j < dim; j++) {
      (ta->dst)[RIDX(i, j, dim)] = avg(dim, i, j, ta->src);
    }
  }
  return NULL;
}

char smooth_descr2[] = "smooth2: multhread only";
void smooth2(int dim, pixel *src, pixel *dst) 
{
  int NUM_THREADS = dim/16;
  int i = 0;
  int t;
  thread_args th_a[NUM_THREADS];
  for (i = 0; i < NUM_THREADS; i++) {
    th_a[i].thread_id = i;
    th_a[i].dim = dim;
    th_a[i].start = (dim/NUM_THREADS) * i;
    th_a[i].end = (dim/NUM_THREADS) * (i + 1);
    th_a[i].src = src;
    th_a[i].dst = dst;
    pthread_create(&(th_a[i].thread_id), NULL, thread_smooth, (void*) (th_a+i));
  }
  for (t = 0; t < NUM_THREADS; t++) {
    pthread_join(th_a[t].thread_id, NULL);
  }
}

typedef struct {
  int red;
  int green;
  int blue;
} pixel_cache;


void* thread_smooth21(void* arg)
{
  thread_args *ta = (thread_args*)arg;
  int dim = ta->dim;
  int start = ta->start;
  int end = ta->end;
  int i,j;

  for (i = start; i < end; i++) {
    for (j = 1; j < dim-1; j++) {
      int ii, jj;
      pixel_sum sum;
      pixel current_pixel;
      int starti = i-1;
      int endi = i+1;
      int startj = j-1;
      int endj = j+1;

      initialize_pixel_sum(&sum);
      for(ii = starti; ii <= endi; ii++)
        for(jj = startj; jj <= endj; jj++)
          accumulate_sum(&sum, ta->src[RIDX(ii, jj, dim)]);
      assign_sum_to_pixel(&current_pixel, sum);
      (ta->dst)[RIDX(i, j, dim)] = current_pixel;
    }
  }
  return NULL;
}

void* thread_smooth22(void* arg)
{
  thread_args *ta = (thread_args*)arg;
  int dim = ta->dim;
  int start = ta->start;
  int end = ta->end;
  int i,j;

  for (i = start; i < end; i++) {
    for (j = 1; j < dim-1; j++) {
      int ii, jj;
      pixel_sum sum;
      pixel current_pixel;

      int starti = i-1;
      int endi = i+1;
      int startj = j-1;
      int endj = j+1;

      initialize_pixel_sum(&sum);
      for(ii = starti; ii <= endi; ii++)
        for(jj = startj; jj <= endj; jj++)
          accumulate_sum(&sum, ta->src[RIDX(ii, jj, dim)]);
      assign_sum_to_pixel(&current_pixel, sum);
      (ta->dst)[RIDX(i, j, dim)] = current_pixel;
    }
  }
  return NULL;
}
void* thread_smooth23(void* arg)
{
  thread_args *ta = (thread_args*)arg;
  int dim = ta->dim;
  int start = ta->start;
  int end = ta->end;
  int i,j;

  for (i = start; i < end; i++) {
    for (j = 1; j < dim-1; j++) {
      int ii, jj;
      pixel_sum sum;
      pixel current_pixel;

      int starti = i-1;
      int endi = i+1;
      int startj = j-1;
      int endj = j+1;

      initialize_pixel_sum(&sum);
      for(ii = starti; ii <= endi; ii++)
        for(jj = startj; jj <= endj; jj++)
          accumulate_sum(&sum, ta->src[RIDX(ii, jj, dim)]);
      assign_sum_to_pixel(&current_pixel, sum);
      (ta->dst)[RIDX(i, j, dim)] = current_pixel;
    }
  }
  return NULL;
}
void* thread_smooth24(void* arg)
{
  thread_args *ta = (thread_args*)arg;
  int dim = ta->dim;
  int start = ta->start;
  int end = ta->end;
  int i,j;

  for (i = start; i < end; i++) {
    for (j = 1; j < dim-1; j++) {
      int ii, jj;
      pixel_sum sum;
      pixel current_pixel;

      int starti = i-1;
      int endi = i+1;
      int startj = j-1;
      int endj = j+1;

      initialize_pixel_sum(&sum);
      for(ii = starti; ii <= endi; ii++)
        for(jj = startj; jj <= endj; jj++)
          accumulate_sum(&sum, ta->src[RIDX(ii, jj, dim)]);
      assign_sum_to_pixel(&current_pixel, sum);
      (ta->dst)[RIDX(i, j, dim)] = current_pixel;
    }
  }
  return NULL;
}


/*
typedef struct {
  int red;
  int green;
  int blue;
} pixel_cache;
*/

/*
void* thread_smooth22(void* arg)
{
  thread_args *ta = (thread_args*)arg;
  pixel_cache cache;
  pixel *src = ta->src;

  
  cache.red = src->red;
  cache.green = src->green;
  cache.blue = src->blue;
  src++;
  cache.red += src->red;
  cache.green += src->green;
  cache.blue += src->blue;
  src+=dim;
  cache.red += src->red;
  cache.green += src->green;
  cache.blue += src->blue;
  src--;
  cache.red += src->red;
  cache.green += src->green;
  cache.blue += src->blue;
  src--;
  cache.red += src->red;
  cache.green += src->green;
  cache.blue += src->blue;
  src-=dim;
  cache.red += src->red;
  cache.green += src->green;
  cache.blue += src->blue;
  src-=dim;
  cache.red += src->red;
  cache.green += src->green;
  cache.blue += src->blue;
  src++;
  cache.red += src->red;
  cache.green += src->green;
  cache.blue += src->blue;
  src++;
  cache.red += src->red;
  cache.green += src->green;
  cache.blue += src->blue;
  (ta->dst)->red = cache.red/9;
  (ta->dst)->green = cache.green/9;
  (ta->dst)->blue = cache.blue/9;

}
*/



static void top(int pos,int dim,pixel *src,pixel *dst);
static void bottom(int pos,int dim,pixel *src,pixel *dst);
static void left(int pos,int dim,pixel *src,pixel *dst);
static void right(int pos,int dim,pixel *src,pixel *dst);
static void corners(int dim,pixel *src,pixel *dst);

char smooth_descr[] = "smooth: fastest version";
void smooth(int dim, pixel *src, pixel *dst)
{
  int NUM_THREADS = dim/16;
  int i,j,tl;

  corners(dim,src,dst);

  for(j = 1;j < dim-1;j++) {
    top(j,dim,src,dst);
    bottom((dim-1)*dim+j,dim,src,dst);
  }

  for(i = 1;i < dim-1;i++) {
    left(i*dim,dim,src,dst);
    right(i*dim+dim-1,dim,src,dst);
  }

  if (NUM_THREADS < 9){
    thread_args *th_a = malloc(sizeof(thread_args));
    th_a[tl].thread_id = tl;
    th_a[tl].dim = dim;
    th_a[tl].src = src;
    th_a[tl].dst = dst;
    th_a[tl].start = 1;
    th_a[tl].end = dim-1;
    thread_smooth21((void*)th_a);
  }

  else{
    thread_args th_a[16];
    for (i = 0; i < 4; i++) {
      tl = i*4;
      if (i == 0){
	th_a[tl].start = 1;
      }
      else{
	th_a[tl].start = NUM_THREADS * tl;
      }
      
      th_a[tl].thread_id = tl;
      th_a[tl].dim = dim;
      th_a[tl].src = src;
      th_a[tl].dst = dst;
      th_a[tl].end = NUM_THREADS * (tl + 1);
      pthread_create(&(th_a[tl].thread_id), NULL, thread_smooth21, (void*) (th_a+tl));

      th_a[tl+1].thread_id = tl + 1;
      th_a[tl+1].dim = dim;
      th_a[tl+1].src = src;
      th_a[tl+1].dst = dst;
      th_a[tl+1].start = NUM_THREADS * (tl + 1);
      th_a[tl+1].end = NUM_THREADS * (tl + 2);
      pthread_create(&(th_a[tl+1].thread_id), NULL, thread_smooth22, (void*) (th_a+tl+1));

      th_a[tl+2].thread_id = tl + 2;
      th_a[tl+2].dim = dim;
      th_a[tl+2].src = src;
      th_a[tl+2].dst = dst;
      th_a[tl+2].start = NUM_THREADS * (tl + 2);
      th_a[tl+2].end = NUM_THREADS * (tl + 3);
      pthread_create(&(th_a[tl+2].thread_id), NULL, thread_smooth23, (void*) (th_a+tl+2));
      
      if (i == 3){
	th_a[tl+3].end = dim-1;
      }
      else {
	th_a[tl+3].end = NUM_THREADS * (tl+4);
      }
	th_a[tl+3].thread_id = tl + 3;
	th_a[tl+3].dim = dim;
	th_a[tl+3].src = src;
	th_a[tl+3].dst = dst;
	th_a[tl+3].start = NUM_THREADS * (tl+3);
	pthread_create(&(th_a[tl+3].thread_id), NULL, thread_smooth24, (void*) (th_a+tl+3));
      
    }
    for (i = 0; i < 16; i++) {
      pthread_join(th_a[i].thread_id, NULL);
    }
  }
}


/*
void* thread_smooth3(void* arg)
{
  thread_args *ta = (thread_args*)arg;
  int dim = ta->dim;
  int start = ta->start;
  int end = ta->end;
  int i = start;
  int j = 0;
  
  for (i = start; i <= end; i++) {
    for (j = 0; j < dim; j++) {
      int ii, jj;
      pixel_sum sum;
      pixel current_pixel;

      int starti = max(i-1, 0);
      int endi = min(i+1, dim-1);
      int startj = max(j-1, 0);
      int endj = min(j+1, dim-1);
      
      initialize_pixel_sum(&sum);
      for(ii = starti; ii <= endi; ii++)
        for(jj = startj; jj <= endj; jj++)
	  accumulate_sum(&sum, ta->src[RIDX(ii, jj, dim)]);

      assign_sum_to_pixel(&current_pixel, sum);
      (ta->dst)[RIDX(i, j, dim)] = current_pixel;
    }
  }
  return NULL;
}
*/
/*
char smooth_descr3[] = "smooth: smooth3";
void smooth3(int dim, pixel *src, pixel *dst)
{
  int NUM_THREADS = dim/16;
  int i,tl;
  if (NUM_THREADS < 5){
    thread_args* th_a = malloc(sizeof(thread_args));
    th_a->thread_id = 1;
    th_a->dim = dim;
    th_a->src = src;
    th_a->dst = dst;
    //th_a->start = 0;
    //th_a->end = dim;
    smooth_helper((void*)th_a);
  }
  else {
    thread_args* th_a[16];
    for (i = 0; i < 16; i++){
      th_a[i] = malloc(sizeof(thread_args));
    }
    for (tl = 0; tl < 16; tl++) {
      th_a[tl]->thread_id = tl;
      th_a[tl]->dim = NUM_THREADS;
      th_a[tl]->src = src + (NUM_THREADS * tl);
      th_a[tl]->dst = dst + (NUM_THREADS * tl);
      th_a[tl]->start = NUM_THREADS * tl;
      th_a[tl]->end = NUM_THREADS * (tl + 1);
      pthread_create(&(th_a[tl]->thread_id), NULL, smooth_helper, (void*) (th_a+tl));
    }
    for (tl = 0; tl < 16; tl++) {
      pthread_join(th_a[tl]->thread_id, NULL);
    }
  }
}
*/

static void corners(int dim, pixel *src, pixel *dst) {
  pixel_cache cache;
  pixel result;
  
  cache.red = cache.green = cache.blue = 0; 
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  src++;
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  src+=dim;
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  src--;
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  src--;
  
  result.red   = cache.red>>2;
  result.green = cache.green>>2;
  result.blue  = cache.blue>>2;
  *dst = result;
  dst += dim-1;
  
  cache.red = cache.green = cache.blue = 0; 
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  src--;
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  src+=dim;
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  src++;
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  src+=1+dim*(dim-3);
  
  result.red   = cache.red>>2;
  result.green = cache.green>>2;
  result.blue  = cache.blue>>2;
  *dst = result;
  dst+=1+dim*(dim-2);
  
  cache.red = cache.green = cache.blue = 0; 
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  src++;
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  src-=dim;
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  src--;
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  src+=dim-1;
  
  result.red   = cache.red>>2;
  result.green = cache.green>>2;
  result.blue  = cache.blue>>2;
  *dst = result;
  dst+=dim-1;
  
  cache.red = cache.green = cache.blue = 0; 
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  src--;
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  src+=dim;
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  src++;
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  
  result.red   = cache.red>>2;
  result.green = cache.green>>2;
  result.blue  = cache.blue>>2;
  *dst = result;
}

static void top(int pos,int dim,pixel *src,pixel *dst) {
  pixel_cache cache;
  pixel result;
  cache.red = cache.green = cache.blue = 0; 
  
  src += pos;
  dst += pos;
  
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  src--;
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  src+=dim;
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  src++;
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  src++;
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  src-=dim;
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  
  result.red   = cache.red/6;
  result.green = cache.green/6;
  result.blue  = cache.blue/6;
  *dst = result;
}


static void left(int pos,int dim,pixel *src,pixel *dst) {
  pixel_cache cache;
  pixel result;
  cache.red = cache.green = cache.blue = 0; 
  
  src += pos;
  dst += pos;
  
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  src-=dim;
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  src++;
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  src+=dim;
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  src+=dim;
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  src--;
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  
  result.red   = cache.red/6;
  result.green = cache.green/6;
  result.blue  = cache.blue/6;
  *dst = result;
}
static void bottom(int pos,int dim,pixel *src,pixel *dst) {
  pixel_cache cache;
  pixel result;
  cache.red = cache.green = cache.blue = 0; 
  
  src += pos;
  dst += pos;
  
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  src++;
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  src-=dim;
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  src--;
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  src--;
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  src+=dim;
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  
  result.red   = cache.red/6;
  result.green = cache.green/6;
  result.blue  = cache.blue/6;
  *dst = result;
}

static void right(int pos,int dim,pixel *src,pixel *dst) {
  pixel_cache cache;
  pixel result;
  cache.red = cache.green = cache.blue = 0; 
  
  src += pos;
  dst += pos;
  
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  src-=dim;
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  src--;
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  src+=dim;
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  src+=dim;
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  src++;
  cache.red   += src->red;
  cache.green += src->green;
  cache.blue  += src->blue;
  
  result.red   = cache.red/6;
  result.green = cache.green/6;
  result.blue  = cache.blue/6;
  *dst = result;
}


/********************************************************************* 
 * register_smooth_functions - Register all of your different versions
 *     of the smooth kernel with the driver by calling the
 *     add_smooth_function() for each test function.  When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_smooth_functions() {
  //    add_smooth_function(&smooth2, smooth_descr2);
  //    add_smooth_function(&smooth4, smooth_descr4);
  //    add_smooth_function(&smooth3, smooth_descr3);
    add_smooth_function(&smooth2, smooth_descr2);
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



