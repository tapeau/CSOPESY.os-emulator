#include <ctime>
#include <memory>
#include "MemoryManager.h"
#include "FlatMemoryAllocator.h"
#include "IMemoryAllocator.h"

// MemoryManager* MemoryManager::sharedInstance = nullptr;

// void MemoryManager::allocateMemory(Process &process)
// {
//   // probably a method we need, but i'm still kinda lost on this.
// }

void MemoryManager::initAllocator(size_t size)
{
  mem_allocator = std::make_shared<FlatMemoryAllocator>(size);
}


std::shared_ptr<IMemoryAllocator> MemoryManager::getAllocator()
{
  return mem_allocator;
}

// IMemoryAllocator* MemoryManager::getAllocator()
// {
//   return mem_alloc;
// }

// int MemoryManager::firstFit()
// {
//   // like a method that would find avaialble block of frames so that they could be fitted?
// }

// i think this is correct?? i just used the logic that sir explained in the video
// int MemoryManager::calculateFragmentation()
// {
//   int free_space = 0;
//   int fragmented_space = 0;
//   bool fragmented = false;
//
//   for (int frame : memory)
//   {
//     if (frame == -1)
//     {
//       free_space += frame_size;
//       fragmented = true;
//     }
//     else if (fragmented)
//     {
//       fragmented_space += free_space;
//       free_space = 0;
//       fragmented = false;
//     }
//   }
//   return fragmented_space;
// }

// void MemoryManager::freeMemory(int process_id)
// {
//   for (int &frame : memory)
//   {
//     if (frame == process_id)
//     {
//       frame = -1;
//     }
//   }
// }

// int MemoryManager::countProcesses()
// {
//   // for the number of processes in memory. still thinking of ways to do this
// }
//
// // just copied some stuff from other files to create this
// void MemoryManager::generateSnapshot(int quantum_cycle)
// {
//   std::ofstream file("memory_stamp_" + std::to_string(quantum_cycle) + ".txt");
//
//   std::time_t now = std::time(nullptr);
//
//   file << "Timestamp: (" << std::put_time(std::localtime(&now), "%m/%d/%Y %H:%M:%S") << ")\n";
//   file << "Number of processes in memory: " << countProcesses() 
//     << "\n";
//   file << "Total external fragmentation in KB: " << calculateFragmentation() 
//     << "\n";
//   file << "----end----" << total_size
//     << "\n";
//
//   // The ASCII printout
//
//   file << "----start---- = 0\n"; // the 0 here is just a placeholder
//   file.close(); 
// }
