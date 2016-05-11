#ifndef _FALLOC_H
#define _FALLOC_H

#include <assert.h>
#include <new>

static const int SIZE = 48;

struct BlockPool;

extern BlockPool *pool;

BlockPool *createPool(size_t maxFileSize, const char *filename = "pool.tmp");
void *alloc(BlockPool *pool);
void release(BlockPool *pool, void *p);

template<typename E>
class mmap_allocator
{
public:

  typedef E value_type;
  typedef ptrdiff_t difference_type;
  typedef size_t size_type;
  typedef E *pointer;
  typedef const E *const_pointer;
  typedef E &reference;
  typedef const E &const_reference;

  pointer address(reference r) const
  { return &r; }
  const_pointer address(const_reference r) const
  { return &r; }

  template <typename U>
  struct rebind { 
    typedef mmap_allocator<U> other; 
  };

  mmap_allocator() throw ()
  { }

  template <typename U>
  mmap_allocator(const mmap_allocator<U> &other) throw ()
  { } 

  mmap_allocator(const mmap_allocator<E> &other) throw ()
  { }

  ~mmap_allocator() throw()
  { }

  void destroy(pointer p)
  { p->~E(); }

  void construct(pointer p, const E &val)
  { ::new((void *) p) E(val); }

  pointer allocate(size_type n, const void * /*hint*/ = 0)
  {
#ifdef DEBUG
    int size = n * sizeof(value_type);
    if (size != SIZE) {
      printf("%d != %d\n", size, SIZE);
    }
    assert(size == SIZE);
#endif    
    void *p = alloc(pool);            
    if (p == NULL)
      throw std::bad_alloc();
    return (E *) p;
  }

  void deallocate(pointer p, size_type n)
  {
    release(pool, p);
  }

  size_type max_size() const throw() 
  { 
    return size_t(-1) / sizeof(E);
  }
};

template<typename T>
inline bool
operator==(const mmap_allocator<T> &, const mmap_allocator<T> &)
{ return true; }

template<typename T>
inline bool
operator!=(const mmap_allocator<T> &, const mmap_allocator<T> &)
{ return false; }

#endif
