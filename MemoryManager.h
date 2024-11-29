#include <memory>
#include <vector>
#include <deque>

#include "IMemoryAllocator.h"

/*
 * Singleton Implementation from: 
 * https://stackoverflow.com/questions/1008019/how-do-you-implement-the-singleton-design-pattern
 */

const std::string FILE_NAME_PREFIX = "memory_stamp_";

class MemoryManager
{
  public:
    static MemoryManager& getInstance() {
      static MemoryManager instance;

      return instance;
    }

    static void destroy();
    std::shared_ptr<IMemoryAllocator> getAllocator();
    void initAllocator(size_t max_overall_mem, size_t mem_per_frame);
    // std::deque<std::shared_ptr<Process>>& ready_queue);
    void writeMemInfoToFile(size_t qq) const;
    void setqq(int qq); 

  private:
    MemoryManager() {};
    MemoryManager(MemoryManager const&);  // Don't Implement
    void operator=(MemoryManager const&); // Don't implement
    size_t quantum_cycle;

    std::shared_ptr<IMemoryAllocator> mem_allocator;
    void deleteFileInfo();
};
