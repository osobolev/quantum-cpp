#ifndef _COMMON_H
#define _COMMON_H

#include <sys/types.h>

typedef int Index;

#ifdef __GNUC__
#  define SIZE_FMT "%zu"
#else
#  define SIZE_FMT "%u"
#endif

// depends on sizeof(Index)!!!
#define INDEX_FMT "%d"

inline size_t ofs(Index i, int size)
{
  return (size_t) i * size;
}

#endif
