#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../helpers/parser.h"

#define LOG_VERBOSE 0

int main() {
  const char *filename = "big-input.txt";
  const char *filemode = "r";

  struct parser_context pctx;

  init_parser(&pctx, filename, filemode);
  
  const char *delimeters = "\n ";
  const bool allowed_whitespaces = false;
  long long grid[5][1024];
  char ops[1024][8];

  memset(grid, 0, sizeof(grid));
  memset(ops, 0, sizeof(ops));
  
  int row = 0;
  int col = 0;
  int cols = 0;
  int ops_idx = 0;
  long long counter = 0;
  while (next_token(&pctx, delimeters, allowed_whitespaces)) {
    if (strlen(pctx.token) <= 0) {
      if (pctx.dlm == '\n') {
	pctx.dlm = 0;
	row++;
	col = 0;
      }
      continue;
    }

    char *code = pctx.token;
    if (strstr(code, "*") || strstr(code, "+")) {
      strcpy(ops[ops_idx], code);
      ops_idx++;
    } else {
      grid[row][col] = strtoll(pctx.token, NULL, 10);
    }
    
    if (LOG_VERBOSE) printf("INFO: Token [%s] Row [%d] Col [%d]\n", pctx.token, row, col);

    col++;
    if (col > cols) {
      cols = col;
    }
    if (pctx.dlm == '\n') {
      pctx.dlm = 0;
      row++;
      col = 0;
    }
  }

  int rows = row-1;
  if (LOG_VERBOSE) {
    printf("INFO: NUMS\n");
    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < cols; j++) {
	printf("[%lld]", grid[i][j]);
      }
      printf("\n");
    }
  }

  if (LOG_VERBOSE) {
    printf("INFO: OPS\n");
    for (int i = 0; i < cols; i++) {
      printf("[%s]", ops[i]);
    }
    printf("\n");
  }

  long long comps[cols];
  memset(comps, 0, sizeof(long long) * cols);
  for (int i = 0; i < cols; i++) {
    if (strcmp(ops[i], "*") == 0) {
      long long result = 1;
      for (int j = 0; j < rows; j++) {
	result *= grid[j][i]; 
      }
      comps[i] = result;
    } else if (strcmp(ops[i], "+") == 0) {
      long long result = 0;
      for (int j = 0; j < rows; j++) {
	result += grid[j][i]; 
      }
      comps[i] = result;
    }
  }

  if (LOG_VERBOSE) {
    printf("INFO: Comps ");
    for (int i = 0; i < cols; i++) {
      printf("[%lld] ", comps[i]);
    }
    printf("\n");
  }

  for (int i = 0; i < cols; i++) {
    counter += comps[i];
  }

  printf("INFO: Answer [%lld]\n", counter);
  close_parser(&pctx);
  return 0;
}
