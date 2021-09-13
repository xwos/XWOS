/**
 * @file
 * @brief C++ Operator: new & delete
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.hxx>
#include <cstdlib>

extern "C" {
  #include <xwos/mm/mempool/allocator.h>
  extern struct xwmm_mempool * cxx_mempool;
}

void * operator new(size_t size)
{
  void * mem;

  xwmm_mempool_malloc(cxx_mempool, size, &mem);
  return mem;
}

void operator delete(void * mem) noexcept
{
  xwmm_mempool_free(cxx_mempool, mem);
}

void operator delete(void * mem, size_t size) noexcept
{
  XWOS_UNUSED(size);
  xwmm_mempool_free(cxx_mempool, mem);
}

void * operator new[](size_t size)
{
  void * mem;

  xwmm_mempool_malloc(cxx_mempool, size, &mem);
  return mem;
}

void operator delete[](void * mem) noexcept
{
  xwmm_mempool_free(cxx_mempool, mem);
}

void operator delete[](void * mem, size_t size) noexcept
{
  XWOS_UNUSED(size);
  xwmm_mempool_free(cxx_mempool, mem);
}
