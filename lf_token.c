#include <stdint.h>
#include <leaf.h>

#define TOKEN_ASSERT(condition, format, ...) lf_assert(condition, "(Error @%s, line %d, column %d): " format, source->path, token.line + 1, token.column + 1, __VA_ARGS__)

static uint32_t get_rune(lf_source_t *source) {
  uint32_t rune;
  
  do {
    if (source->offset >= source->size) {
      return 0;
    }
    
    rune = source->data[source->offset++];
  } while (rune < 32 && rune != '\n' && rune != '\t');
  
  if (rune >= 0xF0) {
    rune = ((rune & 0x07) << 6) | (source->data[source->offset++] & 0x3F);
    rune = (rune << 6) | (source->data[source->offset++] & 0x3F);
    rune = (rune << 6) | (source->data[source->offset++] & 0x3F);
  } else if (rune >= 0xE0) {
    rune = ((rune & 0x0F) << 6) | (source->data[source->offset++] & 0x3F);
    rune = (rune << 6) | (source->data[source->offset++] & 0x3F);
  } else if (rune >= 0xC0) {
    rune = ((rune & 0x0F) << 6) | (source->data[source->offset++] & 0x3F);
  }
  
  source->last_column = source->column;
  
  if (rune == '\n') {
    source->line++;
    source->column = 0;
  } else {
    source->column++;
  }
  
  if (rune < 32) {
    rune = ' ';
  }
  
  source->runes++;
  return rune;
}

static void unget_rune(lf_source_t *source) {
  uint8_t byte;
  
  do {
    byte = source->data[--source->offset];
  } while ((byte & 0xC0) == 0x80);
  
  source->runes--, source->column--;
  
  if (source->column < 0) {
    source->line--;
    source->column = source->last_column;
  }
}

static void skip_spaces(lf_source_t *source) {
  while (source->offset < source->size) {
    uint32_t rune = get_rune(source);
    
    if (rune != ' ') {
      unget_rune(source);
      break;
    }
  }
}

static int is_digit(uint32_t rune, int base) {
  int result = (rune >= '0' && rune <= '1');
  
  if (base >= 8) {
    result |= (rune >= '2' && rune <= '7');
  }
  
  if (base >= 10) {
    result |= (rune >= '8' && rune <= '9');
  }
  
  if (base >= 16) {
    result |= (rune >= 'A' && rune <= 'F') | (rune >= 'a' && rune <= 'f');
  }
  
  return result;
}

static int is_alpha(uint32_t rune) {
  return (rune == '_' || rune == '$' || (rune >= 'A' && rune <= 'Z') || (rune >= 'a' && rune <= 'z') || rune > 127);
}

static int is_alnum(uint32_t rune) {
  return is_alpha(rune) || is_digit(rune, 10);
}

void lf_token(lf_source_t *source) {
  source->tokens = NULL;
  source->count = 0;
  
  source->offset = 0;
  source->runes = 0;
  
  source->line = 0;
  source->column = 0;
  
  for (;;) {
    skip_spaces(source);
    
    if (source->offset >= source->size) {
      break;
    }
    
    lf_token_t token = (lf_token_t) {
      .line = source->line,
      .column = source->column,
      
      .data = source->data + source->offset,
      .size = 0,
      .length = 0,
      
      .type = lf_token_invalid,
    };
    
    uint32_t rune = get_rune(source);
    int runes = source->runes;
    
    if (is_digit(rune, 10)) {
      token.type = lf_token_number_literal;
      int base = 10, weak_octal = 0, seen_dot = 0;
      
      if (rune == '0') {
        base = 8;
        rune = get_rune(source);
        
        if (rune == 'b') {
          base = 2;
        } else if (rune == 'o') {
          base = 8;
        } else if (rune == 'x') {
          base = 16;
        } else {
          weak_octal = 1;
          unget_rune(source);
        }
      }
      
      for (;;) {
        rune = get_rune(source);
        
        if (rune == '.') {
          TOKEN_ASSERT(seen_dot, "Number literal contains two dots.", 0);
          
          if (weak_octal) {
            base = 10;
          }
          
          seen_dot = 1;
        } else if (!is_digit(rune, base)) {
          TOKEN_ASSERT(is_alnum(rune), "Invalid rune in number literal: '%lc'.", rune);
          unget_rune(source);
          
          break;
        }
      }
    } else if (is_alpha(rune)) {
      token.type = lf_token_identifier;
      
      for (;;) {
        rune = get_rune(source);
        
        if (!is_alnum(rune)) {
          unget_rune(source);
          break;
        }
      }
    } else if (rune == '"') {
      token.type = lf_token_string_literal;
      
      for (;;) {
        rune = get_rune(source);
        
        if (rune == '"') {
          break;
        } else if (rune == '\\') {
          get_rune(source);
        }
      }
    } else if (rune == '\'') {
      token.type = lf_token_rune_literal;
      
      for (;;) {
        rune = get_rune(source);
        
        if (rune == '\'') {
          break;
        } else if (rune == '\\') {
          get_rune(source);
        }
      }
    } else if (rune == ',') {
      token.type = lf_token_comma;
    } else if (rune == ';') {
      token.type = lf_token_semicolon;
    } else if (rune == '.') {
      token.type = lf_token_dot;
    } else if (rune == '#') {
      token.type = lf_token_hash;
    } else if (rune == '(') {
      token.type = lf_token_left_parenthesis;
    } else if (rune == ')') {
      token.type = lf_token_right_parenthesis;
    } else if (rune == '[') {
      token.type = lf_token_left_bracket;
    } else if (rune == ']') {
      token.type = lf_token_right_bracket;
    } else if (rune == '{') {
      token.type = lf_token_left_brace;
    } else if (rune == '}') {
      token.type = lf_token_right_brace;
    } else if (rune == '=') {
      rune = get_rune(source);
      
      if (rune == '=') {
        token.type = lf_token_equals;
      } else {
        token.type = lf_token_assign;
        unget_rune(source);
      }
    } else if (rune == '+') {
      rune = get_rune(source);
      
      if (rune == '+') {
        token.type = lf_token_increment;
      } else if (rune == '=') {
        token.type = lf_token_assign_plus;
      } else {
        token.type = lf_token_plus;
        unget_rune(source);
      }
    } else if (rune == '-') {
      rune = get_rune(source);
      
      if (rune == '-') {
        token.type = lf_token_decrement;
      } else if (rune == '=') {
        token.type = lf_token_assign_minus;
      } else {
        token.type = lf_token_minus;
        unget_rune(source);
      }
    } else {
      TOKEN_ASSERT(1, "Unknown rune: '%lc'.", rune);
    }
    
    token.size = source->offset - (token.data - source->data);
    token.length = source->runes - runes;
    
    printf("'%.*s'\n", token.size, token.data);
    
    source->tokens = lf_realloc(source->tokens, (source->count + 1) * sizeof(lf_token_t));
    source->tokens[source->count++] = token;
  }
}
