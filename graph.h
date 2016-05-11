#ifndef _GRAPH_H
#define _GRAPH_H

#include <stdio.h>
#include "number.h"

extern int vertexNum;
extern int degree;
extern int edgeNum;

inline Direction reverse(Direction d)
{
  return (Direction) (1 - d);
}

inline Direction flip(int& d)
{
  Direction ret = (Direction) d;
  d = reverse(ret);
  return ret;
}

Number getEdgeLength(int edge);

int *goingOut(int vertex, int *outLength);

Direction destination(int source, int edge);

int edgeSide(int edge, Direction direction);

int isLoop(int edge);

Direction getStartDir(int edge);

void read(FILE *f);

void setEdgeLength(int edge, Number length);

#endif
