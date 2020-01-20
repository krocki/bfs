#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include "queue.h"
#include "timer.h"

typedef uint32_t u32;
typedef uint64_t u64;

typedef struct {
  u32 i;
  u32 j;
  float w;
} edge;

typedef struct {
  u64 num_e;
  u32 num_v;
  u32 *n;
  edge **e;
} graph;


#define DEBUG 0
#define INFINITY (1.0/0.0)


/* for sorting edges accordding to their indices */
int edge_cmp(const void *a, const void *b) {

  edge *ea = (edge*)a;
  edge *eb = (edge*)b;

  if (ea->i < eb->i) return -1;
  if (ea->i > eb->i) return 1;
  if (ea->j < eb->j) return -1;
  if (ea->j > eb->j) return 1;
  if (ea->w < eb->w) return -1;
  if (ea->w > eb->w) return 1;

  return 0; /* unlikely */
}
void graph_read(graph *g, char *fname) {

  FILE *f = fopen(fname, "r");
  if (NULL == f) {
    fprintf(stderr, "couldn't open %s\n",
    fname); return;
  }

  printf("reading %s...\n", fname);

  edge *e;

  int ok=0;

  if (1 == fscanf(f, "%u\n", &(g->num_v))) {
    if (1 == fscanf(f, "%llu\n", &(g->num_e))) {
      e = calloc(g->num_e, sizeof(edge));
      g->n = (u32*) calloc( g->num_v, sizeof(u32));
      for (u64 x=0; x<g->num_e; x++) {
        if (3 != fscanf(f, "%u %u %f\n",
          &(e[x].i), &(e[x].j), &(e[x].w))) {
            fprintf(stderr,
            "uh-oh, expected an edge "
            "(line %llu)\n", x+2);
            free(e);
            fclose(f);
            return;
        } else {
        #if DEBUG
          printf("edge #%2d: "
          "(%2d, %2d, %4.2f)\n",
          x, e[x].i, e[x].j, e[x].w);
        #endif
          g->n[e[x].i]++;
        }
        ok=1;
      }
    }
  }

  fclose(f);

  if (0==ok) {
    fprintf(stderr,
      "something wrong with the file\n");
    if (NULL != g->n) free(g->n);
    return;
  } else {
    printf("OK: num_v=%u, num_e=%llu\n",
           g->num_v, g->num_e);
  }

  g->e = (edge**) calloc( g->num_v, sizeof(edge *));
  for (u64 x=0; x<g->num_v; x++) {
    g->e[x] = (edge*) calloc (g->n[x], sizeof(edge));
  }

  qsort(e, g->num_e, sizeof(edge), edge_cmp);

  u64 x = 0, idx = 0, src = e[0].i;
  while (x < g->num_e) {
    while (src == e[x].i) {
      /* memcpy(&(g->e[idx]), &e, sizeof(edge); */
      g->e[src][idx] = e[x];
      idx++; x++;
    }
    idx = 0;
    src = e[x].i;
  }

  free(e);
}

void print_edges(int n, edge *e) {
  for (u64 x=0; x<n; x++) {
    printf("%d %d %f\n", e[x].i, e[x].j, e[x].w);
  }
}

void dump_edges(int n, edge *e, char *fname) {
  FILE *f = fopen(fname, "w");
  if (!f) return;
  for (u64 x=0; x<n; x++) {
    fprintf(f, "%d %d %f\n", e[x].i, e[x].j, e[x].w);
  }
  fclose(f);
}

void graph_show(graph *g) {

  if (NULL == g->e) {
    puts("graph is empty");
  }
  else
  for (u32 v=0; v<g->num_v; v++) {
    printf("%d [%2d]: ", v, g->n[v]);
    for (u32 x=0; x<g->n[v]; x++) {
      printf("%s (#%d; %2d, %2d, %4.2f)",
      x==0 ? "" : ", ", x, g->e[v][x].i,
      g->e[v][x].j, g->e[v][x].w);
    }
    puts("");
  }
}

void graph_free(graph *g) {

  for (u32 v=0; v<g->num_v; v++) {
    if (NULL != g->e[v]) {
      free(g->e[v]); g->e[v] = NULL;
    }
  }

  if (NULL != g->e) {
    free(g->e); g->e = NULL;
  }

  if (NULL != g->n) {
    free(g->n); g->n = NULL;
  }

}

void graph_bfs(graph *g, char *oname) {

  u32 root = g->e[0][0].i;
  int *parent = (int *) malloc(g->num_v * sizeof(int));
  memset(parent, -1, sizeof(int) * g->num_v);

  queue *q = calloc(1, sizeof(queue));
  queue *p = calloc(1, sizeof(queue));
  q_push_back(q, root);

  parent[root] = root;

  //double t0 = get_time();
  int iter = 0;
  while (q_not_empty(q)) {
    while (q_not_empty(q)) {
      int t = q_pop_front(q);
      root = t;
      for (int i=0; i<g->n[root]; i++) {
        int idx = g->e[root][i].j;
        if (parent[idx] < 0) {
          q_push_back(p, idx);
          parent[idx] = root;
        }
      }
    }
    q_clear(q);
    q_push_all(q, p);
  }
  // dump parent array
  //if (NULL != oname) dump_edges(g->num_v, d, oname);
  //double t1 = get_time();
  //printf("time diff=%f\n", t1-t0);
  //for (int i=0; i<g->num_v; i++)
  //  printf("parent of %d = %d\n",
  //    i, parent[i]);
  free(parent);
}

int main(int argc, char **argv) {

  if (argc < 2) {
    printf("usage: %s file [out]\n",
      argv[0]);
    return -1;
  }

  char *fname = argv[1];
  char *oname = NULL;

  if (argc > 2) oname = argv[2];

  graph g;
  timeit(graph_read(&g, fname));
  timeit(graph_bfs(&g, oname));
  graph_free(&g);

  return 0;
}
