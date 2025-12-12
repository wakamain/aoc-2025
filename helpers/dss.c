#include <stdio.h>
#include <stdbool.h>
#include "dss.h"

void init_q(struct Queue *q) {
  q->front = 0;
  q->rear = 0;
}
  
bool is_empty(struct Queue *q) { return q->front == q->rear; }
bool is_full(struct Queue *q) { return (q->rear + 1) % MAX_SIZE == q->front; }

void enq(struct Queue *q, int a) {
  if (is_full(q)) {
    printf("ERROR: Queue is full!\n");
    return;
  }

  q->arr[q->rear] = a;
  q->rear = (q->rear + 1) % MAX_SIZE;
}

int deq(struct Queue *q) {
  if (is_empty(q)) {
    printf("ERROR: Queue is empty!\n");
    return -1; // Is -1 an error code or actual value? Fix this.
  }

  int value = q->arr[q->front];
  q->front = (q->front + 1) % MAX_SIZE;
  return value;
}

int q_len(struct Queue *q) {
  if (q->rear >= q->front) return q->rear - q->front;
  else return MAX_SIZE - q->front + q->rear;
}

void print_q(struct Queue *q) {
  size_t len = q_len(q);
  printf("[");
  int i = q->front;
  while (i != q->rear) {
    if (i != q->rear-1) printf("%d, ", q->arr[i]);
    else printf("%d", q->arr[i]);
    i = (i + 1) % MAX_SIZE;
  }
  printf("]\n");
}
