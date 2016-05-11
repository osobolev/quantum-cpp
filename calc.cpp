#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "graph.h"
#include "entry.h"
#include "calc.h"
#include "list.h"

static const char *graphFile = "graph.txt";
static Index maxBlocks = 1000000;
static const char *out = NULL;
static Number maxTime = 100.0;
static double logDelta = 1.0;
static double ampTol = 0.0;
static const char *status = NULL;
static const char *stop = NULL;
static const char *rad = NULL;
#ifdef USE_POOL
static size_t poolSize = (size_t) 512 * 1024 * 1024;
#endif

int parseArgs(int argc, const char *argv[])
{
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      char opt = argv[i][1];
      switch (opt) {
      case 'n':
        if (i + 1 < argc) {
          maxBlocks = (Index) atol(argv[++i]);
        }
        break;
      case 'o':
        if (i + 1 < argc) {
          out = argv[++i];
        }
        break;
      case 'd':
        if (i + 1 < argc) {
          logDelta = atof(argv[++i]);
        }
        break;
      case 't':
        if (i + 1 < argc) {
          maxTime = atof(argv[++i]);
        }
        break;
      case 'a':
        if (i + 1 < argc) {
          ampTol = atof(argv[++i]);
        }
        break;
      case 'i':
        if (i + 1 < argc) {
          status = argv[++i];
        }
        break;
      case 's':
        if (i + 1 < argc) {
          stop = argv[++i];
        }
        break;
      case 'r':
        if (i + 1 < argc) {
          rad = argv[++i];
        }
        break;
#ifdef USE_POOL
      case 'p':
        if (i + 1 < argc) {
          size_t mb = atol(argv[++i]);
          poolSize = mb * 1024 * 1024;
        }
        break;
#endif
      case 'h':
        printf("Usage: %s [options] [file]\n", argv[0]);
        printf(" -n <maxBlocks>\n");
        printf(" -o <outFile>\n");
        printf(" -d <logDelta>\n");
        printf(" -t <maxTime>\n");
        printf(" -a <ampTol>\n");
        printf(" -i <infoFile>\n");
        printf(" -s <stopFile>\n");
        printf(" -r <radFile>\n");
#ifdef USE_POOL
        printf(" -p <poolSize, Mb>\n");
#endif
        return 0;
      }
    } else {
      graphFile = argv[i];
    }
  }
  FILE *f = fopen(graphFile, "rt");
  if (f == NULL) {
    printf("Cannot open file %s\n", graphFile);
    return 1;
  }
  read(f);
  fclose(f);
  return -1;
}

void setOutput(char *output)
{
  out = output;
}

void mainLoop()
{
  FILE *fout = out == NULL ? stderr : fopen(out, "w+t");
  FILE *frad = rad == NULL ? NULL : fopen(rad, "w+t");

#ifdef USE_POOL
  pool = createPool(poolSize);
#endif  

  initEntries(maxBlocks, ampTol);
  startPhotons();
  clock_t t0 = clock();
  Number lastTime = getCurrentTime();
  while (1) {
    Number time = getCurrentTime();
    if (time - lastTime >= logDelta) {
      getStats();
      printShortStats(fout);
      fflush(fout);
      if (frad != NULL) {
        printRadiationStats(frad);
        fflush(frad);
      }
      if (status != NULL) {
        FILE *fstatus = fopen(status, "w+t");
        fprintf(fstatus, "%lf\n", time);
        fclose(fstatus);
      }
      if (stop != NULL) {
        FILE *fstop= fopen(stop, "r");
        if (fstop != NULL) {
          fclose(fstop);
          break;
        }
      }
      lastTime = time;
    }
    if (maxTime > 0 && time >= maxTime)
      break;
    next();
  }
  if (out != NULL) {
    fclose(fout);
  }
  if (frad != NULL) {
    fclose(frad);
  }
  printf("Real running time = %lg\n", (double) (clock() - t0) / CLOCKS_PER_SEC);
  getStats();
  printStats();
}
