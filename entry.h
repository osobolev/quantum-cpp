#ifndef _ENTRY_H
#define _ENTRY_H

#include <stdio.h>

#include "graph.h"
#include "common.h"

void initEntries(Index maxBlocks, double ampTol);

void startPhotons();

void schedule(Number time, Direction dir, double amp, int edge);

void next();

Number getCurrentTime();

void getStats();

void printStats();

void printShortStats(FILE *f);

void printRadiationStats(FILE *f);

#endif
