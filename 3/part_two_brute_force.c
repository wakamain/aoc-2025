#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "../helpers/parser.h"

#define LOG_VERBOSE 1

int numeric_str_cmp(const char input_string_a[], const char input_string_b[]) {
  size_t input_string_a_length = strlen(input_string_a);
  size_t input_string_b_length = strlen(input_string_b);

  if (input_string_a_length != input_string_b_length) return (int)input_string_a_length - (int)input_string_b_length;
  return strcmp(input_string_a, input_string_b);
}

void numeric_str_add(const char input_string_a[], const char input_string_b[], char result_buffer[]) {
  size_t input_string_a_length = strlen(input_string_a);
  size_t input_string_b_length = strlen(input_string_b);
  int max_length = (input_string_a_length > input_string_b_length ? input_string_a_length : input_string_b_length);
  
  int sum = 0;
  int carry = 0;
  for (int i = max_length - 1; i >= 0; i--) {
    int digit_a = (i >= max_length - input_string_a_length) ? input_string_a[i - (max_length - input_string_a_length)] - '0' : 0;
    int digit_b = (i >= max_length - input_string_b_length) ? input_string_b[i - (max_length - input_string_b_length)] - '0' : 0;
    sum = digit_a + digit_b + carry;
    result_buffer[i] = (sum % 10) + '0';
    carry = sum / 10;
  }
  if (carry > 0) {
    memmove(result_buffer + 1, result_buffer, max_length);
    result_buffer[0] = carry + '0';
    result_buffer[max_length + 1] = '\0';
  } else {
    result_buffer[max_length] = '\0';
  }
}

void rec(const char *input_string, int index, char buffer[], int buffer_index, char best[]) {
  if (input_string[index] == '\0') {
    buffer[buffer_index] = '\0';
    size_t buffer_length = strlen(buffer);
    if ((int)buffer_length == 12) {
      if (numeric_str_cmp(buffer, best) > 0) {
	strcpy(best, buffer);
      }
    }
    return;
  }

  buffer[buffer_index] = input_string[index];
  rec(input_string, index + 1, buffer, buffer_index + 1, best);

  rec(input_string, index + 1, buffer, buffer_index, best);
}

int main() {
  const char *filename = "big-input.txt";
  const char *filemode = "r";
  
  struct parser_context pctx;

  init_parser(&pctx, filename, filemode);
  const char delimeter = '\n';

  char counter[1024] = "0";
  while(next_token(&pctx, delimeter)) {
    const char *bank = pctx.token;
    const size_t bank_len = strlen(bank);
    char bank_buffer[1024];
    char largest_bank[20] = "0";
    if (LOG_VERBOSE) printf("INFO: Bank [%s]\n", bank);
    
    rec(bank, 0, bank_buffer, 0, largest_bank);
    numeric_str_add(counter, largest_bank, counter);
    if (LOG_VERBOSE) printf("INFO: Largest Bank [%s]\n", largest_bank);
  }

  printf("INFO: Answer [%s]\n", counter);
  close_parser(&pctx);
  return 0;
}
