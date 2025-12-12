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
  const char *delimeters = ",";
  const bool allowed_whitespaces = false;
  long long counter = 0;
  
  while (next_token(&pctx, delimeters, allowed_whitespaces)) {
    const char *code = pctx.token;
    if (LOG_VERBOSE) printf("INFO: Token [%s]\n", code);
    
    char *dash = strchr(code, '-');
    if (dash == NULL) {
      printf("ERROR: Could not find dash in token!\n");
      return 1;
    }
    
    int dash_index = dash - code;

    char first_id[128];
    memcpy(first_id, code, dash_index);
    first_id[dash_index] = '\0';


    const int dash_index_offset = 1;
    const char *second_id = code + dash_index + dash_index_offset;


    long long first_id_num = strtoll(first_id, NULL, 10);
    long long second_id_num = strtoll(second_id, NULL, 10);
    
    for (long long i = first_id_num; i <= second_id_num; i++) {
      char num_str[128];
      snprintf(num_str, sizeof(num_str), "%lld", i);
      int num_len = strlen(num_str);
      if (num_len % 2 == 0) {
	char num_first_half[64];
	memcpy(num_first_half, num_str, num_len/2);
	num_first_half[num_len/2] = '\0';

	const char *num_second_half = num_str + num_len/2;

	if (strcmp(num_first_half, num_second_half) == 0) {
	  if (LOG_VERBOSE) {
	    printf("INFO: Match found! [%lld]\n", i);
	  }
	  counter += i;
	}
      }
    }
    if (LOG_VERBOSE) printf("\n");
  }

  printf("INFO: Answer [%lld]\n", counter);
  close_parser(&pctx);
  return 0;
}
