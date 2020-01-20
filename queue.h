#ifndef QUEUE_H
#define QUEUE_H

typedef struct queue {
  int cap;
  int tail;
  int head;
  int *data;
} queue;

int q_not_empty(queue *q) {
  return q->tail > q->head;
}

void q_clear(queue *q) {
  q->head=0;
  q->tail=0;
  memset(q->data, 0, sizeof(int) * q->cap);
}

void q_resize(queue *q, int n) {
  if (q->cap == 0)
    q->data = (int*)calloc(n, sizeof(int));
  else
    q->data = realloc(q->data, sizeof(int) * n);
  q->cap = n;
}

void q_push_back(queue *q, int i) {
  if (q->tail >= q->cap)
    q_resize(q, q->cap == 0 ? 1 : q->cap * 2);
  q->data[q->tail++] = i;
}

void q_print(queue *q) {
  for (int i=q->head; i<q->tail; i++) {
    printf("%d%s", q->data[i], i==(q->tail-1)?"\n" : ", ");
  }
}

int q_pop_front(queue *q) {
  return q->data[q->head++];
}

void q_release(queue *q) {
  free(q->data); q->data = 0;
  q->head = 0; q->tail = 0; q->cap = 0;
}

void q_push_all(queue *q, queue *p) {
  while (q_not_empty(p))
    q_push_back(q, q_pop_front(p));
}

#endif
