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
      int length = abs(x[i] - x[j]) + 1;
      int width = abs(y[i] - y[j]) + 1;
      long long area = (long long)length * width;

      if (area > best_area) best_area = area;
      if (LOG_VERBOSE) printf("INFO: Area [%lld]\n", area);
    }
  }

  printf("INFO: Answer [%lld]\n", best_area);
  close_parser(&pctx);
  return 0;
}
