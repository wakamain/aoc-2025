#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

int main() {
  const char *filename = "big-input.txt";
  const char *filemode = "r";

  struct parser_context pctx;

  init_parser(&pctx, filename, filemode);

  int dp = 50;
  int password = 0;
  
  while (next_token(&pctx)) {
    char input[3];
    input[0] = pctx.token[1];
    input[1] = pctx.token[2];
    input[2] = pctx.token[3];

    int rots = atoi (input);
    if (pctx.token[0] == 'R') {
      dp += rots;
    } else if (pctx.token[0] == 'L') {
      dp -= rots;
    }
    dp %= 100;

    if (dp == 0) password++;
  }
  
  close_parser(&pctx);

  printf("INFO: Password [%d]\n", password);
  return 0;
}

