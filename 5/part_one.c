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
  int counter = 0;
  
  long long first_ids[256];
  long long second_ids[256];
  int insert_idx = 0;

  long long seen_ids[1024];
  int seen_ids_idx = 0;
  
  while (next_token(&pctx, delimeters, allowed_whitespaces)) {
    const char *code = pctx.token;
    if (LOG_VERBOSE) printf("INFO: Token [%s]\n", code);
    
    char *dash = strchr(code, '-');
    if (dash == NULL) {
      long long id = strtoll(code, NULL, 10);

      int already_seen = 0;
      for (int j = 0; j < seen_ids_idx; j++) {
	if (id == seen_ids[j]) {
	  already_seen++;
	  break;
	}
      }
      
      if (already_seen) continue;

      int id_matches = 0;
      for (int i = 0; i < insert_idx; i++) {
	if (first_ids[i] <= id && id <= second_ids[i]) {
	  if (LOG_VERBOSE) printf("INFO: ID [%lld] is fresh! Range: [%lld - %lld]\n", id, first_ids[i], second_ids[i]);
	  id_matches++;
	}
      }

      if (id_matches) {
	seen_ids[seen_ids_idx++] = id;
	counter++;
      }
    } else {    
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
  }

  printf("INFO: Answer [%d]\n", counter);
  close_parser(&pctx);
  return 0;
}
