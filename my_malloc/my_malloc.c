
#include "my_malloc.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

reg_t * head_free_region = NULL;
reg_t * tail_free_region = NULL;
unsigned long total_region_size = 0;
unsigned long free_region_size = 0;

reg_t * alloc_region(size_t size) {
  reg_t * new_reg = sbrk(size + sizeof(reg_t));
  if (new_reg == (void *)-1) {
    fprintf(stderr, "space full\n");
    return NULL;
  }
  new_reg->size = size;
  new_reg->is_free = 0;
  new_reg->next = NULL;
  new_reg->prev = NULL;
  total_region_size += size + sizeof(reg_t);
  //tail_free_region=new_reg;
  //if (head_free_region==NULL){
  //  head_free_region=new_reg;
  //}
  return new_reg;
}
void use_region(reg_t * curr, size_t size) {
  //delete region from linkedlist
  if (head_free_region == tail_free_region && head_free_region == curr) {
    head_free_region = NULL;
    tail_free_region = NULL;
  }
  else if (head_free_region == curr) {
    head_free_region = curr->next;
    curr->next->prev = NULL;
  }
  else if (tail_free_region == curr) {
    tail_free_region = curr->prev;
    curr->prev->next = NULL;
  }
  else {
    curr->prev->next = curr->next;
    curr->next->prev = curr->prev;
  }
  curr->is_free = 0;
  free_region_size -= curr->size + sizeof(reg_t);
  curr->prev = NULL;
  curr->next = NULL;
  reg_t * p = split(curr, size);
  if (p != NULL) {  //if split region, add back
    add_region(p);
    free_region_size += p->size + sizeof(reg_t);
  }

  return;
}

reg_t * split(reg_t * curr, size_t size) {
  if (sizeof(reg_t) >= (curr->size - size)) {
    return NULL;
  }
  else {
    reg_t * new_region;
    new_region = (reg_t *)((char *)curr + sizeof(reg_t) + size);
    new_region->size = curr->size - size - sizeof(reg_t);
    curr->size = size;
    new_region->is_free = 1;
    new_region->prev = curr->prev;
    new_region->next = curr->next;
    return new_region;
  }
}

void * ff_malloc(size_t size) {
  //if has free region
  ////find first fit region and remove it from linked list
  if (head_free_region != NULL) {
    reg_t * curr = head_free_region;
    while (curr && curr <= tail_free_region) {
      if (size <= curr->size) {
        use_region(curr, size);
        curr->is_free = 0;
        return (void *)((char *)curr + sizeof(reg_t));
      }
      curr = curr->next;
    }
  }
  //if not has free region
  ////alloc new region
  return (void *)((char *)alloc_region(size) + sizeof(reg_t));
}

reg_t * merge(reg_t * left_region, reg_t * right_region) {
  if ((char *)left_region + left_region->size + sizeof(reg_t) != (char *)right_region) {
    return right_region;
  }
  else {
    left_region->size += right_region->size + sizeof(reg_t);
    left_region->next = right_region->next;
    if (left_region->next) {
      left_region->next->prev = left_region;
    }
    right_region->prev = NULL;
    right_region->next = NULL;
    return left_region;
  }
}

void add_region(reg_t * free_region) {
  free_region_size += free_region->size + sizeof(reg_t);
  //add region in the head
  if (head_free_region == NULL || free_region < head_free_region) {
    if (tail_free_region == NULL) {
      tail_free_region = free_region;
    }
    else {
      free_region->next = head_free_region;
      head_free_region->prev = free_region;
      merge(free_region, head_free_region);
    }
    head_free_region = free_region;
    return;
  }
  else {  //add region in the middle
    reg_t * curr = head_free_region;
    while (curr && curr <= tail_free_region) {
      if (curr < free_region) {
        curr = curr->next;
      }
      else {
        //curr->prev->next=free_region;
        free_region->prev = curr->prev;
        free_region->next = curr;
        curr->prev->next = free_region;
        curr->prev = free_region;
        reg_t * merge_region = merge(free_region->prev, free_region);
        merge_region = merge(merge_region, merge_region->next);
        if (curr == tail_free_region && merge_region != tail_free_region) {
          tail_free_region = merge_region;
        }
        return;
      }
    }
    //free_region in the end
    tail_free_region->next = free_region;
    free_region->prev = tail_free_region;

    reg_t * merge_region = merge(tail_free_region, free_region);
    tail_free_region = merge_region;
  }
}

void ff_free(void * ptr) {
  if (ptr == NULL) {
    return;
  }
  //add the region back to linked list
  reg_t * free_region = (reg_t *)ptr - 1;
  if (free_region->is_free == 0) {
    free_region->is_free = 1;
    add_region(free_region);
  }
  else {
    fprintf(stderr, "double free\n");
    return;
  }
}

void * bf_malloc(size_t size) {
  //if has free region
  //find best fit region and remove it from linked list
  if (head_free_region != NULL) {
    reg_t * curr = head_free_region;
    reg_t * use = NULL;
    while (curr && curr <= tail_free_region) {
      if (size <= curr->size && (use == NULL || use->size > curr->size)) {
        use = curr;
      }
      curr = curr->next;
      ;
    }
    if (use != NULL) {
      use_region(use, size);
      return (void *)((char *)use + sizeof(reg_t));
    }
  }
  //if not has free region
  ////alloc new region
  return (void *)((char *)alloc_region(size) + sizeof(reg_t));
}

void bf_free(void * ptr) {
  //same as ff_free
  ff_free(ptr);
}

void print_chain() {
  reg_t * curr = head_free_region;
  int i = 0;
  while (curr && curr <= tail_free_region) {
    i += 1;
    printf("region%p %ld\n", (void *)curr, curr->size);
    curr = curr->next;
  }
  printf("data_segment_size = %lu, data_segment_free_space = %lu\n",
         total_region_size,
         free_region_size);
}

unsigned long get_data_segment_size() {
  return total_region_size;
}

unsigned long get_data_segment_free_space_size() {
  return free_region_size;
}
//#define func_malloc(size) ff_malloc(size)
//#define func_free(ptr) ff_free(ptr)
//#ifdef FF

/* #define NUM_ITERS 1 */
/* #define NUM_ITEMS 10 */

/* #define MALLOC(sz) ff_malloc(sz) */
/* #define FREE(p) ff_free(p) */
/* //#endif */
/* //#ifdef BF */
/* //#define MALLOC(sz) bf_malloc(sz) */
/* //#define FREE(p) bf_free(p) */
/* //#endif */
/* // */

/* double calc_time(struct timespec start, struct timespec end) { */
/*   double start_sec = (double)start.tv_sec * 1000000000.0 + (double)start.tv_nsec; */
/*   double end_sec = (double)end.tv_sec * 1000000000.0 + (double)end.tv_nsec; */

/*   if (end_sec < start_sec) { */
/*     return 0; */
/*   } */
/*   else { */
/*     return end_sec - start_sec; */
/*   } */
/* }; */

/* struct malloc_list { */
/*   size_t bytes; */
/*   int * address; */
/* }; */
/* typedef struct malloc_list malloc_list_t; */

/* malloc_list_t malloc_items[0][NUM_ITEMS]; */

/* unsigned free_list[NUM_ITEMS]; */
/* int main(int argc, char * argv[]) { */
/*   int i, j, k; */
/*   unsigned tmp; */
/*   unsigned long data_segment_size; */
/*   unsigned long data_segment_free_space; */
/*   struct timespec start_time, end_time; */

/*   srand(0); */

/*   const unsigned chunk_size = 32; */
/*   const unsigned min_chunks = 4; */
/*   const unsigned max_chunks = 16; */
/*   for (i = 0; i < NUM_ITEMS; i++) { */
/*     malloc_items[0][i].bytes = */
/*         ((rand() % (max_chunks - min_chunks + 1)) + min_chunks) * chunk_size; */
/*     malloc_items[1][i].bytes = */
/*         ((rand() % (max_chunks - min_chunks + 1)) + min_chunks) * chunk_size; */
/*     free_list[i] = i; */
/*   }  //for i */

/*   i = NUM_ITEMS; */
/*   while (i > 1) { */
/*     i--; */
/*     j = rand() % i; */
/*     tmp = free_list[i]; */
/*     free_list[i] = free_list[j]; */
/*     free_list[j] = tmp; */
/*   }  //while */

/*   for (i = 0; i < NUM_ITEMS; i++) { */
/*     malloc_items[0][i].address = (int *)MALLOC(malloc_items[0][i].bytes); */
/*   }  //for i */

/*   //Start Time */
/*   clock_gettime(CLOCK_MONOTONIC, &start_time); */

/*   for (i = 0; i < NUM_ITERS; i++) { */
/*     unsigned malloc_set = i % 2; */
/*     for (j = 0; j < NUM_ITEMS; j += 50) { */
/*       print_chain(); */
/*       for (k = 0; k < 50; k++) { */
/*         int item_to_free = free_list[j + k]; */
/*         FREE(malloc_items[malloc_set][item_to_free].address); */
/*       }  //for k */
/*       for (k = 0; k < 50; k++) { */
/*         malloc_items[1 - malloc_set][j + k].address = */
/*             (int *)MALLOC(malloc_items[1 - malloc_set][j + k].bytes); */
/*       }  //for k */
/*     }    //for j */
/*   }      //for i */

/*   //Stop Time */
/*   clock_gettime(CLOCK_MONOTONIC, &end_time); */

/*   data_segment_size = get_data_segment_size(); */
/*   data_segment_free_space = get_data_segment_free_space_size(); */
/*   printf("data_segment_size = %lu, data_segment_free_space = %lu\n", */
/*          data_segment_size, */
/*          data_segment_free_space); */

/*   double elapsed_ns = calc_time(start_time, end_time); */
/*   printf("Execution Time = %f seconds\n", elapsed_ns / 1e9); */
/*   printf("Fragmentation  = %f\n", */
/*          (float)data_segment_free_space / (float)data_segment_size); */

/*   for (i = 0; i < NUM_ITEMS; i++) { */
/*     FREE(malloc_items[0][i].address); */
/*   }  //for i */

/*   return 0; */
/* } */
