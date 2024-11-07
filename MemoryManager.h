#include <memory>
#include <vector>
#include "IMemoryAllocator.h"
#include "Process.h"
#include "Scheduler.h"

// just made a class that can theoretically handle memory allocation, deallocation, and the snapshot requirement.
class MemoryManager
{
  public:
    static MemoryManager* getInstance();
    static void initialize();
    static void destroy();

    IMemoryAllocator* getAllocator();
    // void freeMemory(int process_id);
    // void allocateMemory(Process &process);
    // void generateSnapshot(int quantum_cycle);
    void initAllocator(size_t size);

  private:
    MemoryManager();
    ~MemoryManager() = default;
    MemoryManager(MemoryManager const&) {};
    MemoryManager& operator=(MemoryManager const&) {return *this;};
    static MemoryManager* sharedInstance;

    IMemoryAllocator* mem_alloc;

    int total_size;
    int frame_size;
    std::vector<int> memory;  // a vector that can represent memory blocks?

    int firstFit();
    int calculateFragmentation();
    int countProcesses();
};
