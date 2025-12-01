#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

int max(int a, int b) {
  return a > b ? a : b;
}

int main() {
  const char *filename = "big-input.txt";
  const char *filemode = "r";

  struct parser_context pctx;

  init_parser(&pctx, filename, filemode);

  int dp = 50;
  int password = 0;
  
  while (next_token(&pctx)) {
    char *code = pctx.token;
    int rots = atoi (code + 1);

    if (code[0] == 'R') {
      for (int i = 0; i < rots; i++) {
	dp++;
	dp = ((dp % 100) + 100) % 100;
	if (dp == 0) password++;
      }
    } else if (code[0] == 'L') {
      for (int i = 0; i < rots; i++) {
	dp--;
	dp = ((dp % 100) + 100) % 100;
	if (dp == 0) password++;
      }
    }
  }

  close_parser(&pctx);

  printf("INFO: Password [%d]\n", password);
  return 0;
}

