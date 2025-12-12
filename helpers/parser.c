#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "parser.h"

int is_token_char(int c) {
  const char *char_set = "-@.+*^,[]#(){}:";
  return isalnum(c) || strchr(char_set, c) != NULL;
}

int init_parser(struct parser_context *ctx, const char *filename, const char *filemode) {
  ctx->fp = fopen(filename, filemode);
  if (!ctx->fp) {
    printf("ERROR: Can't open %s\n", filename);
    return 1;
  }
}

#define TOKEN_OK 1
#define TOKEN_EOF 0
#define TOKEN_ERR -1

int next_token(struct parser_context *ctx, const char *delims, bool whitespace) {
  int c;
  int i = 0;
  memset(ctx->buffer, 0, sizeof(ctx->buffer));
  
  while ((c = fgetc(ctx->fp)) != EOF) {
    if (i >= sizeof(ctx->buffer)) {
      printf("ERROR: Input too long! Terminating program.\n");
      return TOKEN_ERR;
    }

    if (strchr(delims, c) != NULL) {
      ctx->dlm = c; 
      // Null terminate buffer
      if (i > 0) {
	ctx->buffer[i] = '\0';
	i = 0;
	ctx->token = ctx->buffer;
	return TOKEN_OK;
      } else if (c == '\n') {
	// Return empty token
        ctx->buffer[0] = '\0';
        ctx->token = ctx->buffer;
        return TOKEN_OK; 
      }
      
    } else if (is_token_char(c)) {
      // Add to alphanumeric or special character to buffer
      ctx->buffer[i] = c;
      i++;
    } else if (whitespace && c == ' ') {
      // Add whitespace to buffer
      ctx->buffer[i] = c;
      i++;
    }
  }
  
  if (i > 0) {
    ctx->buffer[i] = '\0';
    i = 0;
    ctx->token = ctx->buffer;
    return TOKEN_OK;
  }
  
  return TOKEN_EOF;
}

int close_parser(struct parser_context *ctx) {
  fclose(ctx->fp);
}
