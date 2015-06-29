#include "cppsupport.h"
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////

void cppInitialize()
{
    void (**p)(void);
    for (p = &__init_array_start; p != &__init_array_end; ++p)
            (*p)();
}

////////////////////////////////////////////////////////////////////////

void *operator new(size_t size)
{
  //debugf("new: %d (%d)", size, system_get_free_heap_size());
  return malloc(size);
}

void *operator new[](size_t size)
{
  //debugf("new[]: %d (%d)", size, system_get_free_heap_size());
  return malloc(size);
}

void operator delete(void * ptr)
{
  if (ptr != NULL)
    free(ptr);
}

void operator delete[](void * ptr)
{
  if (ptr != NULL)
    free(ptr);
}

extern "C" void __cxa_pure_virtual(void)
{
  printf("Bad pure_virtual_call");
  system_restart();
}

extern "C" void __cxa_deleted_virtual(void)
{
  printf("Bad deleted_virtual_call");
  system_restart();
}

extern "C" void __cxa_guard_acquire(uint64_t* guard_object)
{
}

extern "C" void __cxa_guard_release(uint64_t* guard_object)
{
}

extern "C" void __cxa_guard_abort(uint64_t* guard_object)
{
}
