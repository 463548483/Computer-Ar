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

reg_t * alloc_region(size_t size);
reg_t * split(reg_t * curr, size_t size);
void use_region(reg_t * curr, size_t size);
void add_region(reg_t * free_region);
reg_t * merge(reg_t * left_region, reg_t * right_region);
void * ff_malloc(size_t size);
void ff_free(void * ptr);
void * bf_malloc(size_t size);
void bf_free(void * ptr);
void print_chain();
unsigned long get_data_segment_size();
unsigned long get_data_segment_free_space_size();
