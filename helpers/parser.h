#define BUFFER_SIZE 4096

struct parser_context {
  FILE *fp;
  char buffer[BUFFER_SIZE];
  char *token;
  char dlm;
};

int is_token_char(int c);

int init_parser(struct parser_context *ctx, const char *filename, const char *filemode);

int next_token(struct parser_context *ctx, const char *delims, bool whitespace);

int close_parser(struct parser_context *ctx);
