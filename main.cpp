#include "calc.h"

int main(int argc, const char *argv[])
{
  int init = parseArgs(argc, argv);
  if (init >= 0)
    return init;
  mainLoop();
  return 0;
}
