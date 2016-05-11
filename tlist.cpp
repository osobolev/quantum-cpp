#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "list.h"

static Container data;

static int blockSize;

void initList(Index maxCount, int blockSize)
{
  ::blockSize = blockSize;
}

Index getSize()
{
  return data.size();
}

inline void *getEntry(Value &ptr)
{
  return &ptr;
}

void binarySearch(Number key, Found *found)
{
  found->it = data.find(key);
  if (found->it != data.end()) {
    found->exists = true;
  } else {
    found->exists = false;
    found->it = data.lower_bound(key);
  }
}

void *insertIntoList(Number key)
{
  Iterator i = data.insert(std::make_pair(key, Value()));
  return getEntry(i->second);
}

void *getAt(Found &found, Number *atTime)
{
  if (found.it == data.end()) {
    return NULL;
  } else {
    Iterator i = found.it;
    *atTime = i->first;
    return getEntry(i->second);
  }
}

void *getBefore(Found &found, Number *beforeTime)
{
  if (found.it == data.end() || found.it == data.begin()) {
    return NULL;
  } else {
    Iterator i = found.it;
    i--;
    *beforeTime = i->first;
    return getEntry(i->second);
  }
}

void deleteFirst()
{
  Iterator i = data.begin();
  data.erase(i);
}

Number getFirst()
{
  return data.begin()->first;
}

void initIter(Iterator *iter)
{
  *iter = data.begin();
}

bool moveForward(Iterator *iter)
{
  (*iter)++;
  return *iter != data.end();
}

void *getAt(Iterator &it, Number *atTime)
{
  *atTime = it->first;
  return getEntry(it->second);
}

void iterate(TIterFunc iter)
{
  for (Iterator i = data.begin(); i != data.end(); i++) {
    void *p = getEntry(i->second);
    iter(p);
  }
}
