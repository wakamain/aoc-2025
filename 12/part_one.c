#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../helpers/parser.h"

#define LOG_VERBOSE 0

#define MAX_PRESENTS 7
#define PRESENT_ROWS 3
#define MAX_EXPRESSION_SIZE 16
#define MAX_EXPRESSIONS 2048
#define MAX_SHAPE_VARIANTS 8

typedef struct {
  int r, c;
} Point;

typedef struct {
  int id;
  int num_blocks;
  Point blocks[PRESENT_ROWS * PRESENT_ROWS];
  int width, height;
} Shape;

void normalize_shape(Shape *s) {
  int min_r = 100;
  int min_c = 100;

  for (int i = 0; i < s->num_blocks; i++) {
    if (s->blocks[i].r < min_r) min_r = s->blocks[i].r;
    if (s->blocks[i].c < min_c) min_c = s->blocks[i].c;
  }

  for (int i = 0; i < s->num_blocks; i++) {
    s->blocks[i].r -= min_r;
    s->blocks[i].c -= min_c;
  }
}

Shape get_shape_variation(Shape base, int variation) {
  Shape current = base;

  // If variations greater than 5, then flips needed
  if (variation >= 4) {
    for (int i = 0; i < current.num_blocks; i++) current.blocks[i].c = -current.blocks[i].c;
  }

  // Variations 0,1,2,3 are 0, 90, 180, 270 rotations
  int rotations = variation % 4;
  for (int rot = 0; rot < rotations; rot++) {
    for (int i = 0; i < current.num_blocks; i++) {
      int old_r = current.blocks[i].r;
      int old_c = current.blocks[i].c;
      current.blocks[i].r = old_c;
      current.blocks[i].c = -old_r;
    }
  }

  normalize_shape(&current);
  return current;
}

bool can_fit(int grid_h, int grid_w, int grid[grid_h][grid_w], int gr, int gc, Shape s) {
  for (int i = 0; i < s.num_blocks; i++) {
    int r = gr + s.blocks[i].r;
    int c = gc + s.blocks[i].c;

    if (r < 0 || r >= grid_h || c < 0 || c >= grid_w) return false;

    if (grid[r][c] != 0) return false;
  }
  
  return true;
}

void toggle_shape(int grid_h, int grid_w, int grid[grid_h][grid_w], int gr, int gc, Shape s, int val) {
  for (int i = 0; i < s.num_blocks; i++) {
    grid[gr + s.blocks[i].r][gc + s.blocks[i].c] = val;
  }
}

bool find_empty_cell(int h, int w, int grid[h][w], int *r, int *c) {
  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      if (grid[i][j] == 0) {
	*r = i;
	*c = j;
	return true;
      }
    }
  }
  return false;
}

bool shapes_equal(Shape a, Shape b) {
  if (a.num_blocks != b.num_blocks) return false;

  for (int i = 0; i < a.num_blocks; i++) {
    bool found = false;
    for (int j = 0; j < b.num_blocks; j++) {
      if (a.blocks[i].r == b.blocks[j].r && a.blocks[i].c == b.blocks[j].c) {
	found = true;
	break;
      }
    }
    if (!found) return false;
  }
  return true;
}

Shape base_shapes[MAX_PRESENTS];
Shape unique_variants[MAX_PRESENTS][MAX_SHAPE_VARIANTS];
int num_variants[MAX_PRESENTS];
bool dfs(int h, int w, int grid[h][w], int counts[MAX_PRESENTS]) {
  int fr = -1;
  int fc = -1;

  // If no empty cells left grid solved
  if (!find_empty_cell(h, w, grid, &fr, &fc)) return true;

  int try_order[] = {5,4,3,2,1,0,6};
  for (int k = 0; k < MAX_PRESENTS; k++) {
    int p = try_order[k];
    
    if (counts[p] > 0) {
      counts[p]--;

      for (int v = 0; v < MAX_SHAPE_VARIANTS; v++) {
	if (p == 6 && v > 0) break;
	Shape s = unique_variants[p][v];
	
	for (int b = 0; b < s.num_blocks; b++) {
	  int origin_r = fr - s.blocks[b].r;
	  int origin_c = fc - s.blocks[b].c;

	  if (can_fit(h, w, grid, origin_r, origin_c, s)) {
	    toggle_shape(h, w, grid, origin_r, origin_c, s, 1);

	    if (dfs(h, w, grid, counts)) return true;
	    
	    toggle_shape(h, w, grid, origin_r, origin_c, s, 0);
	  }
	}
      }

      counts[p]++;
    }
  }
  
  return false;
}

int main() {
  const char *filename = "big-input.txt";
  const char *filemode = "r";

  struct parser_context pctx;

  init_parser(&pctx, filename, filemode);

  const char *delimeters = "\n: ";
  const bool allow_whitespaces = false;

  int present_indexes[MAX_PRESENTS];
  char presents[MAX_PRESENTS][PRESENT_ROWS][PRESENT_ROWS];
  int widths[MAX_EXPRESSIONS];
  int heights[MAX_EXPRESSIONS];
  int counters[MAX_EXPRESSIONS][MAX_PRESENTS*2];

  int present_idx = 0;
  int row = 0;
  int expression_idx = 0;
  int counters_idx = 0;
  bool reading_counts = false;
  int current_count_idx = 0;

  while (next_token(&pctx, delimeters, allow_whitespaces)) {
    const char *token = pctx.token;
    const size_t token_len = strlen(token);
    if (token_len == 0) continue;

    if (LOG_VERBOSE) printf("INFO: Token [%s]\n", token);
    
    if (strchr(token, 'x')) {
      char token_cpy[MAX_EXPRESSION_SIZE];
      strcpy(token_cpy, token);
      char *x_pos = strchr(token_cpy, 'x');
      
      *x_pos = '\0';
      widths[expression_idx] = atoi(token_cpy);
      heights[expression_idx] = atoi(x_pos + 1);
      expression_idx++;
      
      reading_counts = true;
      current_count_idx = 0;
      continue;
    }

    if (reading_counts) {
      if (isdigit(token[0])) {
        counters[counters_idx][current_count_idx] = atoi(token);
        current_count_idx++;

        if (current_count_idx == 6) {
          reading_counts = false;
          counters_idx++;
        }
      }
      continue;
    }
    
    if (token_len == 1 && isdigit(token[0])) {
      present_indexes[present_idx] = token[0] - '0';
      row = 0;
      continue;
    }

    if (token_len == PRESENT_ROWS && !isdigit(token[0])) {
      if (present_idx >= MAX_PRESENTS) {
        printf("ERROR: Too many presents!\n");
        return 1;
      }
      
      for (int col = 0; col < PRESENT_ROWS; col++) {
	presents[present_idx][row][col] = token[col];
      }
      row++;

      if (row == PRESENT_ROWS) present_idx++;
    }
  }

  if (LOG_VERBOSE) {
    printf("\nINFO: Present Indexes [");
    for (int i = 0; i < present_idx; i++) {
      if (i < present_idx - 1) printf("%d, ", present_indexes[i]);
      else printf("%d", present_indexes[i]);
    }
    printf("]\n");
  }

  int rows = row;
  if (LOG_VERBOSE) {
    printf("\nINFO: Presents\n");
    for (int i = 0; i < present_idx; i++) {
      for (int j = 0; j < rows; j++) {
	for (int k = 0; k < rows; k++) {
	  printf("%c", presents[i][j][k]);
	}
	printf("\n");
      }
      printf("\n");
    }
  }

  if (LOG_VERBOSE) {
    printf("\nINFO: Areas\n");
    for (int i = 0; i < expression_idx; i++) {
      printf("W[%d] * H[%d] = A[%d]\n", widths[i], heights[i], widths[i] * heights[i]);
    }
  }

  if (LOG_VERBOSE) {
    printf("\nINFO: Counters\n");
    for (int i = 0; i < counters_idx; i++) {
      printf("INFO: C = ");
      for (int j = 0; j < present_idx; j++) {
	printf("%d", counters[i][j]);
      }
      printf("\n");
    }
  }

  for (int i = 0; i < present_idx; i++) {
    base_shapes[i].id = 1;
    base_shapes[i].num_blocks = 0;
    for (int r = 0; r < PRESENT_ROWS; r++) {
      for (int c = 0; c < PRESENT_ROWS; c++) {
	if (presents[i][r][c] == '#') {
	  int b = base_shapes[i].num_blocks;
	  base_shapes[i].blocks[b].r = r;
	  base_shapes[i].blocks[b].c = c;
	  base_shapes[i].num_blocks++;
	}
      }
    }

    normalize_shape(&base_shapes[i]);
  }
  
  // Dummy presents to fill up to 7
  base_shapes[6].id = 6;
  base_shapes[6].num_blocks = 1;
  base_shapes[6].blocks[0].r = 0;
  base_shapes[6].blocks[0].c = 0;
  base_shapes[6].width = 1;
  base_shapes[6].height = 1;

  for (int p = 0; p < MAX_PRESENTS; p++) {
    num_variants[p] = 0;
    for (int v = 0; v < MAX_SHAPE_VARIANTS; v++) {
      // Optimization: Filler (6) only needs 1
      if (p == 6 && v > 0) break; 

      Shape s = get_shape_variation(base_shapes[p], v);
      bool duplicate = false;
      for (int existing = 0; existing < num_variants[p]; existing++) {
	if (shapes_equal(s, unique_variants[p][existing])) {
	  duplicate = true;
	  break;
	}
      }
          
      if (!duplicate) {
	unique_variants[p][num_variants[p]] = s;
	num_variants[p]++;
      }
    }

    if (LOG_VERBOSE) printf("Present %d has %d unique variations\n", p, num_variants[p]);
  }
  
  int answer = 0;
  for (int i = 0; i < expression_idx; i++) {
    int w = widths[i];
    int h = heights[i];
    int region_area = w * h;

    int current_counts[MAX_PRESENTS];
    memset(current_counts, 0, sizeof(current_counts));

    int presents_area = 0;
    for (int p = 0; p < MAX_PRESENTS - 1; p++) {
      current_counts[p] = counters[i][p];
      presents_area += (counters[i][p] * base_shapes[p].num_blocks);
    }

    int gap = region_area - presents_area;
    
    if (gap >= 0) {
      current_counts[6] = gap;
      int grid[h][w];
      memset(grid, 0, sizeof(grid));

      if (LOG_VERBOSE) printf("Checking Region %d\n...", i);

      if (dfs(h, w, grid, current_counts)) answer++;      
    }
  }

  printf("INFO: Answer [%d]\n", answer);
  
  close_parser(&pctx);
  return 0;
}
