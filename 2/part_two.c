#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../helpers/parser.h"

#define LOG_VERBOSE 0

int count_sub_sequences(const char *major_string, const char *minor_string) {
  int counter = 0;
  int minor_string_len = strlen(minor_string);
  const char *major_pointer = major_string;
  
  if (minor_string_len == 0) return 0;

  while ((major_pointer = strstr(major_pointer, minor_string)) != NULL) {
    counter++;
    major_pointer += minor_string_len;
  }
  
  return counter;
}

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
    char num_seen[128];    
    for (long long i = first_id_num; i <= second_id_num; i++) {
      char num_str[128];
      snprintf(num_str, sizeof(num_str), "%lld", i);
      int num_len = strlen(num_str);
      for (int j = 1; j <= num_len; j++) {
	char sub_sequence[128];
	memcpy(sub_sequence, num_str, j);
	sub_sequence[j] = '\0';
	int sub_sequence_len = strlen(sub_sequence);
	
	if (strstr(num_str, sub_sequence) != NULL) {
	  int occurences = count_sub_sequences(num_str, sub_sequence);
	  float chunks = (float)num_len/(float)sub_sequence_len;
	  float epsilon = 1e-9;
	  
	  if (occurences >= 2 && fabs(chunks - occurences) < epsilon) {
	    if (strcmp(num_seen, num_str) == 0) continue;
	    if (LOG_VERBOSE) {
	      printf("INFO: [%lld] fits condition. ", i);
	      printf("[%s] occured [%d] times\n", sub_sequence, occurences);
	    }
	    counter += i;
	    strncpy(num_seen, num_str, sizeof(num_seen));
	    num_seen[sizeof(num_seen)-1] = '\0';
	  }
	}
      }
      
    }
    if (LOG_VERBOSE) printf("\n");
  }

  printf("INFO: Answer [%lld]\n", counter);
  close_parser(&pctx);
  return 0;
}
