#include <memory>
#include <vector>
#include "IMemoryAllocator.h"
#include "Scheduler.h"

// just made a class that can theoretically handle memory allocation, deallocation, and the snapshot requirement.
/*
 * Singleton Implementation from: 
 * https://stackoverflow.com/questions/1008019/how-do-you-implement-the-singleton-design-pattern
 */

class MemoryManager
{
  public:
    static MemoryManager& getInstance()
    {
      static MemoryManager instance;

      return instance;
    }
    static void destroy();
    std::shared_ptr<IMemoryAllocator> getAllocator();
    // void freeMemory(int process_id);
    // void allocateMemory(Process &process);
    // void generateSnapshot(int quantum_cycle);
    void initAllocator(size_t size);
    // void printMemory() const;

  private:
    MemoryManager() {};
    MemoryManager(MemoryManager const&);  // Don't Implement
    void operator=(MemoryManager const&); // Don't implement

    // IMemoryAllocator* mem_alloc;
    std::shared_ptr<IMemoryAllocator> mem_allocator;
};
