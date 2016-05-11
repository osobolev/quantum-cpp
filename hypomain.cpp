#include <stdlib.h>
#include <stdio.h>

#include "graph.h"
#include "calc.h"

int main(int argc, char **argv)
{
  int n = atoi(argv[1]);
  int edge = atoi(argv[2]);
  int init = parseArgs(argc - 2, argv + 2);
  if (init >= 0)
    return init;
  Number length0 = getEdgeLength(edge);
  char buf[1024];
  sprintf(buf, "out%d.txt", n);
  setOutput(buf);
  setEdgeLength(edge, length0 / n);
  mainLoop();
  return 0;
}
