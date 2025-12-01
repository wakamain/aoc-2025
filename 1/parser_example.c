#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define BUFFER_SIZE 100

int parse() {
  char token[BUFFER_SIZE];
  
  const char *filename = "small-input.txt";
  const char *filemode = "r";

  FILE *fp = fopen(filename, filemode);
  if (!fp) {
    printf("ERROR: Can't open %s\n", filename);
    return 1;
  } else {
    int c;
    int i=0;
    while ((c = fgetc(fp)) != EOF) {
      if (i >= BUFFER_SIZE) {
	printf("ERROR: Input too long! Terminating program.\n");
	return 1;
      }
      
      if (isalnum(c)) {
	// Add to alphanumeric character to token
	token[i] = c;
	i++;
      } else if (isspace(c)) {
	// Null terminate token
	if (i > 0) {
	  token[i] = '\0';
	  i = 0;
	  printf("INFO: Current Token is: %s\n", token);
	  memset(token, 0, sizeof(token));
	}
      }
    }

    if (i > 0) {
      token[i] = 0;
      printf("INFO: Current Token is: %s\n", token);
    }
    
    fclose(fp);
  }

  
  return 0;
}

int main() {
  parse();
  return 0;
}
