#ifndef _LIST_H
#define _LIST_H

#include "common.h"
#include "number.h"

#include <map>

#include "small_entry.h"
typedef Entry Value;

//#define USE_POOL

#ifdef USE_POOL
#include "falloc.h"
typedef std::multimap< Number, Value, std::less<Number>, mmap_allocator< std::pair<const Number, Value> > > Container;
#else
typedef std::multimap<Number, Value> Container;
#endif

typedef Container::iterator Iterator;

typedef struct {
  bool exists;
  Iterator it;
} Found;

void initList(Index maxBlocks, int blockSize);

Index getSize();

void initIter(Iterator *iter);

bool moveForward(Iterator *iter);

void *getAt(Iterator &it, Number *atTime);

typedef void (*TIterFunc)(void *elem);

void iterate(TIterFunc iter);

void *getAt(Found &found, Number *atTime);

void *getBefore(Found &found, Number *beforeTime);

void binarySearch(Number key, Found *found);

void *insertIntoList(Number key);

Number getFirst();

void deleteFirst();

#endif
