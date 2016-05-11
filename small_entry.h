#ifndef _SMALL_ENTRY_H
#define _SMALL_ENTRY_H

#include "number.h"

typedef struct {
  double amp;
  Direction direction;
  Small edge;
} 
#ifdef __GNUC__
__attribute__((packed))
#endif
Entry;

#endif
