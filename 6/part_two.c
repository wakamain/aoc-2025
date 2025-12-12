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

  const char *delimeters = "\n";
  const bool allow_whitespaces = true;

  long long counter = 0;
  static char grid[4096][4096];
  memset(grid, 0, sizeof(grid));

  int row = 0;
  while (next_token(&pctx, delimeters, allow_whitespaces)) {
    size_t token_len = strlen(pctx.token);
    for (int col = 0; col < (int)token_len; col++) {
      grid[row][col] = pctx.token[col];
    }

    row++;
    if (LOG_VERBOSE) printf("INFO: Token [%s]\n", pctx.token);
  }

  int rows = row;
  int cols = 0;
  
  for (int i = 0; i < rows; i++) {
    size_t row_len = strlen(grid[i]);
    if (row_len > cols) cols = row_len;
  }

  if (LOG_VERBOSE) {
    printf("INFO: ROWS [%d] COLS [%d]\n", rows, cols);
  
    printf("INFO: NUMS (CHARS)\n");
    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < cols; j++) {
	printf("[%c]", grid[i][j]);
      }
      printf("\n");
    }

    printf("INFO: TRANSPOSED-NUMS (CHARS)\n");
    for (int j = 0; j < cols; j++) {
      for (int i = 0; i < rows; i++) {
	printf("[%c]", grid[i][j]);
      }
      printf("\n");
    }
  }

  bool is_add = true;
  long long nums[4096];
  int n_idx = 0;
  for (int j = 0; j < cols; j++) {
    int null_cnt = 0;
    char num[rows+1];
    int k = 0;
    for (int i = 0; i < rows; i++) {
      if (grid[i][j] == '+') {
	is_add = true; 
      } else if ((grid[i][j] == '*')) {
	is_add = false;
      } else if (grid[i][j] == ' ') null_cnt++;
      else {
	num[k] = grid[i][j];
	k++;
      }
    }

    if (null_cnt < rows) {
      long long num_long_long = atoll (num);
      nums[n_idx] = num_long_long;
      n_idx++;
      memset(num, 0, sizeof(num));
    }
    
    if (null_cnt >= rows) {
      long long accumulator = is_add ? 0 : 1;
      if (LOG_VERBOSE) {
	printf("INFO: ");
	for (int i = 0; i < n_idx; i++) {
	  if (is_add) {
	    if (i < n_idx-1) printf("[%lld] + ", nums[i]);
	    else printf("[%lld] ", nums[i]);
	  }
	  else {
	    if (i < n_idx-1) printf("[%lld] * ", nums[i]);
	    else printf("[%lld] ", nums[i]);
	  }
	}
      }

      for (int i = 0; i < n_idx; i++) {
	if (is_add) {
	  accumulator += nums[i];
	}
	else {
	  accumulator *= nums[i];
	}
      }

      if (LOG_VERBOSE) {
	printf("= [%lld] ", accumulator);
	printf("\n");
      }

      counter += accumulator;
      memset(nums, 0, sizeof(nums));
      n_idx = 0;
      null_cnt = 0;
    }
  }

  
  long long accumulator = is_add ? 0 : 1;

  if (LOG_VERBOSE) {
    printf("INFO: ");
    for (int i = 0; i < n_idx; i++) {
      if (is_add) {
	if (i < n_idx-1) printf("[%lld] + ", nums[i]);
	else printf("[%lld] ", nums[i]);
      }
      else {
	if (i < n_idx-1) printf("[%lld] * ", nums[i]);
	else printf("[%lld] ", nums[i]);
      }
    }
  }

  for (int i = 0; i < n_idx; i++) {
    if (is_add) {
      accumulator += nums[i];
    }
    else {
      accumulator *= nums[i];
    }
  }


  if (LOG_VERBOSE) {
    printf("= [%lld] ", accumulator);
    printf("\n");
  }
      
  counter += accumulator;

  printf("INFO: Answer [%lld]\n", counter);
  close_parser(&pctx);
  return 0;
}
