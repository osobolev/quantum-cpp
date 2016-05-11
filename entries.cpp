#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "graph.h"
#include "entry.h"
#include "list.h"

//#define CHECK_UNDERFLOW

static Number currentTime;
static double startAmp = 0;

static Index *photonCount;
static double *photonEnergy;
static Index totalPhotonCount;
static double totalPhotonEnergy;
static Index *radiation;

static Found found;

static void initInternal()
{
  currentTime = 0;

  photonCount = (Index *) malloc(edgeNum * sizeof(Index));
  photonEnergy = (double *) malloc(edgeNum * sizeof(double));

  radiation = (Index *) calloc(vertexNum * edgeNum, sizeof(Index));
}

inline bool isNear(Number n1, Number n2)
{
  return fabs(n1 - n2) < 1e-10;
}

inline Index *getRadiation(int vertex, int edge) {
  return &radiation[vertex * edgeNum + edge];
}

#include "slow_small.cpp"

Number getCurrentTime()
{
  return currentTime;
}

void startPhotons()
{
  double amp = 1.0;
  for (int i = 0; i < edgeNum; i++) {
    Direction dir = getStartDir(i);
    if (dir != NONE) {
      schedule(getEdgeLength(i), dir, amp, i);
      startAmp += amp;
    }
  }
}

void getStats()
{
  totalPhotonCount = 0;
  totalPhotonEnergy = 0;
  for (int i = 0; i < edgeNum; i++) {
    photonCount[i] = 0;
    photonEnergy[i] = 0;
  }
  iterate(&statFunc);
}

void printStats()
{
  printf("Total photon count: " INDEX_FMT "\n", totalPhotonCount);
  printf("Total photon energy: %lg%+lg\n", startAmp, totalPhotonEnergy - startAmp);
  for (int i = 0; i < edgeNum; i++) {
    printf("On edge %d: %.16lf / " INDEX_FMT "\n", i + 1, photonEnergy[i], photonCount[i]);
  }
}

void printShortStats(FILE *f)
{
  fprintf(f, "%.16lf\t" INDEX_FMT, currentTime, totalPhotonCount);
  for (int i = 0; i < edgeNum; i++) {
    fprintf(f, "\t%.16lf\t" INDEX_FMT, photonEnergy[i], photonCount[i]);
  }
  fprintf(f, "\n");
}

void printRadiationStats(FILE *f)
{
  fprintf(f, "%.16lf", currentTime);
  for (int i = 0; i < vertexNum; i++) {
    for (int j = 0; j < edgeNum; j++) {
      fprintf(f, "\t" INDEX_FMT, *getRadiation(i, j));
    }
  }
  fprintf(f, "\n");
}
