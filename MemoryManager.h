#include <vector>
#include <Process.h>
#include "Scheduler.h"

// just made a class that can theoretically handle memory allocation, deallocation, and the snapshot requirement.
class MemoryManager
{
public:
    MemoryManager(int total_size, int frame_size);
    void freeMemory(int process_id);
    void allocateMemory(Process &process);
    void generateSnapshot(int quantum_cycle);

private:
    int total_size;
    int frame_size;
    std::vector<int> memory;  // a vector that can represent memory blocks?

    int firstFit();
    int calculateFragmentation();
    int countProcesses();
};
