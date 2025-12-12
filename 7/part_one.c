#include <stdio.h>
#include <string.h>
#include "../helpers/parser.h"

#define LOG_VERBOSE 0

int main() {
  const char *filename = "big-input.txt";
  const char *filemode = "r";

  struct parser_context pctx;

  init_parser(&pctx, filename, filemode);

  const char *delimeters = "\n";
  const bool allowed_whitespaces = false;
  int counter = 0;

  char grid[256][256];
  memset(grid, '.', sizeof(grid));
  int line = 0;
  size_t cols = 0;
  while (next_token(&pctx, delimeters, allowed_whitespaces)) {
    if (LOG_VERBOSE) printf("INFO: Line [%s]\n", pctx.token);
    size_t token_len = strlen(pctx.token);
    cols = token_len;

    for (int i = 0; i < (int)token_len; i++) {
      grid[line][i] = pctx.token[i];
    }
    line++;
  }

  char beam_grid[256][256];
  memset(beam_grid, '.', sizeof(beam_grid));
  int rows = line;

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < (int)cols; j++) {
      if (i == 0) continue;

      if ((grid[i-1][j] == 'S' || beam_grid[i-1][j] == '|') && grid[i][j] == '.') beam_grid[i][j] = '|';
      if (i >= 2 && beam_grid[i-2][j] == '|' && grid[i-1][j] == '^') {
	if (j > 0) beam_grid[i][j-1] = '|';
	if (j < cols-1) beam_grid[i][j+1] = '|';
      }
    }
  }

  if (LOG_VERBOSE) {
    printf("---GRID---\n");
    for (int i = 0; i < rows; i++) {
      printf("[");
      for (int j = 0; j < (int)cols; j++) {
	printf("%c", grid[i][j]);
      }
      printf("]\n");
    }

    printf("---BEAM GRID---\n");
    for (int i = 0; i < rows; i++) {
      printf("[");
      for (int j = 0; j < (int)cols; j++) {
	printf("%c", beam_grid[i][j]);
      }
      printf("]\n");
    }
  }

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < (int)cols; j++) {
      if (i == rows-1 ) continue;

      if (beam_grid[i][j] == '|' && grid[i+1][j] == '^') counter++;
    }
  }

  printf("INFO: Answer [%d]\n", counter);
  
  close_parser(&pctx);
  return 0;
}
