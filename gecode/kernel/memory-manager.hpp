/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 *  Main authors:
 *     Christian Schulte <schulte@gecode.org>
 *
 *  Contributing authors:
 *     Guido Tack <tack@gecode.org>
 *
 *  Copyright:
 *     Christian Schulte, 2002
 *     Guido Tack, 2004
 *
 *  Last modified:
 *     $Date$ by $Author$
 *     $Revision$
 *
 *  This file is part of Gecode, the generic constraint
 *  development environment:
 *     http://www.gecode.org
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software and associated documentation files (the
 *  "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so, subject to
 *  the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 *  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

namespace Gecode {

  /**
   * \brief Base-class for freelist-managed objects
   *
   * Freelist-managed object must inherit from this class. The size
   * of objects of subclasses is defined by the parameters in
   * Gecode::MemoryConfig.
   * \ingroup FuncMemSpace
   */
  class FreeList {
  protected:
    /// Pointer to next freelist object
    FreeList* _next;
  public:
    /// Use uninitialized
    FreeList(void);
    /// Initialize with next freelist object \a n
    FreeList(FreeList* n);
    /// Return next freelist object
    FreeList* next(void) const;
    /// Return pointer to next link in freelist object
    FreeList** nextRef(void);
    /// Set next freelist object to \a n
    void next(FreeList* n);
  };

  /// Manage memory for space
  class MemoryManager {
  private:
    /// Memory-chunks allocated from heap
    class HeapChunk {
    public:
      /// Next heap chunk already allocated
      HeapChunk* next;
      /// Start of memory area inside chunk
      double area[1];
    };
  public:
    /// Constructor initialization
    MemoryManager(void);
    /// Constructor during cloning \a mm and for a memory area for subscriptions of size \a s_sub
    MemoryManager(MemoryManager& mm, size_t s_sub);
    /// Release all allocated heap chunks
    ~MemoryManager(void);

  private:
    size_t     cur_hcsz;  ///< Current heap chunk size
    HeapChunk* cur_hc;    ///< Current heap chunk
    size_t     requested; ///< Total amount of heap memory requested

    char*  start; ///< Start of current heap area used for allocation
    size_t lsz;   ///< Size left for allocation

    /// Refill current heap area (outlined) issued by request of size \a s
    GECODE_KERNEL_EXPORT
    void alloc_refill(size_t s);
    /// Do the real work for refilling
    void alloc_fill(size_t s, bool first);

  public:
    /// Allocate memory of size \a s
    void* alloc(size_t s);
    /// Return how much memory has been allocated
    size_t allocated(void) const;
    /// Get the memory area for subscriptions
    void* subscriptions(void) const;

  private:
    /// Start of free lists
    FreeList* fl[MemoryConfig::fl_size_max-MemoryConfig::fl_size_min+1];
    /// Refill free list
    template <size_t> void fl_refill(void);
    /// Translate size to index in free list
    static size_t sz2i(size_t);
    /// Translate index in free list to size
    static size_t i2sz(size_t);

  public:
    /// Allocate free list element of size \a s
    template <size_t s>
    void* fl_alloc(void);
    /// Release all free list elements of size \a s between f and l (inclusive)
    template <size_t> void  fl_dispose(FreeList* f, FreeList* l);

  private:
    /// Memory-chunks for reusing slack memory
    class ReuseChunk {
    public:
      /// Size of chunk
      size_t      size;
      /// Next chunk for reusal
      ReuseChunk* next;
    };
    /// Slack memory chunks
    ReuseChunk* slack;
  public:
    /// Store for reusal, if of sufficient size for free list
    void reuse(void* p, size_t s);
  };

  class Region;

  /// Shared object for scratch area
  class SharedRegionArea {
    friend class Region;
  private:
    /// How many spaces use this scratch area object
    unsigned int use_cnt;
    /// Amount of free memory
    size_t free;
    /// The actual memory area (allocated from top to bottom)
    double area[MemoryConfig::region_area_size / sizeof(double)];
  public:
    /// Initialize
    SharedRegionArea(void);
    /// Return memory block if available
    bool alloc(size_t s, void*& p);
    /// Return copy during cloning
    SharedRegionArea* copy(bool share);
    /// Release by one space
    bool release(void);
    /// Allocate memory from heap
    static void* operator new(size_t s);
    /// Free memory allocated from heap
    static void  operator delete(void* p);
  };


  /*
   * Freelists
   *
   */

  forceinline
  FreeList::FreeList(void) {}

  forceinline
  FreeList::FreeList(FreeList* n)
    : _next(n) {}

  forceinline FreeList*
  FreeList::next(void) const {
    return _next;
  }

  forceinline FreeList**
  FreeList::nextRef(void) {
    return &_next;
  }

  forceinline void
  FreeList::next(FreeList* n) {
    _next = n;
  }

  forceinline size_t
  MemoryManager::sz2i(size_t s) {
    assert(s >= (MemoryConfig::fl_size_min << MemoryConfig::fl_unit_size));
    assert(s <= (MemoryConfig::fl_size_max << MemoryConfig::fl_unit_size));
    return (s >> MemoryConfig::fl_unit_size) - MemoryConfig::fl_size_min;
  }

  forceinline size_t
  MemoryManager::i2sz(size_t i) {
    return (i + MemoryConfig::fl_size_min) << MemoryConfig::fl_unit_size;
  }


  /*
   * The active memory manager
   *
   */

  forceinline size_t
  MemoryManager::allocated(void) const {
    return requested;
  }

  forceinline void*
  MemoryManager::alloc(size_t sz) {
    // Size must be a multiple of four
    assert(sz > 0);
    // Perform alignment
    MemoryConfig::align(sz);
    // Check whether sufficient memory left
    if (sz > lsz)
      alloc_refill(sz);
    lsz -= sz;
    return start + lsz;
  }

  forceinline void*
  MemoryManager::subscriptions(void) const {
    return &cur_hc->area[0];
  }

  forceinline void
  MemoryManager::alloc_fill(size_t sz, bool first) {
    // Adjust current heap chunk size
    if (((requested > MemoryConfig::hcsz_inc_ratio*cur_hcsz) ||
         (sz > cur_hcsz)) &&
        (cur_hcsz < MemoryConfig::hcsz_max)) {
      cur_hcsz <<= 1;
    }
    // Increment the size that it caters for the initial overhead
    size_t overhead = sizeof(HeapChunk) - sizeof(double);
    sz += overhead;
    // Round size to next multiple of current heap chunk size
    size_t allocate = ((sz > cur_hcsz) ?
                       (((size_t) (sz / cur_hcsz)) + 1) * cur_hcsz : cur_hcsz);
    // Request a chunk of size allocate
    HeapChunk* hc = static_cast<HeapChunk*>(heap.ralloc(allocate));
    start = Support::ptr_cast<char*>(&hc->area[0]);
    lsz   = allocate - overhead;
    // Link heap chunk, where the first heap chunk is kept in place
    if (first) {
      requested = allocate;
      hc->next = NULL; cur_hc = hc;
    } else {
      requested += allocate;
      hc->next = cur_hc->next; cur_hc->next = hc;
    }
#ifdef GECODE_MEMORY_CHECK
    for (char* c = start; c < (start+lsz); c++)
      *c = 0;
#endif
  }

  forceinline
  MemoryManager::MemoryManager(void)
    : cur_hcsz(MemoryConfig::hcsz_min), requested(0), slack(NULL) {
    alloc_fill(cur_hcsz,true);
    for (size_t i = MemoryConfig::fl_size_max-MemoryConfig::fl_size_min+1;
         i--; )
      fl[i] = NULL;
  }

  forceinline
  MemoryManager::MemoryManager(MemoryManager& mm, size_t s_sub)
    : cur_hcsz(mm.cur_hcsz), requested(0), slack(NULL) {
    MemoryConfig::align(s_sub);
    if ((mm.requested < MemoryConfig::hcsz_dec_ratio*mm.cur_hcsz) &&
        (cur_hcsz > MemoryConfig::hcsz_min) &&
        (s_sub*2 < cur_hcsz))
      cur_hcsz >>= 1;
    alloc_fill(cur_hcsz+s_sub,true);
    // Skip the memory area at the beginning for subscriptions
    lsz   -= s_sub;
    start += s_sub;
    for (size_t i = MemoryConfig::fl_size_max-MemoryConfig::fl_size_min+1;
         i--; )
      fl[i] = NULL;
  }

  forceinline
  MemoryManager::~MemoryManager(void) {
    // Release all allocated heap chunks
    HeapChunk* hc = cur_hc;
    do {
      HeapChunk* t = hc; hc = hc->next;
      heap.rfree(t);
    } while (hc != NULL);
  }



  /*
   * Slack memory management
   *
   */
  forceinline void
  MemoryManager::reuse(void* p, size_t s) {
#ifdef GECODE_MEMORY_CHECK
    {
      char* c = static_cast<char*>(p);
      char* e = c + s;
      while (c < e) {
        *c = 0; c++;
      }
    }
#endif
    if (s < (MemoryConfig::fl_size_min<<MemoryConfig::fl_unit_size))
      return;
    if (s > (MemoryConfig::fl_size_max<<MemoryConfig::fl_unit_size)) {
      ReuseChunk* rc = static_cast<ReuseChunk*>(p);
      rc->next = slack;
      rc->size = s;
      slack = rc;
    } else {
      size_t i = sz2i(s);
      FreeList* f = static_cast<FreeList*>(p);
      f->next(fl[i]); fl[i]=f;
    }
  }


  /*
   * Freelist management
   *
   */

  template <size_t s>
  forceinline void*
  MemoryManager::fl_alloc(void) {
    size_t i = sz2i(s);
    FreeList* f = fl[i];
    if (f == NULL) {
      fl_refill<s>(); f = fl[i];
    }
    FreeList* n = f->next();
    fl[i] = n;
    return f;
  }

  template <size_t s>
  forceinline void
  MemoryManager::fl_dispose(FreeList* f, FreeList* l) {
    size_t i = sz2i(s);
    l->next(fl[i]); fl[i] = f;
  }

  template <size_t sz>
  void
  MemoryManager::fl_refill(void) {
    // Try to acquire memory from slack
    if (slack != NULL) {
      ReuseChunk* m = slack;
      slack = NULL;
      do {
        char*  block = Support::ptr_cast<char*>(m);
        size_t s     = m->size;
        assert(s >= sz);
        m = m->next;
        fl[sz2i(sz)] = Support::ptr_cast<FreeList*>(block);
        while (s >= 2*sz) {
          Support::ptr_cast<FreeList*>(block)->next
            (Support::ptr_cast<FreeList*>(block+sz));
          block += sz;
          s     -= sz;
        }
        Support::ptr_cast<FreeList*>(block)->next(NULL);
      } while (m != NULL);
    } else {
      char* block = static_cast<char*>(alloc(MemoryConfig::fl_refill*sz));
      fl[sz2i(sz)] = Support::ptr_cast<FreeList*>(block);
      int i = MemoryConfig::fl_refill-2;
      do {
        Support::ptr_cast<FreeList*>(block+i*sz)->next
          (Support::ptr_cast<FreeList*>(block+(i+1)*sz));
      } while (--i >= 0);
      Support::ptr_cast<FreeList*>(block+
                                   (MemoryConfig::fl_refill-1)*sz)->next
        (Support::ptr_cast<FreeList*>(NULL));
    }
  }

  /*
   * Shared region area
   *
   */

  forceinline void*
  SharedRegionArea::operator new(size_t s) {
    return heap.ralloc(s);
  }
  forceinline void
  SharedRegionArea::operator delete(void* p) {
    heap.rfree(p);
  }
  forceinline
  SharedRegionArea::SharedRegionArea(void)
    : use_cnt(1), free(MemoryConfig::region_area_size) {}
  forceinline SharedRegionArea*
  SharedRegionArea::copy(bool share) {
    if (share) {
      use_cnt++;
      return this;
    } else {
      return new SharedRegionArea();
    }
  }
  forceinline bool
  SharedRegionArea::release(void) {
    return --use_cnt == 0;
  }
  forceinline bool
  SharedRegionArea::alloc(size_t s, void*& p) {
    MemoryConfig::align(s);
    if (s > free)
      return false;
    free -= s;
    p = Support::ptr_cast<char*>(&area[0]) + free;
    return true;
  }


}

// STATISTICS: kernel-core
