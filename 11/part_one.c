#include <stdio.h>
#include <string.h>
#include "../helpers/parser.h"
#include "../helpers/uthash.h"

#define LOG_VERBOSE 0

#define MAX_KEY_LEN 4
#define MAX_CHILDREN 50

struct Node {
  char key[MAX_KEY_LEN];
  char *children[MAX_CHILDREN];
  size_t child_count;
  long long cached_paths;
  bool is_calculated;
  UT_hash_handle hh;
};

struct Node *graph = NULL;

void print_graph(struct Node *graph) {
  struct Node *node, *tmp;
  HASH_ITER(hh, graph, node, tmp) {
    printf("INFO: [%s] -> ", node->key);
    for (size_t i = 0; i < node->child_count; i++) {
      printf("[%s]", node->children[i]);
    }
    printf("\n");
  }
}

long long dfs(char *key) {
  if (strcmp(key, "out") == 0) return 1;
  
  struct Node *node, *tmp;
  node = NULL;
  HASH_FIND_STR(graph, key, node);
  if (node == NULL) return 0;

  if (node->is_calculated) return node->cached_paths;

  long long total_paths = 0;

  for (size_t i = 0; i < node->child_count; i++) {
    total_paths += dfs(node->children[i]);
  }

  node->cached_paths = total_paths;
  node->is_calculated = false;
  
  return total_paths;
}

int main() {
  const char *filename = "big-input.txt";
  const char *filemode = "r";

  struct parser_context pctx;

  init_parser(&pctx, filename, filemode);

  const char *delimeters = "\n";
  const bool allowed_whitespaces = true;
  int devices = 0;
  while (next_token(&pctx, delimeters, allowed_whitespaces)) {
    const char *token = pctx.token;
    const size_t token_len = strlen(token);
    if (LOG_VERBOSE) printf("INFO: Token [%s]\n", token);

    const char *colon = strchr(token, ':');
    if (!colon) continue;
    
    char lhs_buf[MAX_KEY_LEN];
    size_t lhs_len = colon - token;
    memcpy(lhs_buf, token, lhs_len);
    lhs_buf[lhs_len] = '\0';

    if (LOG_VERBOSE) printf("INFO: LHS [%s] ", lhs_buf);
    
    const char *p = colon + 1;
    while(*p == ' ') p++;
    char rhs_buf[MAX_CHILDREN * 3];
    char rhs_buf_print[MAX_CHILDREN * 3];
    size_t rhs_len = strlen(p);
    memcpy(rhs_buf, p, rhs_len);
    rhs_buf[rhs_len] = '\0';
    strcpy(rhs_buf_print, rhs_buf);

    struct Node *node = NULL;
    HASH_FIND_STR(graph, lhs_buf, node);
    if (node == NULL) {
      node = (struct Node *)malloc(sizeof *node);
      memset(node, 0, sizeof *node);
      strcpy(node->key, lhs_buf);
      node->child_count = 0;
      HASH_ADD_STR(graph, key, node);
    }


    const char *dlm = " ";
    char *tok_p = strtok(rhs_buf_print, dlm);

    if (LOG_VERBOSE) {
      printf("RHS ");
      while (tok_p != NULL) {
	printf("[%s] ", tok_p);
	tok_p = strtok(NULL, dlm);
      }
      printf("\n");
    }

    char *tok = strtok(rhs_buf, dlm);
    while (tok != NULL) {
      if (node->child_count >= MAX_CHILDREN) {
	printf("ERROR: Too many children for key %s\n", lhs_buf);
	return 1;
      }
      
      node->children[node->child_count] = malloc(strlen(tok) + 1);
      strcpy(node->children[node->child_count], tok);
      node->child_count++;
      tok = strtok(NULL, dlm);
    }
    devices++;
  }

  if (LOG_VERBOSE) print_graph(graph);
  long long result = dfs("you");
  printf("INFO: Answer [%llu]\n", result);
  
  close_parser(&pctx);
  return 0;
}
