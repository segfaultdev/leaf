#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <leaf.h>

static size_t peak_memory = 0;
static size_t used_memory = 0;

void *lf_alloc(size_t size) {
  size_t *block = malloc(sizeof(size_t) + size);
  used_memory += size, *block = size;
  
  if (used_memory > peak_memory) {
    peak_memory = used_memory;
  }
  
  return (void *)(block + 1);
}

void *lf_realloc(void *ptr, size_t size) {
  size_t *block = NULL;
  
  if (ptr) {
    block = (size_t *)(ptr) - 1;
    used_memory -= *block;
  }
  
  block = realloc(block, sizeof(size_t) + size);
  used_memory += size, *block = size;
  
  if (used_memory > peak_memory) {
    peak_memory = used_memory;
  }
  
  return (void *)(block + 1);
}

void lf_free(void *ptr) {
  size_t *block = (size_t *)(ptr) - 1;
  used_memory -= *block;
  
  free(block);
}

size_t lf_peak_memory(void) {
  return peak_memory;
}

size_t lf_used_memory(void) {
  return used_memory;
}
