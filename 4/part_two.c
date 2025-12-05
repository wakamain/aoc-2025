#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../helpers/parser.h"

#define LOG_VERBOSE 0
#define VISUALIZE 0

int main() {
  const char *filename = "big-input.txt";
  const char *filemode = "r";
  
  struct parser_context pctx;

  init_parser(&pctx, filename, filemode);

  const char delimeter = '\n';

  char grid[256][256];
  unsigned char to_delete[256][256];
  memset(to_delete, 0, sizeof(to_delete));
  int level_number = 0;
  int grid_length = 0;
  while (next_token(&pctx, delimeter)) {
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
    printf("INFO: Level Number [%d] Grid Length [%d]\n", level_number, grid_length);
    printf("\n---PAPER ROLLS BEFORE REMOVAL---\n");
    for (int i = 0; i < level_number; i++) {
      for (int j = 0; j < grid_length; j++) {
	printf("%c", grid[i][j]);
      }
      printf("\n");
    }
    printf("\n");
  }
  
  int updates = 0;
  do {
    updates = 0;
    memset(to_delete, 0, sizeof(to_delete));
    
    for (int i = 0; i < level_number; i++) {
      for (int j = 0; j < grid_length; j++) {
	if (grid[i][j] == '@') {
	  int surrounding_paper_rolls = 0;
  
	  if (i > 0 && grid[i-1][j] == '@') surrounding_paper_rolls++;
	  if (i < level_number-1 && grid[i+1][j] == '@') surrounding_paper_rolls++;
	  if (j > 0 && grid[i][j-1] == '@') surrounding_paper_rolls++;
	  if (j < grid_length-1 && grid[i][j+1] == '@') surrounding_paper_rolls++;
	  if (i > 0 && j > 0 && grid[i-1][j-1] == '@') surrounding_paper_rolls++;
	  if (i > 0 && j < grid_length-1 && grid[i-1][j+1] == '@') surrounding_paper_rolls++;
	  if (i < level_number-1 && j > 0 && grid[i+1][j-1] == '@') surrounding_paper_rolls++;
	  if (i < level_number-1 && j < grid_length-1 && grid[i+1][j+1] == '@') surrounding_paper_rolls++;

	  if (surrounding_paper_rolls < 4) {
	    to_delete[i][j] = 1;
	  }
	}
      }
    }

    if (VISUALIZE) {
      printf("\033[H"); // move to top-left
      
      for (int i = 0; i < level_number; i++) {
	for (int j = 0; j < grid_length; j++) {
	  printf("%c", grid[i][j]);
	}
	printf("\n");
      }
      printf("\n");
      
      fflush(stdout);
      usleep(100000);
  }


    for (int i = 0; i < level_number; i++) {
      for (int j = 0; j < grid_length; j++) {
	if (to_delete[i][j]) {
	  grid[i][j] = '.';
	  updates++;
	}
      }
    }

    counter += updates;
  } while (updates);
  
  if (LOG_VERBOSE) {
    printf("\n---PAPER ROLLS AFTER REMOVAL---\n");
    for (int i = 0; i < level_number; i++) {
      for (int j = 0; j < grid_length; j++) {
	printf("%c", grid[i][j]);
      }
      printf("\n");
    }
    printf("\n");
  }

  printf("INFO: Counter [%d]\n", counter);
  return 0;
}
