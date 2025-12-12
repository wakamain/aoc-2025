#define MAX_SIZE 1000000

struct Queue {
  int arr[MAX_SIZE];
  int front;
  int rear;
};

void init_q(struct Queue *q);
bool is_empty(struct Queue *q);
bool is_full(struct Queue *q);
void enq(struct Queue *q, int a);
int deq(struct Queue *q);
int q_len(struct Queue *q);
void print_q(struct Queue *q);
