#include <stdio.h>
#include <string.h>
#include "falloc.h"

typedef int Mask;

static const int BITS = sizeof(Mask) * 8;

typedef struct _Payload {
  char data[SIZE];
}
#ifdef __GNUC__
__attribute__((packed))
#endif
Payload;

typedef struct _Block {
  Mask mask;
  _Block *next;
  _Payload data[BITS];

  void init()
  {
    mask = 0;
    next = NULL;
  }
}
#ifdef __GNUC__
__attribute__((packed))
#endif
Block;

static inline bool isFull(Mask mask)
{
  return mask == -1;
}

struct BlockPool {
private:

  Block *blocks;
  int numBlocks;
  Block *freeHead;

public:

  void *alloc()
  {
    if (freeHead == NULL)
      return NULL;
    int i = ffs(~freeHead->mask) - 1;
    Payload *ptr = &freeHead->data[i];
    freeHead->mask |= 1 << i;
    if (isFull(freeHead->mask)) {
      Block *next = freeHead->next;
      if (next == NULL) {
        next = freeHead + 1;
        if (next - blocks >= numBlocks) {
          next = NULL;
        } else {
          next->init();
        }
      }
      freeHead = next;
    }
    return ptr;
  }

  void release(void *ptr)
  {
    int blockIndex = ((char *) ptr - (char *) blocks) / sizeof(Block);
    Block *block = &blocks[blockIndex];
    int index = (Payload *) ptr - block->data;
    Mask oldMask = block->mask;
    block->mask &= ~(1 << index);
    if (isFull(oldMask)) {
      block->next = freeHead;
      freeHead = block;
    }
  }

  BlockPool(Block *arena, int _numBlocks) : blocks(arena), numBlocks(_numBlocks)
  {
    freeHead = arena;
    freeHead->init();
  }
};

BlockPool *pool;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#if defined(WIN32)
# include <mman.h>
#else
# include <sys/mman.h>
#endif

BlockPool *createPool(size_t maxFileSize, const char *filename)
{
  size_t blockSize = sizeof(Block);
  size_t blocks = (maxFileSize + blockSize - 1) / blockSize;
  size_t memorySize = blocks * blockSize;
  printf("Trying to open memory file %s\n", filename);
  int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXG);
  if (fd < 0) {
    printf("Can't create/open file %s\n", filename);
    return NULL;
  }
  lseek(fd, memorySize + 1, SEEK_SET);
  write(fd, "", 1);
  lseek(fd, 0, SEEK_SET);
  printf("Trying to map file to memory\n");
  void *mmapPtr = mmap(NULL, memorySize, PROT_WRITE | PROT_READ | PROT_EXEC, MAP_PRIVATE | MAP_NORESERVE, fd, 0);
  if (mmapPtr == MAP_FAILED) {
    printf("Can't allocate memory %ld bytes. Error: %s\n", (long) memorySize, strerror(errno));
    close(fd);
    unlink(filename);
    return NULL;
  }
  close(fd);
  printf("Trying to initialize memory pool on base: %p, size: %ld bytes\n", mmapPtr, (long) memorySize);
  BlockPool *pool = new BlockPool((Block *) mmapPtr, blocks);
  printf("Complete pool creation, max capacity %ld items\n", (long) (blocks * BITS));
  return pool;
}

void *alloc(BlockPool *pool)
{
  return pool->alloc();
}

void release(BlockPool *pool, void *p)
{
  pool->release(p);
}
