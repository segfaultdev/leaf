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

struct lf_token_t {
  int line, column;
  
  const uint8_t *data; // Stored by reference to source buffer
  int size, length;
  
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
    lf_token_hash,
    
    lf_token_left_parenthesis,
    lf_token_right_parenthesis,
    lf_token_left_bracket,
    lf_token_right_bracket,
    lf_token_left_brace,
    lf_token_right_brace,
    
    lf_token_assign,
    lf_token_greater,
    lf_token_less,
    lf_token_plus,
    lf_token_minus,
    lf_token_asterisk,
    lf_token_power,
    lf_token_slash,
    lf_token_percent,
    lf_token_and,
    lf_token_or,
    lf_token_xor,
    lf_token_not,
    lf_token_bool_and,
    lf_token_bool_or,
    lf_token_bool_not,
    lf_token_left_shift,
    lf_token_right_shift,
    
    lf_token_equals,
    lf_token_not_equals,
    lf_token_greater_equals,
    lf_token_less_equals,
    lf_token_assign_plus,
    lf_token_assign_minus,
    lf_token_assign_asterisk,
    lf_token_assign_power,
    lf_token_assign_slash,
    lf_token_assign_percent,
    lf_token_assign_and,
    lf_token_assign_or,
    lf_token_assign_xor,
    lf_token_assign_not,
    lf_token_assign_left_shift,
    lf_token_assign_right_shift,
    
    lf_token_increment,
    lf_token_decrement,
    
    // Other tokens
    
    lf_token_identifier,
    
    lf_token_string_literal,
    lf_token_rune_literal,
    lf_token_number_literal,
  } type;
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

// lf_type.c

typedef struct lf_type_t lf_type_t;

struct lf_type_t {
  enum {
    lf_type_integer,
    lf_type_bool,
    lf_type_struct,
    lf_type_tuple,
    lf_type_union,
    lf_type_pointer,
  };
  
  union {
    int integer_size;
    
    struct {
      lf_type_t *struct_types;
      lf_token_t *struct_names;
      
      int struct_count;
    };
    
    struct {
      lf_type_t *tuple_types;
      int tuple_count;
    };
    
    struct {
      lf_type_t *union_types;
      lf_token_t *union_names;
      
      int union_count;
    };
    
    lf_type_t *pointer_type;
  };
};

#endif
