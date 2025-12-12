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
  while(next_token(&pctx, delimeters, allowed_whitespaces)) {
    const char *bank = pctx.token;
    const size_t bank_len = strlen(bank);
    
    if (LOG_VERBOSE) printf("INFO: Bank [%s]\n", bank);
    int largest_joltage = 0;
    for (int i = 0; i < (int)bank_len; i++) {
      for (int j = i+1; j < (int)bank_len; j++) {
	char joltage_char[8];
	snprintf(joltage_char, sizeof(joltage_char), "%c%c", bank[i], bank[j]);
	int joltage = atoi (joltage_char);
	
	if (joltage > largest_joltage) {
	  largest_joltage = joltage;
	}
      }
    }
    
    if (LOG_VERBOSE) printf("INFO: Largest Joltage [%d]\n", largest_joltage);
    counter += largest_joltage;
  }

  printf("INFO: Answer [%d]\n", counter);
  close_parser(&pctx);
  return 0;
}
