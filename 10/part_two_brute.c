#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "../helpers/parser.h"

#define LOG_VERBOSE 1

#define MAX_MACHINES 256
#define MAX_JOLTAGES 64
#define MAX_BUTTONS 32
#define MAX_BUTTON_COLS 64

int dfs(int button_idx, int counters[], int target[], int num_buttons, int num_counters, int button_coeff[MAX_BUTTONS][MAX_JOLTAGES], int MAX_TARGET) {
    if (button_idx == num_buttons) {
        for (int i = 0; i < num_counters; i++)
            if (counters[i] != target[i])
                return 10000000;
        return 0;
    }

    int best = 10000000;
    for (int presses = 0; presses <= MAX_TARGET; presses++) {
        for (int c = 0; c < num_counters; c++)
            counters[c] += button_coeff[button_idx][c] * presses;

        int res = presses + dfs(button_idx+1, counters, target, num_buttons, num_counters, button_coeff, MAX_TARGET);
        if (res < best) best = res;

        for (int c = 0; c < num_counters; c++)
            counters[c] -= button_coeff[button_idx][c] * presses;
    }
    return best;
}


int main() {
  const char *filename = "big-input.txt";
  const char *filemode = "r";

  struct parser_context pctx;
  
  init_parser(&pctx, filename, filemode);

  const char *delimeters = "\n";
  const bool allowed_whitespaces = false;
  int lines = 0;

  int joltages[MAX_MACHINES][MAX_JOLTAGES];
  int j_ptr = 0;
  int joltages_len[MAX_MACHINES];
  
  int buttons_choices[MAX_MACHINES][MAX_BUTTONS][MAX_BUTTON_COLS];
  // double solver_matrix[MAX_JOLTAGES][MAX_BUTTONS + 1];
  memset(buttons_choices, -1, sizeof(buttons_choices));
  // memset(solver_matrix, 0, sizeof(solver_matrix));
  
  int buttons_group = 0;
  int buttons_line_groups[MAX_MACHINES];
  int buttons_cols = 0;
  int highest_cols = 0;
  
  while (next_token(&pctx, delimeters, allowed_whitespaces)) {
    if (LOG_VERBOSE) printf("INFO: Token [%s]\n", pctx.token);
    
    size_t token_len = strlen(pctx.token);
    int token_ptr = 0;
    bool save_joltage = false;
    bool save_button = false;
    int curr_val = 0;
    bool reading_num = false;
    
    while (token_ptr < token_len) {
      char c = pctx.token[token_ptr];
      if (c == '{') {
	save_joltage = true;
	reading_num = false;
      } else if (c == '}') {
	save_joltage = false;

	if (reading_num) {
	  joltages[lines][j_ptr] = curr_val;
	  curr_val = 0;
	  j_ptr++;
	}
	
	reading_num = false;
      } else if (save_joltage && isdigit(c)) {
	reading_num = true;
	int digit = c - '0';
	curr_val = (curr_val * 10) + digit;
      } else if (c == ',' && save_joltage) {
	if (reading_num) {
	  joltages[lines][j_ptr] = curr_val;
	  curr_val = 0;
	  j_ptr++;
	}
	
	reading_num = false;
      }

      if (c == '(') {
	save_button = true;
	reading_num = false;
	buttons_cols = 0;
      } else if (c == ')') {
	save_button = false;

	if (reading_num) {
	  buttons_choices[lines][buttons_group][buttons_cols++] = curr_val;
	  curr_val = 0;
	  buttons_group++;
	}
	
	reading_num = false;
	if (buttons_cols > highest_cols) highest_cols = buttons_cols;
	buttons_cols = 0;
      } else if (save_button && isdigit(c)) {
	reading_num = true;
	int digit = c - '0';
	curr_val = (curr_val * 10) + digit;
      } else if (c == ',') {
	if (reading_num) {
	  buttons_choices[lines][buttons_group][buttons_cols++] = curr_val;
	  curr_val = 0;
	}
      }
      
      token_ptr++;
    }
    
    joltages_len[lines] = j_ptr;
    j_ptr = 0;
    buttons_line_groups[lines] = buttons_group;
    buttons_group = 0;
    lines++;
  }

  if (LOG_VERBOSE) {
    for (int i = 0; i < lines; i++) {
      printf("INFO: {");
      for (int j = 0; j < joltages_len[i]; j++) {
	if (j < joltages_len[i] - 1) printf("%d, ", joltages[i][j]);
	else printf("%d", joltages[i][j]);
      }
      printf("}\n");
    }
  }

  if (LOG_VERBOSE) {
    printf("INFO: Button Choices\n");
    for (int i = 0; i < lines; i++) {
      for (int j = 0; j < buttons_line_groups[i]; j++) {
	for (int k = 0; k < highest_cols; k++) {
	  int digit = buttons_choices[i][j][k];
	  if (digit >= 0) printf("%d ", buttons_choices[i][j][k]);
	}
	printf("\n");
      }
      printf("New Machine\n");
    }
  }

  int presses = 0;
  for (int i = 0; i < lines; i++) {
    int num_counters = joltages_len[i];
    int button_coeff[MAX_BUTTONS][MAX_JOLTAGES];
    memset(button_coeff, 0, sizeof(button_coeff));

    for (int j = 0; j < buttons_line_groups[i]; j++) {
      for (int k = 0; k < highest_cols; k++) {
	int digit = buttons_choices[i][j][k];
	if (digit != -1 && digit < num_counters) button_coeff[j][digit] = 1;
      }
    }

    int MAX_TARGET = 0;
    for (int c = 0; c < num_counters; c++) {
      if (joltages[i][c] > MAX_TARGET) MAX_TARGET = joltages[i][c];
    }
    
    int best_total = 10000000;
    int num_buttons = buttons_line_groups[i];
    int stack[MAX_BUTTONS];
    int counters[MAX_JOLTAGES];

    memset(counters, 0, sizeof(counters));
    best_total = dfs(0, counters, joltages[i], num_buttons, num_counters, button_coeff, MAX_TARGET);
    presses += best_total;
    printf("INFO: Machine %d minimal presses = %d\n", i, best_total);
  }

  printf("INFO: Answer [%d]\n", presses);
  close_parser(&pctx);
  return 0;
}
