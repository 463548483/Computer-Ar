#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct region_tag {
  size_t size;
  int is_free;  //0=use, 1=free
  struct region_tag * prev;
  struct region_tag * next;
};

typedef struct region_tag reg_t;

reg_t * alloc_region(size_t size, int sbrk_lock);
reg_t * split(reg_t * curr, size_t size);
void use_region(reg_t * curr,
                size_t size,
                reg_t ** head_free_region,
                reg_t ** tail_free_region);
void add_region(reg_t * free_region,
                reg_t ** head_free_region,
                reg_t ** tail_free_region);
reg_t * merge(reg_t * left_region, reg_t * right_region);
//void * ff_malloc(size_t size);
void ff_free(void * ptr, reg_t ** head_free_region, reg_t ** tail_free_region);
void * bf_malloc(size_t size,
                 reg_t ** head_free_region,
                 reg_t ** tail_free_region,
                 int sbrk_lock);
void bf_free(void * ptr, reg_t ** head_free_region, reg_t ** tail_free_region);
/* void print_chain(); */
/* unsigned long get_data_segment_size(); */
/* unsigned long get_data_segment_free_space_size(); */
//Thread Safe malloc/free: locking version
void * ts_malloc_lock(size_t size);
void ts_free_lock(void * ptr);

//Thread Safe malloc/free: non-locking version
void * ts_malloc_nolock(size_t size);
void ts_free_nolock(void * ptr);
