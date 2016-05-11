#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "graph.h"

int vertexNum;
int degree;
int edgeNum;

static Number *edges; // [numEdges]
static int *dest; // [numEdges * 2]
static int *outEdgesLengths; // [numVertex]
static int **outEdges; // [numVertex]
static Direction *startDir; // [numEdges]

Number getEdgeLength(int edge)
{
  return edges[edge];
}

int *goingOut(int vertex, int *outLength)
{
  assert(vertex >= 0 && vertex < vertexNum);
  *outLength = outEdgesLengths[vertex];
  return outEdges[vertex];
}

int edgeSide(int edge, Direction direction)
{
  int *vert = &dest[edge << 1];
  return vert[reverse(direction)];
}

Direction destination(int source, int edge)
{
  assert(source >= 0 && source < vertexNum);
  assert(edge >= 0 && edge < edgeNum);
  int *vert = &dest[edge << 1];
  if (source == vert[0]) {
    return FWD;
  } else {
    return BACK;
  }
}

int isLoop(int edge)
{
  int *vert = &dest[edge << 1];
  return vert[0] == vert[1];
}

Direction getStartDir(int edge)
{
  return startDir[edge];
}

static int *doAlloc()
{
  edges = (Number *) malloc(edgeNum * sizeof(Number));
  dest = (int *) malloc(edgeNum * 2 * sizeof(int));
  startDir = (Direction *) malloc(edgeNum * sizeof(Direction));
  outEdgesLengths = (int *) malloc(vertexNum * sizeof(int));
  outEdges = (int **) malloc(vertexNum * sizeof(int *));
  return (int *) malloc(vertexNum * degree * sizeof(int));
}

void read(FILE *f)
{
  int i;
  fscanf(f, "%d %d %d", &vertexNum, &edgeNum, &degree);
  int *out = doAlloc();
  for (i = 0; i < edgeNum; i++) {
    fscanf(f, "%lf", &edges[i]);
    startDir[i] = NONE;
  }
  for (i = 0; i < edgeNum; i++) {
    fscanf(f, "%d %d", &dest[i * 2], &dest[i * 2 + 1]);
  }
  for (i = 0; i < vertexNum; i++) {
    int len;
    fscanf(f, "%d", &len);
    outEdgesLengths[i] = len;
    outEdges[i] = &out[i * degree];
    for (int j = 0; j < len; j++) {
      fscanf(f, "%d", &outEdges[i][j]);
    }
  }
  for (;;) {
    int startEdge;
    if (fscanf(f, "%d", &startEdge) != 1)
      break;
    int d;
    Direction dir;
    if (fscanf(f, "%d", &d) == 1) {
      dir = d == 0 ? FWD : BACK;
    } else {
      dir = FWD;
    }
    startDir[startEdge] = dir;
  }
}

void setEdgeLength(int edge, Number length)
{
  edges[edge] = length;
}
