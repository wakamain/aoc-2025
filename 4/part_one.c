#include <stdio.h>
#include <string.h>
#include "../helpers/parser.h"

#define LOG_VERBOSE 0

int dfs(char g[256][256], char mg[256][256], int r, int c) {
  int surrounding_paper_rolls = 0;
  // if (r < 0 || c < 0 || r >= level_number || c >= grid_length) return;

  if (g[r][c] == '.') return 0;

  if (g[r-1][c] == '@') surrounding_paper_rolls++;
  if (g[r+1][c] == '@') surrounding_paper_rolls++;
  if (g[r][c-1] == '@') surrounding_paper_rolls++;
  if (g[r][c+1] == '@') surrounding_paper_rolls++;
  if (g[r-1][c-1] == '@') surrounding_paper_rolls++;
  if (g[r-1][c+1] == '@') surrounding_paper_rolls++;
  if (g[r+1][c-1] == '@') surrounding_paper_rolls++;
  if (g[r+1][c+1] == '@') surrounding_paper_rolls++;

  if (surrounding_paper_rolls < 4) {
    mg[r][c] = '@';
    return 1;
  }

  /* dfs(r-1, c, cnt); */
  /* dfs(r+1, c, cnt); */
  /* dfs(r, c-1, cnt); */
  /* dfs(r, c+1, cnt); */
  /* dfs(r-1, c-1, cnt); */
  /* dfs(r-1, c+1, cnt); */
  /* dfs(r+1, c-1, cnt); */
  /* dfs(r+1, c+1, cnt); */
  return 0;
}

int main() {
  const char *filename = "big-input.txt";
  const char *filemode = "r";
  
  struct parser_context pctx;

  init_parser(&pctx, filename, filemode);

  const char *delimeters = "\n";
  const bool allowed_whitespaces = false;

  char grid[256][256];
  char modifiable_grid[256][256];
  int level_number = 0;
  int grid_length = 0;
  while (next_token(&pctx, delimeters, allowed_whitespaces)) {
    const char *level = pctx.token;
    const size_t level_length = strlen(level);
    grid_length = (int)level_length;
    
    if (LOG_VERBOSE) printf("INFO: Level [%d] [%s]\n", level_number, level);

    for (int i = 0; i < (int)level_length; i++) {
      grid[level_number][i] = level[i];
    }
    level_number++;
  }

  close_parser(&pctx);

  int counter = 0;

  if (LOG_VERBOSE) {
    printf("\n---ALL PAPER ROLLS---\n");
    for (int i = 0; i < level_number; i++) {
      for (int j = 0; j < grid_length; j++) {
	printf("%c", grid[i][j]);
      }
      printf("\n");
    }
  }


  for (int i = 0; i < level_number; i++) {
    for (int j = 0; j < grid_length; j++) {
      modifiable_grid[i][j] = '.';   
      counter += dfs(grid, modifiable_grid, i, j);
    }
  }

  if (LOG_VERBOSE) {
    printf("\n---PAPER PROLLS SURROUNDED BY LESS THAN 4 OTHER PAPER ROLLS\n");
    for (int i = 0; i < level_number; i++) {
      for (int j = 0; j < grid_length; j++) {
	printf("%c", modifiable_grid[i][j]);
      }
      printf("\n");
    }
  }

  printf("INFO: Counter [%d]\n", counter);
  return 0;
}
