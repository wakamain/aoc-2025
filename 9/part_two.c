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
  const bool allowed_whitespaces = false;

  int x[512];
  int y[512];
  int lines = 0;
  while (next_token(&pctx, delimeters, allowed_whitespaces)) {
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

  long long best_area = 0;
  for (int i = 0; i < lines; i++) {
    for (int j = i + 1; j < lines; j++) {
      bool rec_intersected = false;
      int min_rec_x = x[i] < x[j] ? x[i] : x[j];
      int max_rec_x = x[i] > x[j] ? x[i] : x[j];
      int min_rec_y = y[i] < y[j] ? y[i] : y[j];
      int max_rec_y = y[i] > y[j] ? y[i] : y[j];

      for (int k = 0; k < lines; k++) {
	int l = (k + 1) % lines;
	int min_wall_x = x[k] < x[l] ? x[k] : x[l];
	int max_wall_x = x[k] > x[l] ? x[k] : x[l];
	int min_wall_y = y[k] < y[l] ? y[k] : y[l];
	int max_wall_y = y[k] > y[l] ? y[k] : y[l];

	if (x[k] == x[l] && min_rec_x < x[k] && x[k] < max_rec_x && min_wall_y < max_rec_y && max_wall_y > min_rec_y) {
	  rec_intersected = true;
	}
	if (y[k] == y[l] && min_rec_y < y[k] && y[k] < max_rec_y && min_wall_x < max_rec_x && max_wall_x > min_rec_x) {
	  rec_intersected = true;
	}
      }
      
      int length = (max_rec_x - min_rec_x) + 1;
      int width = (max_rec_y - min_rec_y) + 1;
      long long area = (long long)length * (long long)width;
      
      if (!rec_intersected && area > best_area) {
	if (LOG_VERBOSE) printf("INFO: X1=[%d] Y1=[%d], X2=[%d] Y2=[%d]. Area not intersected [%lld]\n", x[i], y[i], x[j], y[j], area);
	best_area = area;
      }
    }
  }

  printf("INFO: Answer [%lld]\n", best_area);
  close_parser(&pctx);
  return 0;
}
