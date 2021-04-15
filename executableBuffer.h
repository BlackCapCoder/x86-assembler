#ifndef EXECUTABLEBUFFER_H
#define EXECUTABLEBUFFER_H


#include <malloc.h>
#include <sys/mman.h>
#include <unistd.h>
#include <bits/types.h>
#include <stdlib.h>
#include <string.h>
#include <string>


class ExecutableBuffer
{
  const int  pageSize;
  uint8_t  * ptr;
  size_t _size, _limit;

  void expand ()
  {
    // probably not safe
    // ptr     = reinterpret_cast<decltype(ptr)>(realloc(ptr, _limit + pageSize));

    ptr = reinterpret_cast<decltype(ptr)>
      (mremap (ptr, _limit, _limit + pageSize, MREMAP_MAYMOVE));
    _limit += pageSize;
  }

public:

  ExecutableBuffer (uint8_t * ptr, size_t size, size_t limit, const int pageSize)
    : pageSize { pageSize }
    , ptr      { ptr  }
    , _size    { size }
    , _limit   { limit }
  {}

  ExecutableBuffer (const size_t alloc)
    : pageSize { getpagesize() }
    , _size    { 0 }
    , _limit   { alloc }
  {
    if (alloc <= 0)
      return;

    // posix_memalign ((void **) & ptr, pageSize, alloc);
    // mprotect (ptr, _limit, PROT_EXEC | PROT_READ | PROT_WRITE);

    // mmap'ed memory is guaranteed to be page alligned
    ptr = reinterpret_cast<decltype(ptr)> (
        mmap ( nullptr
             , alloc
             , PROT_EXEC | PROT_READ | PROT_WRITE
             , MAP_PRIVATE | MAP_ANONYMOUS
             , -1
             , 0 ));
  }

  ExecutableBuffer () : ExecutableBuffer (getpagesize ()) {}

  ~ExecutableBuffer ()
  {
    // free (ptr);
    munmap (ptr, _limit);
    _size  = _limit = 0;
  }


  inline bool isAligned ()
  {
    return ((unsigned long) ptr) % pageSize == 0;
  };


  inline uint8_t * begin () { return ptr;         }
  inline uint8_t * end   () { return ptr + _size; }
  inline size_t    size  () { return _size;       }

  inline void clear ()
  {
    memset (ptr, 0, _size);
    _size = 0;
  }

  inline void push_back_unsafe (const uint8_t x)
  {
    ptr[_size++] = x;
  }

  inline void push_back (const uint8_t x)
  {
    if (_size >= _limit) expand ();
    push_back_unsafe (x);
  }

  ExecutableBuffer clone ()
  {
    uint8_t * mem;
    posix_memalign ((void **) & mem, pageSize, size());
    mprotect       (mem, _size, PROT_EXEC | PROT_READ | PROT_WRITE);
    memcpy         (mem, ptr, _size);
    return ExecutableBuffer (mem, _size, _limit, pageSize);
  }

  std::string to_hex ()
  {
    static constexpr char alph[] = "0123456789abcdef";
    char str [size() * 2 + 1];
    size_t i = 0;

    for (uint8_t x : *this)
    {
      str[i++] = alph[x >> 4];
      str[i++] = alph[x & 0xF];
    }

    str[i] = 0;
    return str;
  }


private:
  template <class> struct func;

  template <class R, class ... As>
  struct func <R(As...)> { using type = R (*) (As...); };

public:


  template <class T>
  inline typename func<T>::type compile ()
  {
    return reinterpret_cast <typename func<T>::type> (ptr);
  }

};


#endif // ifndef EXECUTABLEBUFFER_H
