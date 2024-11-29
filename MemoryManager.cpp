#include <ctime>
#include <fstream>
#include <memory>
#include <string>
#include "MemoryManager.h"
#include "FlatMemoryAllocator.h"
#include "PagingAllocator.h"

void MemoryManager::deleteFileInfo()
{
  // delete existing files from previous program execution
  for (size_t i = 0; i < quantum_cycle; ++i) {
    std::string filePath = FILE_NAME_PREFIX + std::to_string(i) + ".txt";
    std::remove(filePath.c_str());
  }
}

void MemoryManager::initAllocator(size_t max_overall_mem, size_t mem_per_frame)
{
  // FlatMemoryAllocator if mem_per_frame and max_overall_mem is the same
  if (max_overall_mem == mem_per_frame) {
    mem_allocator = std::make_shared<FlatMemoryAllocator>(max_overall_mem);
  } else {
    // PagingAllocator
    mem_allocator = std::make_shared<PagingAllocator>(max_overall_mem, mem_per_frame);
  }
  deleteFileInfo();
}

 uint64_t MemoryManager::getMemUsed() const {
   if (mem_allocator) {
     return mem_allocator->getMem();
   }
    return 0;
}

// dumps memory usage into file for given quantum_cycle
void MemoryManager::writeMemInfoToFile(size_t qq) const
{
  std::string filename = FILE_NAME_PREFIX + std::to_string(qq) + ".txt";
  std::ofstream outfile;

  outfile.open(filename, std::ios_base::app);
  outfile << mem_allocator->visualizeMemory();
  outfile.close();
}

void MemoryManager::setqq(int qq)
{
  quantum_cycle = qq;
}

std::shared_ptr<IMemoryAllocator> MemoryManager::getAllocator()
{
  // std::scoped_lock<std::mutex> lock(mem_mutex);
  return mem_allocator;
}
