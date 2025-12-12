#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "../helpers/parser.h"

#define LOG_VERBOSE 0

int find_set(int *parent, int i) {
  if (parent[i] == i) {
    return i;
  }

  int root = find_set(parent, parent[i]);
  parent[i] = root;

  return root;
}

int union_find(int *parent, int *size, int i, int j) {
  int root_i = find_set(parent, i);
  int root_j = find_set(parent, j);

  if (root_i == root_j) return 0;

  if (size[root_i] < size[root_j]) {
    parent[root_i] = root_j;
    size[root_j] += size[root_i];
  } else {
    parent[root_j] = root_i;
    size[root_i] += size[root_j];
  }
  
  return 1;
}

int compare_ints_desc(const void *a, const void *b) {
  return (*(int*)b - *(int*)a);
}

int compare_edges(const void *a, const void *b) {
  const long long *edge_a = (const long long *)a;
  const long long *edge_b = (const long long *)b;

  if (edge_a[0] < edge_b[0]) return -1;
  if (edge_a[0] > edge_b[0]) return 1;
  
  return 0;
}

int main() {
  const char *filename = "big-input.txt";
  const char *filemode = "r";

  struct parser_context pctx;

  init_parser(&pctx, filename, filemode);

  const char *delimeters = "\n";
  const bool allowed_whitespaces = false;

  int x[2048];
  int y[2048];
  int z[2048];
  int sp_idx = 0;
  while (next_token(&pctx, delimeters, allowed_whitespaces)) {
    if (LOG_VERBOSE) printf("INFO: Token [%s]\n", pctx.token);

    size_t token_len = strlen(pctx.token);
    if (token_len == 0) {
        continue;
    }
    int s_idx = 0;
    int n_idx = 0;
    int f_idx = 0;
    while (n_idx < (int)token_len) {
      if (pctx.token[n_idx] == ',') {
	int num_len = n_idx - s_idx;
	char num_chr[num_len+1];
	memcpy(num_chr, pctx.token + s_idx, num_len);
	num_chr[num_len] = '\0';

	int num = atoi (num_chr);
	if (f_idx == 0) x[sp_idx] = num;
	else if (f_idx == 1) y[sp_idx] = num;
	
	s_idx = n_idx + 1;
	f_idx++;
      }
      n_idx++;
    }

    if (s_idx < token_len) {
      int num_len = n_idx - s_idx;
      char num_chr[num_len+1];
      memcpy(num_chr, pctx.token + s_idx, num_len);
      num_chr[num_len] = '\0';

      int num = atoi (num_chr);
      z[sp_idx] = num;
    }
    
    sp_idx++;
  }

  int rows = sp_idx;
  for (int i = 0; i < rows; i++) {
    if (LOG_VERBOSE) printf("INFO: X=[%d] Y=[%d] Z=[%d]\n", x[i], y[i], z[i]);
  }

  static long long edges[1000000][3];
  int c_idx = 0;
  int edge_cnt = 0;
  for (int i = 0; i < rows; i++) {
    for (int j = i + 1; j < rows; j++) {
      // Euclidean distance
      long long dx = x[i] - x[j];
      long long dy = y[i] - y[j];
      long long dz = z[i] - z[j];

      long long dist_sq = (dx * dx) + (dy * dy) + (dz * dz);

      edges[edge_cnt][0] = dist_sq;
      edges[edge_cnt][1] = i;
      edges[edge_cnt][2] = j;
      edge_cnt++;
    }
    c_idx = i;
  }

  // Sort according to edge distance
  qsort(edges, edge_cnt, sizeof(edges[0]), compare_edges);

  if (LOG_VERBOSE) {
    printf("---SORTED EDGES---\n");
    for (int i = 0; i < edge_cnt; i++) {
      printf("INFO: dist=[%lld] i=[%lld] j=[%lld]\n", edges[i][0], edges[i][1], edges[i][2]);
    }
  }

  int parent[2048];
  int size[2048];
 
  for (int i = 0; i < rows; i++) {
    parent[i] = i;
    size[i] = 1;
  }

  long long circuit_first_x = 0;
  long long circuit_last_x = 0;
  int groups = rows;
  for (int i = 0; i < edge_cnt; i++) {
    int p1 = edges[i][1];
    int p2 = edges[i][2];

    int result = union_find(parent, size, p1, p2);
    groups -= result;
    if (result == 1 && groups == 1) {
      circuit_first_x = x[p1];
      circuit_last_x = x[p2];
      break;
    }
  }

  int sizes[2048];
  int sz_idx = 0;
  for (int i = 0; i < rows; i++) {
    if (parent[i] == i) {
      sizes[sz_idx] = size[i];
      sz_idx++;
    }
  }

  // Sort them using qsort
  qsort(sizes, sz_idx, sizeof(int), compare_ints_desc);
  
  if (LOG_VERBOSE) printf("INFO: Circuit Size [%d] Rows [%d]\n", sizes[0], rows);

  long long answer = 0;
  if (LOG_VERBOSE) printf("INFO: First Circuit X [%lld] Last Circuit X [%lld]\n", circuit_first_x, circuit_last_x);
  answer = circuit_first_x * circuit_last_x;
  printf("INFO: Answer [%lld]\n", answer);
  close_parser(&pctx);
    
  return 0;
}
