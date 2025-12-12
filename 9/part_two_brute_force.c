#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../helpers/parser.h"

#define LOG_VERBOSE 1

int main() {
  const char *filename = "big-input.txt";
  const char *filemode = "r";

  struct parser_context pctx;

  init_parser(&pctx, filename, filemode);

  const char *delimeters = "\n";
  const bool allows_whitespace = false;

  int x[512];
  int y[512];
  int lines = 0;
  while (next_token(&pctx, delimeters, allows_whitespace)) {
    if (LOG_VERBOSE) printf("INFO: Token [%s]\n", pctx.token);

    size_t token_len = strlen(pctx.token);
    if (token_len == 0) continue;

    int s_idx = 0;
    int p_idx = 0;
    while (p_idx < (int)token_len) {
      if (pctx.token[p_idx] == ',') {
	int num_len = p_idx - s_idx;
	char num_chr[num_len+1];
	memcpy(num_chr, pctx.token + s_idx, num_len);
	num_chr[num_len] = '\0';

	int num = atoi (num_chr);
	x[lines] = num;
	s_idx = p_idx + 1;
      }
      p_idx++;
    }

    if (s_idx < (int)token_len) {
      int num_len = p_idx - s_idx;
      char num_chr[num_len+1];
      memcpy(num_chr, pctx.token + s_idx, num_len);
      num_chr[num_len] = '\0';

      int num = atoi (num_chr);
      y[lines] = num;
    }
      
    lines++;
  }

  int max_x = 0;
  int max_y = 0;
  for (int i = 0; i < lines; i++) {
      if (x[i] > max_x) max_x = x[i];
      if (y[i] > max_y) max_y = y[i];
  }
  
  int helper_rows = max_y + 2;
  int helper_cols = max_x + 2;
  int helper_grid[helper_rows][helper_cols];
  int sum_grid[helper_rows][helper_cols];
  memset(helper_grid, 0, sizeof(helper_grid));
  memset(sum_grid, 0, sizeof(sum_grid));
  
  for (int i = 0; i < lines; i++) {
    helper_grid[y[i]][x[i]] = 1;
  }

  // Fill horizontally
  for (int i = 0; i < helper_rows; i++) {
    int first = -1;
    int last = -1;

    for (int j = 0; j < helper_cols; j++) {
      if (helper_grid[i][j] == 1) {
	first = j;
	break;
      }
    }

    for (int j = helper_cols - 1; j > 0; j--) {
      if (helper_grid[i][j] == 1) {
	last = j;
	break;
      }
    }

    if (first > 0 && last > first) {
      for (int j = first + 1; j < last; j++) {
	if (helper_grid[i][j] == 0) helper_grid[i][j] = 2; 
      }
    }
  }

  // Fill horizontally
  for (int i = 0; i < helper_cols; i++) {
    int first = -1;
    int last = -1;

    for (int j = 0; j < helper_rows; j++) {
      if (helper_grid[j][i] == 1) {
	first = j;
	break;
      }
    }

    for (int j = helper_rows - 1; j >= 0; j--) {
      if (helper_grid[j][i] == 1) {
	last = j;
	break;
      }
    }

    if (first >= 0 && last > first) {
      for (int j = first + 1; j < last; j++) {
	if (helper_grid[j][i] == 0) helper_grid[j][i] = 2; 
      }
    }
  }

    // Fill horizontally again with 2's
  for (int i = 0; i < helper_rows; i++) {
    int first = -1;
    int last = -1;

    for (int j = 0; j < helper_cols; j++) {
      if (helper_grid[i][j] == 2) {
	first = j;
	break;
      }
    }

    for (int j = helper_cols - 1; j > 0; j--) {
      if (helper_grid[i][j] == 2) {
	last = j;
	break;
      }
    }

    if (first > 0 && last > first) {
      for (int j = first + 1; j < last; j++) {
	if (helper_grid[i][j] == 0) helper_grid[i][j] = 2; 
      }
    }
  }

  // Fill 2's region with 1's
  for (int i = 0; i < helper_rows; i++) {
    for (int j = 0; j < helper_cols; j++) {
      if (helper_grid[i][j] == 2) helper_grid[i][j] = 1;
    }
  }

  // Create summed-area-table
  for (int i = 0; i < helper_rows; i++) {
    for (int j = 0; j < helper_cols; j++) {
      int top = (i > 0) ? sum_grid[i-1][j] : 0;
      int left = (j > 0) ? sum_grid[i][j-1] : 0;
      int top_left = (i > 0 && j > 0) ? sum_grid[i-1][j-1] : 0;
      
      sum_grid[i][j] = helper_grid[i][j] + top + left - top_left;
    }
  }

  long long best_area = 0;
  for (int i = 0; i < lines; i++) {
    for (int j = i + 1; j < lines; j++) {
      int min_x = x[i] < x[j] ? x[i] : x[j];
      int max_x = x[i] > x[j] ? x[i] : x[j];
      int min_y = y[i] < y[j] ? y[i] : y[j];
      int max_y = y[i] > y[j] ? y[i] : y[j];
      
      int length = (max_x - min_x) + 1;
      int width = (max_y - min_y) + 1;
      long long area = (long long)length * (long long)width;

      int a = sum_grid[max_y][max_x];
      int b = (min_y > 0) ? sum_grid[min_y-1][max_x] : 0;
      int c = (min_x > 0) ? sum_grid[max_y][min_x-1] : 0;
      int d = (min_y > 0 && min_x > 0) ? sum_grid[min_y-1][min_x-1] : 0;
      
      long long target_area = a - b - c + d;

      printf("INFO: Area [%lld] Target Area [%lld]\n", area, target_area);

      if (target_area == area && target_area > best_area) {
	best_area = target_area;
      }
    }
  }

  printf("INFO: Answer [%lld]\n", best_area);
  close_parser(&pctx);
  return 0;
}
