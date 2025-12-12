#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "../helpers/parser.h"
#include "../helpers/dss.h"

#define LOG_VERBOSE 0

#define MAX_MACHINES 256
#define MAX_LIGHTS 64
#define MAX_BUTTONS 32
#define MAX_BUTTON_COLS 64

int bfs(struct Queue *q, unsigned int btn_masks[MAX_BUTTONS], int num_btns, int *visited) {
  while (!is_empty(q)) {
    unsigned int curr_state = (unsigned int)deq(q);
    if (curr_state == 0) return visited[curr_state];

    for (int k = 0; k < num_btns; k++) {
      unsigned int next_state = curr_state ^ btn_masks[k];

      if (visited[next_state] == -1) {
	visited[next_state] = visited[curr_state] + 1;
	enq(q, (int)next_state);
      }
    }
  }
  
  return -1;
}

int main() {
  const char *filename = "big-input.txt";
  const char *filemode = "r";

  struct parser_context pctx;
  
  init_parser(&pctx, filename, filemode);

  const char *delimeters = "\n";
  const bool allowed_whitespaces = false;
  int lines = 0;
  
  char target_lights[MAX_MACHINES][MAX_LIGHTS];
  int target_ptr = 0;
  int target_lights_lens[MAX_MACHINES];

  int buttons_choices[MAX_MACHINES][MAX_BUTTONS][MAX_BUTTON_COLS];
  memset(buttons_choices, -1, sizeof(buttons_choices));
  int buttons_group = 0;
  int buttons_line_groups[MAX_MACHINES];
  int buttons_cols = 0;
  int highest_cols = 0;
  while (next_token(&pctx, delimeters, allowed_whitespaces)) {
    if (LOG_VERBOSE) printf("INFO: Token [%s]\n", pctx.token);
    
    size_t token_len = strlen(pctx.token);
    int token_ptr = 0;
    bool save_light = false;
    bool save_button = false;
    while (token_ptr < token_len) {
      if (pctx.token[token_ptr] == '[') {
	save_light = true;
      } else if (pctx.token[token_ptr] == ']') {
	save_light = false;
      } else if (save_light) {
	target_lights[lines][target_ptr] = pctx.token[token_ptr];
	target_ptr++;
      }

      if (pctx.token[token_ptr] == '(') {
	save_button = true;
	buttons_cols = 0;
      } else if (pctx.token[token_ptr] == ')') {
	save_button = false;
	buttons_group++;
	if (buttons_cols > highest_cols) highest_cols = buttons_cols;
	buttons_cols = 0;
      } else if (save_button && isdigit(pctx.token[token_ptr])) {
	int digit = pctx.token[token_ptr] - '0';
	buttons_choices[lines][buttons_group][buttons_cols++] = digit;
      }
      
      token_ptr++;
    }
    target_lights[lines][target_ptr] = '\0';
    target_lights_lens[lines] = target_ptr;
    target_ptr = 0;
    buttons_line_groups[lines] = buttons_group;
    buttons_group = 0;
    lines++;
  }

  if (LOG_VERBOSE) {
    for (int i = 0; i < lines; i++) {
      printf("INFO: [");
      for (int j = 0; j < target_lights_lens[i]; j++) {
	char light = target_lights[i][j];
	printf("%c", light);
      }
      printf("]\n");
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

  unsigned int button_masks[MAX_MACHINES][MAX_BUTTONS];
  memset(button_masks, 0, sizeof(button_masks));
  for (int i = 0; i < lines; i++) {
    for (int j = 0; j < buttons_line_groups[i]; j++) {
      int mask = 0;
      for (int k = 0; k < highest_cols; k++) {
	int digit = buttons_choices[i][j][k];
	if (digit >= 0) mask |= (1 << digit);
      }
      button_masks[i][j] = mask;
    }
  }
  
  int total_button_presses = 0;
  for (int i = 0; i < lines; i++) {
    // Convert lights into bitmask
    unsigned int encoded_lights = 0;
    for (int j = 0; j < target_lights_lens[i]; j++) {
      char light = target_lights[i][j];
      if (light == '#') encoded_lights |= (1 << j);
    }
    
    struct Queue q;
    init_q(&q);
    int curr_len = target_lights_lens[i];
    size_t visited_size = (size_t)pow(2, curr_len);
    int *visited = malloc(visited_size * sizeof(int));
    if (visited == NULL) {
      printf("ERROR: Run out of memory for machine %d\n", i);
      return 1;
    }
    
    memset(visited, -1, visited_size * sizeof(int));
    enq(&q, encoded_lights);
    visited[encoded_lights] = 0;

    int button_presses = bfs(&q, button_masks[i], buttons_line_groups[i], visited);
    if (button_presses == -1) {
      printf("ERROR: Machine %d could not be solved!\n", i);
      free(visited);
      return 1;
    }
    total_button_presses += button_presses;
    free(visited);
  }

  printf("INFO: Answer [%d]\n", total_button_presses);
  close_parser(&pctx);
  return 0;
}
