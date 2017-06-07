#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<math.h>

#define UNSEEN ('u')
#define FRINGE ('f')
#define INTREE ('t')

struct AdjStruct {
    int to;
    double weight;
};
typedef struct AdjStruct * AdjData;

struct AdjVecNode {
    int size;
    int capacity;
    AdjData * data;
};
typedef struct AdjVecNode * AdjVec;

struct MinPQNode {
    int numVerticies, numPQ;
    int minVertex;
    double oo;
    int * status;
    int * parent;
    double * fringeWgt;
};
typedef struct MinPQNode * MinPQ;

int isEmptyPQ(MinPQ pq) {
    return (pq->numPQ == 0);
}

// does work for getMin
void findMin(MinPQ pq) {
    int v;
    double minWgt;
    minWgt = pq->oo;

    for(v = 1; v<= pq->numVerticies; v++) {
        if(pq->status[v] == FRINGE) {
            if(pq->fringeWgt[v] < minWgt) {
                pq->minVertex = v;
                minWgt = pq->fringeWgt[v];
            }
        }
    }
}

int getMin(MinPQ pq) {
    if(pq->minVertex == -1) 
        findMin(pq);
    return pq->minVertex;
}

int getStatus(MinPQ pq, int id) {
    return (pq->status[id]);
}

int getParent(MinPQ pq, int id) {
    return (pq->parent[id]);
}

double getPriority(MinPQ pq, int id) {
    return (pq->fringeWgt[id]);
}

void delMin(MinPQ pq) {
    int oldMin = getMin(pq);
    pq->status[oldMin] = INTREE;
    pq->minVertex = -1;
    pq->numPQ--;
}

void insertPQ(MinPQ pq, int id, double priority, int par) {
    pq->parent[id] = par;
    pq->fringeWgt[id] = priority;
    pq->status[id] = FRINGE;
    pq->minVertex = -1;
    pq->numPQ++;
}

void decreaseKey(MinPQ pq, int id, double priority, int par) {
    pq->parent[id] = par;
    pq->fringeWgt[id] = priority;
    pq->minVertex = -1;
}

// Constructor for MinPQ
MinPQ createPQ(int n, int status[], double priority[], int parent[]) {
    MinPQ pq = calloc(1, sizeof(struct MinPQNode));
    pq->parent = parent;
    pq->fringeWgt = priority;   
    pq->status = status;
    for(int i = 1; i <= n; i++)
        pq->status[i] = UNSEEN;
    pq->numVerticies = n; 
    pq->numPQ = 0;
    pq->minVertex = -1;
    pq->oo = INFINITY;
    return  pq; 
}

AdjVec makeEmptyAdjVec() {
    AdjVec av = calloc(1, sizeof(struct AdjVecNode));
    av->size = 0;
    // initial capacity is 4 -- change accordingly
    av->capacity = 4;
    av->data = calloc(av->capacity, sizeof(AdjData));
    return av;
}

void initEdges(AdjVec * adjList, int numVertices) {
    for(int i = 1; i <= numVertices; i++)
        adjList[i] = makeEmptyAdjVec();
}

void adjVecPush(AdjVec vec, AdjData pushedData) {
    if(vec->size == vec->capacity) {
        vec->capacity = 2 * vec->capacity;
        vec->data = realloc(vec->data, vec->capacity * sizeof(AdjData));
    }
    vec->data[vec->size] = pushedData;
    vec->size++;   
}

int loadEdges(FILE * input, AdjVec * adjList, int numVertices) {
    int from, to;
    double weight;
    char line[256];
    int edgeCount = 0;
    while(fgets(line, 256, input)) {
        int sscanfRtnVal;
        sscanfRtnVal = sscanf(line, " %d %d %lf ", &from, &to, &weight);
        if(sscanfRtnVal < 2) {
            fprintf(stderr,"There were not enough values on the line being read. Exiting...\n");
            exit(EXIT_FAILURE);
    }
        if(from > numVertices || to > numVertices) {
            fprintf(stderr,"An edge is out of bounds. Exiting...\n");
            exit(EXIT_FAILURE);
        }
        AdjData ad = calloc(1, sizeof(struct AdjStruct));
        ad->to = to;
        ad->weight = weight;
        adjVecPush(adjList[from], ad);
        edgeCount++;
    }
    return edgeCount;
}

void updateFringe(MinPQ pq, AdjVec adjVec, int v) {
    for(int i = 0; i < adjVec->size; i++) {
        int w = adjVec->data[i]->to;
        double newWgt = adjVec->data[i]->weight;
          if(getStatus(pq, w)  == UNSEEN) 
              insertPQ(pq, w,newWgt + getPriority(pq, v), v);
          else if(getStatus(pq, w) == FRINGE) {
              if(newWgt + getPriority(pq, v) < getPriority(pq, w))
                  decreaseKey(pq, w, newWgt + getPriority(pq, v), v);
          }
    }
}

void dijkstraSSSP(int n, int s, AdjVec * adjList, int * parent,double * fringeWgt, int * status) {
    MinPQ pq = createPQ(n, status,fringeWgt, parent);
    insertPQ(pq, s,0.0, -1);
    while(isEmptyPQ(pq)==0) {
        int v = getMin(pq);
        delMin(pq);
        updateFringe(pq, adjList[v], v);
    }
}

void printArrays(int n, int * status, double * fringeWgt, int * parent) {
    printf("vertex  status     cost     parent\n");
    printf("----------------------------------\n");
    for(int i = 1; i<= n; i++) {
        if(fringeWgt[i] > -.00001 && fringeWgt[i] < .00001)
            printf("  %3d      %c         oo       %d\n", i, status[i], parent[i]);
        else
            printf("  %3d      %c     %3.3lf       %d\n", i, status[i], fringeWgt[i], parent[i]);
    }
}

int main(int argc, char * argv[]) {
 
    int start, numVertices, sscanfRtnVal;
    FILE * input;
    char numVerticesBuff[32];

    if(argc < 3) {
        printf("usage: dssp <start-vertex> <input source>\n");
        return EXIT_SUCCESS;
    } 
 
    // open input
    start = atoi(argv[1]);
    if(strcmp(argv[2], "-") != 0)
        input = fopen(argv[2], "r");
    else 
        input = stdin;
    if(input == NULL) {
        fprintf(stderr, "There was an error opening \'%s\'. Exiting...\n", argv[2]);
        return EXIT_FAILURE;
    }
  
    // get numVertices 
    fgets(numVerticesBuff, 32, input);     
    sscanfRtnVal = sscanf(numVerticesBuff, " %d ", &numVertices);
    if(sscanfRtnVal != 1) {
        fprintf(stderr, "First line was not one value. Exiting...\n");
        return EXIT_FAILURE;
    }    

    AdjVec * adjList = calloc(numVertices + 1, sizeof(AdjVec));
    
    initEdges(adjList, numVertices);
    // returns int (number of edges) if needed
    loadEdges(input, adjList, numVertices); 

    // At this point adjList is loaded and represents the graph input by the user
  
    /* print graph for testing
    for(int i = 0; i < numVertices; i++) {
        printf("%2d: ", i + 1);
        for(int j = 0; j < adjList[i]->size; j++) 
           printf("%2d (%2.3lf)  ", adjList[i]->data[j]->to, adjList[i]->data[j]->weight);
        printf("\n");
    }
    */

    int * status = calloc(numVertices + 1, sizeof(int));
    double * fringeWgt = calloc(numVertices + 1, sizeof(double));
    int * parent = calloc(numVertices + 1, sizeof(int));

    // Dijkstra's 
    dijkstraSSSP(numVertices, start, adjList, parent, fringeWgt, status);

    // print results
    printArrays(numVertices, status, fringeWgt, parent);
   
    fclose(input);
    return EXIT_SUCCESS;
}
