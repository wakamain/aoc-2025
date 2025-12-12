#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "../helpers/parser.h"

#define LOG_VERBOSE 0

struct best_num_index {
  int best_num;
  int best_index;
};

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

struct best_num_index get_largest_number(char input_string[], int start_index, int search_window) {
  size_t input_string_length = strlen(input_string);
  struct best_num_index bni;
  
  if (input_string_length <= 0) {
    printf("ERROR: Input string length is less than 0!");
      bni.best_num = -1;
      bni.best_index = -1;
      
      return bni;
  }

  int best = 0; // In this case the string is guaranteed not to have a 0
  int best_index = 0;
  for (int i = start_index; i < search_window; i++) {
    int num = input_string[i] - '0';
    if (num > best) best = num;
  }

  for (int i = start_index; i < search_window; i++) {
    int num = input_string[i] - '0';
    if (num == best) {
      best_index = i;
      break;
    }
  }

  bni.best_num = best;
  bni.best_index = best_index;
  
  return bni;
}

int main() {
  const char *filename = "big-input.txt";
  const char *filemode = "r";
  
  struct parser_context pctx;

  init_parser(&pctx, filename, filemode);
  const char *delimeters = "\n";
  const bool allowed_whitespaces = false;

  char counter[128] = "0";
  while(next_token(&pctx, delimeters, allowed_whitespaces)) {
    const char *bank = pctx.token;
    const size_t bank_len = strlen(bank);

    char largest_bank[16];
    char new_bank[128];
    strcpy(new_bank, bank);
    
    if (LOG_VERBOSE) printf("INFO: Bank [%s]\n", new_bank);

    int position = 0;
    for (int i = 0; i < 12; i++) {
      int required_numbers = 12 - i;
      int bank_search_window = bank_len - required_numbers + 1;
      struct best_num_index bni_result = get_largest_number(new_bank, position, bank_search_window);
      largest_bank[i] = bni_result.best_num + '0';
      position = bni_result.best_index + 1;
    }

    largest_bank[12] = '\0';
    if (LOG_VERBOSE) printf("INFO: Largest Bank [%s]\n", largest_bank);
    numeric_str_add(counter, largest_bank, counter);
  }

  printf("INFO: Answer [%s]\n", counter);
  close_parser(&pctx);
  return 0;
}
