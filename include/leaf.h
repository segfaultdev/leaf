#ifndef __LEAF_H__
#define __LEAF_H__

#include <stddef.h>
#include <stdint.h>

// leaf.c

void lf_assert(int condition, const char *format, ...);

// lf_alloc.c

void *lf_alloc(size_t size);
void *lf_realloc(void *ptr, size_t size);
void  lf_free(void *ptr);

size_t lf_peak_memory(void);
size_t lf_used_memory(void);

// lf_token.c

typedef struct lf_token_t lf_token_t;
typedef struct lf_source_t lf_source_t;

enum {
  // Invalid token
  
  lf_token_invalid,
  
  // All keywords
  
  lf_token_void,
  lf_token_bool,
  lf_token_byte,
  lf_token_sint,
  lf_token_usint,
  lf_token_int,
  lf_token_uint,
  lf_token_lint,
  lf_token_ulint,
  
  lf_token_null,
  lf_token_true,
  lf_token_false,
  
  lf_token_struct,
  lf_token_tuple,
  lf_token_union,
  lf_token_enum,
  
  lf_token_return,
  lf_token_if,
  lf_token_else,
  lf_token_while,
  lf_token_for,
  lf_token_break,
  lf_token_continue,
  
  lf_token_extern,
  ln_token_const,
  
  lf_token_sizeof,
  lf_token_typeof,
  
  lf_token_include,
  
  // Symbols
  
  lf_token_comma,
  lf_token_semicolon,
  
  lf_token_dot,
  lf_token_arrow,
  
  // Other tokens
  
  lf_token_identifier,
  
  lf_token_string_literal,
  lf_token_rune_literal,
  lf_token_number_literal,
};

struct lf_token_t {
  int line, column;
  
  const uint8_t *data; // Stored by reference to source buffer
  int size, length;
  
  int type;
};

struct lf_source_t {
  const char *path;
  
  uint8_t *data;
  int size;
  
  int offset, runes;
  int line, column;
  
  lf_token_t *tokens;
  int count;
  
  int last_column; // For unget_rune()
};

// Notes:
// - The function expects source.data to contain the actual loaded source code, and source.tokens to be null.
// - Every non-ASCII character (mainly UTF-8) found in a token is treated the same way as any letter (a-z, A-Z), even if it is not one.

void lf_token(lf_source_t *source);

#endif
