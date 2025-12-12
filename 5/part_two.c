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
  long long counter = 0;
  
  long long first_ids[256];
  long long second_ids[256];
  int insert_idx = 0;

  while (next_token(&pctx, delimeters, allowed_whitespaces)) {
    const char *code = pctx.token;
    char *dash = strchr(code, '-');
    if (dash == NULL) {
      continue;
    }
    
    if (LOG_VERBOSE) printf("INFO: Token [%s]\n", code);
    
    int dash_index = dash - code;

    char first_id[128];
    memcpy(first_id, code, dash_index);
    first_id[dash_index] = '\0';

    const int dash_index_offset = 1;
    const char *second_id = code + dash_index + dash_index_offset;
 
    long long first_id_num = strtoll(first_id, NULL, 10);
    long long second_id_num = strtoll(second_id, NULL, 10);

    first_ids[insert_idx] = first_id_num;
    second_ids[insert_idx] = second_id_num;
    insert_idx++;
  }

  // Sorting according to first ID
  for (int j = 0; j < insert_idx; j++) {
    for (int i = 0; i < insert_idx; i++) {
      if (i == insert_idx-1) break;
      if (first_ids[i] > first_ids[i+1]) {
	long long temp_first_ids_next = first_ids[i+1];
	long long temp_second_ids_next = second_ids[i+1];

	first_ids[i+1] = first_ids[i];
	second_ids[i+1] = second_ids[i];

	first_ids[i] = temp_first_ids_next;
	second_ids[i] = temp_second_ids_next;
      }
    }
  }

  long long my_range_first = first_ids[0];
  long long my_range_second = second_ids[0];

  for (int i = 1; i < insert_idx; i++) {
    if (my_range_second >= first_ids[i]) {
      // Overlap
      if (my_range_second < second_ids[i]) my_range_second = second_ids[i];
      if (LOG_VERBOSE) printf("INFO: Overlap. Created new range. [%lld-%lld]\n", my_range_first, my_range_second);
    } else {
      // Don't overlap
      if (LOG_VERBOSE) printf("INFO: Adding range. [%lld-ll%d]\n", my_range_first, my_range_second);
      counter += (my_range_second - my_range_first) + 1;
      my_range_first = first_ids[i];
      my_range_second = second_ids[i];
    }
  }
  if (LOG_VERBOSE) printf("INFO: Adding range. [%lld-%lld]\n", my_range_first, my_range_second);
  counter += (my_range_second - my_range_first) + 1;

  printf("INFO: Answer [%lld]\n", counter);
  close_parser(&pctx);
  return 0;
}
