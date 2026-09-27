/* =====================================================================
   Social Network Graph Analysis
   Compares Adjacency Matrix vs Adjacency List representations for:
     a) Construction, BFS, DFS
     b) Vertex/edge search operation
     c) Space, traversal behaviour, search cost, time complexity
   Network: A-B, A-C, B-D, B-E, C-F, E-F   (undirected, unweighted)
   ===================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ---------------------- Adjacency List node ---------------------- */
typedef struct Node {
    int vertex;
    struct Node *next;
} Node;

/* ---------------------- Globals for the small demo graph ---------------------- */
#define MAXN 6
char labels[MAXN];
int  N;                       /* number of vertices           */
int  matrix[MAXN][MAXN];      /* adjacency matrix              */
Node *list[MAXN];             /* adjacency list heads          */

long long matrixOps = 0;      /* operation counters             */
long long listOps   = 0;

/* ---------------------- Utility ---------------------- */
int indexOf(char c) {
    for (int i = 0; i < N; i++) if (labels[i] == c) return i;
    return -1;
}

void addEdgeMatrix(int u, int v) {
    matrix[u][v] = 1;
    matrix[v][u] = 1;
}

/* insert at head (O(1)) -- lists are reversed afterwards to restore
   the original edge-input order for readability of the trace          */
void addEdgeListHead(int u, int v) {
    Node *a = (Node*)malloc(sizeof(Node));
    a->vertex = v; a->next = list[u]; list[u] = a;
    Node *b = (Node*)malloc(sizeof(Node));
    b->vertex = u; b->next = list[v]; list[v] = b;
}

void reverseList(int u) {
    Node *prev = NULL, *cur = list[u], *nxt;
    while (cur) { nxt = cur->next; cur->next = prev; prev = cur; cur = nxt; }
    list[u] = prev;
}

/* ---------------------- Printing ---------------------- */
void printMatrix() {
    printf("\n--- Adjacency Matrix ---\n    ");
    for (int i = 0; i < N; i++) printf("%c ", labels[i]);
    printf("\n");
    for (int i = 0; i < N; i++) {
        printf("%c | ", labels[i]);
        for (int j = 0; j < N; j++) printf("%d ", matrix[i][j]);
        printf("\n");
    }
}

void printList() {
    printf("\n--- Adjacency List ---\n");
    for (int i = 0; i < N; i++) {
        printf("%c -> ", labels[i]);
        for (Node *p = list[i]; p; p = p->next) printf("%c ", labels[p->vertex]);
        printf("\n");
    }
}

/* =====================================================================
   BFS -- Matrix version, with step-by-step trace
   ===================================================================== */
void bfsMatrix(int start) {
    int visited[MAXN] = {0};
    int queue[MAXN], front = 0, rear = 0;
    int step = 0;
    long long ops = 0;

    printf("\n--- BFS (Adjacency Matrix) starting at %c ---\n", labels[start]);
    printf("%-5s %-8s %-20s %-20s\n", "Step", "Visit", "Queue(after)", "Visited-so-far");

    visited[start] = 1;
    queue[rear++] = start;

    while (front < rear) {
        int u = queue[front++];
        step++;
        for (int v = 0; v < N; v++) {
            ops++;                       /* one matrix cell inspection */
            if (matrix[u][v] == 1 && !visited[v]) {
                visited[v] = 1;
                queue[rear++] = v;
            }
        }
        /* print trace row */
        printf("%-5d %-8c ", step, labels[u]);
        char qbuf[64] = "", vbuf[64] = "";
        for (int i = front; i < rear; i++) { char t[4]; sprintf(t, "%c ", labels[queue[i]]); strcat(qbuf, t); }
        for (int i = 0; i < N; i++) if (visited[i]) { char t[4]; sprintf(t, "%c ", labels[i]); strcat(vbuf, t); }
        printf("%-20s %-20s\n", qbuf, vbuf);
    }
    printf("BFS traversal order: ");
    for (int i = 0; i < rear; i++) printf("%c ", labels[queue[i]]);
    printf("\nMatrix cell inspections (ops) = %lld\n", ops);
    matrixOps += ops;
}

/* =====================================================================
   BFS -- List version, with step-by-step trace
   ===================================================================== */
void bfsList(int start) {
    int visited[MAXN] = {0};
    int queue[MAXN], front = 0, rear = 0;
    int step = 0;
    long long ops = 0;

    printf("\n--- BFS (Adjacency List) starting at %c ---\n", labels[start]);
    printf("%-5s %-8s %-20s %-20s\n", "Step", "Visit", "Queue(after)", "Visited-so-far");

    visited[start] = 1;
    queue[rear++] = start;

    while (front < rear) {
        int u = queue[front++];
        step++;
        for (Node *p = list[u]; p; p = p->next) {
            ops++;                       /* one list node traversal */
            int v = p->vertex;
            if (!visited[v]) { visited[v] = 1; queue[rear++] = v; }
        }
        printf("%-5d %-8c ", step, labels[u]);
        char qbuf[64] = "", vbuf[64] = "";
        for (int i = front; i < rear; i++) { char t[4]; sprintf(t, "%c ", labels[queue[i]]); strcat(qbuf, t); }
        for (int i = 0; i < N; i++) if (visited[i]) { char t[4]; sprintf(t, "%c ", labels[i]); strcat(vbuf, t); }
        printf("%-20s %-20s\n", qbuf, vbuf);
    }
    printf("BFS traversal order: ");
    for (int i = 0; i < rear; i++) printf("%c ", labels[queue[i]]);
    printf("\nList node traversals (ops) = %lld\n", ops);
    listOps += ops;
}

/* =====================================================================
   DFS -- Matrix version (iterative, explicit stack), with trace
   ===================================================================== */
void dfsMatrix(int start) {
    int visited[MAXN] = {0};
    int stack[MAXN], top = -1;
    int order[MAXN], oc = 0;
    int step = 0;
    long long ops = 0;

    printf("\n--- DFS (Adjacency Matrix) starting at %c ---\n", labels[start]);
    printf("%-5s %-8s %-20s\n", "Step", "Visit", "Stack(after push)");

    stack[++top] = start;
    while (top >= 0) {
        int u = stack[top--];
        if (visited[u]) continue;
        visited[u] = 1;
        order[oc++] = u;
        step++;
        /* push neighbours in reverse label order so smallest label is popped first */
        for (int v = N - 1; v >= 0; v--) {
            ops++;
            if (matrix[u][v] == 1 && !visited[v]) stack[++top] = v;
        }
        printf("%-5d %-8c ", step, labels[u]);
        char sbuf[64] = "";
        for (int i = 0; i <= top; i++) { char t[4]; sprintf(t, "%c ", labels[stack[i]]); strcat(sbuf, t); }
        printf("%-20s\n", sbuf);
    }
    printf("DFS traversal order: ");
    for (int i = 0; i < oc; i++) printf("%c ", labels[order[i]]);
    printf("\nMatrix cell inspections (ops) = %lld\n", ops);
    matrixOps += ops;
}

/* =====================================================================
   DFS -- List version (iterative, explicit stack), with trace
   ===================================================================== */
void dfsList(int start) {
    int visited[MAXN] = {0};
    int stack[MAXN], top = -1;
    int order[MAXN], oc = 0;
    int step = 0;
    long long ops = 0;

    printf("\n--- DFS (Adjacency List) starting at %c ---\n", labels[start]);
    printf("%-5s %-8s %-20s\n", "Step", "Visit", "Stack(after push)");

    stack[++top] = start;
    while (top >= 0) {
        int u = stack[top--];
        if (visited[u]) continue;
        visited[u] = 1;
        order[oc++] = u;
        step++;
        /* collect neighbours then push in reverse so list order is preserved on pop */
        int nbrs[MAXN], nc = 0;
        for (Node *p = list[u]; p; p = p->next) { ops++; nbrs[nc++] = p->vertex; }
        for (int i = nc - 1; i >= 0; i--) if (!visited[nbrs[i]]) stack[++top] = nbrs[i];

        printf("%-5d %-8c ", step, labels[u]);
        char sbuf[64] = "";
        for (int i = 0; i <= top; i++) { char t[4]; sprintf(t, "%c ", labels[stack[i]]); strcat(sbuf, t); }
        printf("%-20s\n", sbuf);
    }
    printf("DFS traversal order: ");
    for (int i = 0; i < oc; i++) printf("%c ", labels[order[i]]);
    printf("\nList node traversals (ops) = %lld\n", ops);
    listOps += ops;
}

/* =====================================================================
   Search operation: does edge (u,v) exist?  -- with operation trace
   ===================================================================== */
int searchEdgeMatrix(int u, int v, long long *ops) {
    (*ops)++;                       /* single direct index access */
    return matrix[u][v];
}

int searchEdgeList(int u, int v, long long *ops, int verbose) {
    int step = 0;
    for (Node *p = list[u]; p; p = p->next) {
        (*ops)++; step++;
        if (verbose) printf("   compare %d: %c -> %c ?\n", step, labels[u], labels[p->vertex]);
        if (p->vertex == v) return 1;
    }
    return 0;
}

void runSearch(int u, int v) {
    long long opsM = 0, opsL = 0;
    printf("\n--- Search operation: does edge (%c,%c) exist? ---\n", labels[u], labels[v]);
    int rm = searchEdgeMatrix(u, v, &opsM);
    printf("Adjacency Matrix : result=%s, operations=%lld (direct index matrix[%c][%c])\n",
           rm ? "FOUND" : "NOT FOUND", opsM, labels[u], labels[v]);
    int rl = searchEdgeList(u, v, &opsL, 1);
    printf("Adjacency List   : result=%s, operations=%lld (linear scan of %c's list)\n",
           rl ? "FOUND" : "NOT FOUND", opsL, labels[u]);
}

/* =====================================================================
   Load the small demo graph from input.txt.
   Search queries are parsed and stored (not executed yet) so that the
   caller can run BFS/DFS first and the search demo afterwards, matching
   the assignment's part (a) -> part (b) ordering.
   ===================================================================== */
int  queryU[16], queryV[16], numQueries = 0;

void loadGraph(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) { fprintf(stderr, "Cannot open %s\n", path); exit(1); }

    fscanf(f, "%d", &N);
    for (int i = 0; i < N; i++) fscanf(f, " %c", &labels[i]);

    int m; fscanf(f, "%d", &m);
    int edges[64][2];
    for (int i = 0; i < m; i++) {
        char a, b; fscanf(f, " %c %c", &a, &b);
        edges[i][0] = indexOf(a); edges[i][1] = indexOf(b);
    }
    memset(matrix, 0, sizeof(matrix));
    for (int i = 0; i < N; i++) list[i] = NULL;
    for (int i = 0; i < m; i++) { addEdgeMatrix(edges[i][0], edges[i][1]); addEdgeListHead(edges[i][0], edges[i][1]); }
    for (int i = 0; i < N; i++) reverseList(i);

    int q; fscanf(f, "%d", &q);
    numQueries = q;
    for (int i = 0; i < q; i++) {
        char a, b; fscanf(f, " %c %c", &a, &b);
        queryU[i] = indexOf(a); queryV[i] = indexOf(b);
    }
    printf("Loaded graph with %d vertices, %d edges, %d search queries.\n", N, m, q);
    fclose(f);
}

/* =====================================================================
   PART 2: Scalability experiment on larger random sparse graphs
   to empirically demonstrate time/space complexity growth.
   ===================================================================== */
typedef struct { int *heads; Node **nodes_pool; } BigList; /* not used, simple version below */

void scalabilityTest() {
    printf("\n\n===================== SCALABILITY EXPERIMENT =====================\n");
    printf("Sparse random graphs (E approx 2*V), average of 20000 random edge\n");
    printf("searches, times measured with clock().\n\n");
    printf("%-8s %-14s %-14s %-16s %-16s %-18s %-18s\n",
           "V", "E", "MatrixMem(B)", "ListMem(B)", "BFS-Mat(ms)", "BFS-List(ms)", "Search(Mat/List us)");

    int sizes[] = {100, 500, 1000, 2000, 4000};
    for (int s = 0; s < 5; s++) {
        int n = sizes[s];
        int targetE = 2 * n;

        /* allocate matrix */
        int **mat = (int**)malloc(n * sizeof(int*));
        for (int i = 0; i < n; i++) mat[i] = (int*)calloc(n, sizeof(int));

        Node **lst = (Node**)calloc(n, sizeof(Node*));

        srand(42 + n);
        int e = 0;
        /* spanning tree to keep graph connected */
        for (int i = 1; i < n; i++) {
            int u = i, v = rand() % i;
            if (!mat[u][v]) {
                mat[u][v] = mat[v][u] = 1;
                Node *a = malloc(sizeof(Node)); a->vertex = v; a->next = lst[u]; lst[u] = a;
                Node *b = malloc(sizeof(Node)); b->vertex = u; b->next = lst[v]; lst[v] = b;
                e++;
            }
        }
        while (e < targetE) {
            int u = rand() % n, v = rand() % n;
            if (u != v && !mat[u][v]) {
                mat[u][v] = mat[v][u] = 1;
                Node *a = malloc(sizeof(Node)); a->vertex = v; a->next = lst[u]; lst[u] = a;
                Node *b = malloc(sizeof(Node)); b->vertex = u; b->next = lst[v]; lst[v] = b;
                e++;
            }
        }

        long matMem  = (long)n * n * sizeof(int);
        long listMem = (long)2 * e * sizeof(Node) + (long)n * sizeof(Node*);

        /* BFS timing - matrix */
        clock_t t0 = clock();
        {
            int *visited = calloc(n, sizeof(int));
            int *queue = malloc(n * sizeof(int));
            int front = 0, rear = 0;
            visited[0] = 1; queue[rear++] = 0;
            while (front < rear) {
                int u = queue[front++];
                for (int v = 0; v < n; v++) if (mat[u][v] && !visited[v]) { visited[v] = 1; queue[rear++] = v; }
            }
            free(visited); free(queue);
        }
        double bfsMatMs = 1000.0 * (clock() - t0) / CLOCKS_PER_SEC;

        /* BFS timing - list */
        t0 = clock();
        {
            int *visited = calloc(n, sizeof(int));
            int *queue = malloc(n * sizeof(int));
            int front = 0, rear = 0;
            visited[0] = 1; queue[rear++] = 0;
            while (front < rear) {
                int u = queue[front++];
                for (Node *p = lst[u]; p; p = p->next) if (!visited[p->vertex]) { visited[p->vertex] = 1; queue[rear++] = p->vertex; }
            }
            free(visited); free(queue);
        }
        double bfsListMs = 1000.0 * (clock() - t0) / CLOCKS_PER_SEC;

        /* Search timing: 20000 random edge-existence queries */
        int trials = 20000;
        srand(7);
        t0 = clock();
        volatile int dummy = 0;
        for (int i = 0; i < trials; i++) { int u = rand()%n, v = rand()%n; dummy += mat[u][v]; }
        double searchMatUs = 1e6 * (clock() - t0) / CLOCKS_PER_SEC / trials;

        srand(7);
        t0 = clock();
        for (int i = 0; i < trials; i++) {
            int u = rand()%n, v = rand()%n;
            for (Node *p = lst[u]; p; p = p->next) if (p->vertex == v) { dummy++; break; }
        }
        double searchListUs = 1e6 * (clock() - t0) / CLOCKS_PER_SEC / trials;

        printf("%-8d %-14d %-14ld %-16ld %-16.4f %-16.4f %-9.4f/%-8.4f\n",
               n, e, matMem, listMem, bfsMatMs, bfsListMs, searchMatUs, searchListUs);

        for (int i = 0; i < n; i++) free(mat[i]);
        free(mat);
        for (int i = 0; i < n; i++) { Node *p = lst[i]; while (p) { Node *t = p; p = p->next; free(t); } }
        free(lst);
    }
    printf("====================================================================\n");
}

int main() {
    printf("=====================================================================\n");
    printf(" SOCIAL NETWORK GRAPH ANALYSIS -- Adjacency Matrix vs Adjacency List\n");
    printf(" Network: A-B, A-C, B-D, B-E, C-F, E-F\n");
    printf("=====================================================================\n");

    loadGraph("input.txt");
    printMatrix();
    printList();

    int start = indexOf('A');
    bfsMatrix(start);
    bfsList(start);
    dfsMatrix(start);
    dfsList(start);

    printf("\n\n===================== PART (b): SEARCH OPERATION =====================\n");
    for (int i = 0; i < numQueries; i++) runSearch(queryU[i], queryV[i]);

    printf("\n--- Cumulative operation totals for this run (BFS+DFS+searches) ---\n");
    printf("Total Adjacency-Matrix operations = %lld\n", matrixOps);
    printf("Total Adjacency-List   operations = %lld\n", listOps);

    scalabilityTest();

    return 0;
}
