#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <leaf.h>

static void at_exit(void) {
  printf("(%lu bytes used, %lu peak)\n", lf_used_memory(), lf_peak_memory());
}

void lf_assert(int condition, const char *format, ...) {
  if (condition) {
    va_list args;
    va_start(args, format);
    
    vfprintf(stderr, format, args);
    va_end(args);
    
    putchar('\n');
    exit(1);
  }
}

int main(int argc, const char *argv[argc]) {
  atexit(at_exit);
  
  lf_source_t source = (lf_source_t) {
    .path = argv[1],
  };
  
  FILE *file = fopen(source.path, "rb");
  fseek(file, 0, SEEK_END);
  
  source.size = ftell(file);
  fseek(file, 0, SEEK_SET);
  
  source.data = lf_alloc(source.size);
  fread(source.data, 1, source.size, file);
  
  fclose(file);
  
  lf_token(&source);
  lf_free(source.data);
  
  return 0;
}