#include "FlatMemoryAllocator.h"
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>

FlatMemoryAllocator::FlatMemoryAllocator(size_t max_size) : max_size(max_size), alloc_size(0) {
  memory.reserve(max_size);
  initializeMemory();
  // std::cout << "Memory is now initialized with max_size: " << max_size << "\n";
}

FlatMemoryAllocator::~FlatMemoryAllocator()
{
  memory.clear();
}

bool FlatMemoryAllocator::canAllocAt(size_t index, size_t size) const
{
  // check bounds to ensure index + size cannot exceed max_size
  return (index + size <= max_size);
}


void FlatMemoryAllocator::allocAt(size_t index, size_t size)
{
  // std::fill(alloc_map.begin() + index, alloc_map.begin() + index + size, true);
  for (int i = index; i < index + size; ++i) {
    alloc_map[i] = true;
  }
  alloc_size += size;
}

void FlatMemoryAllocator::initializeMemory()
{
  // std::fill(memory.begin(), memory.end(), '.'); // '.' represents unallocated memory
  // initialize all existing elements in alloc_map to false
  for (auto& elem : alloc_map) {
    elem.second = false;
  }
}

void* FlatMemoryAllocator::allocate(std::shared_ptr<Process> process)
{
  std::scoped_lock<std::mutex> lock(mem_mtx);
  size_t size = process->getMemReq();
  for (size_t i = 0; i < max_size - size + 1; ++i) {
    if (!alloc_map[i] && canAllocAt(i, size)) {
      allocAt(i, size);
      // DEBUGGING PURPOSES:
      // std::cout << process->getName() << ", successfully allocated at " << std::to_string(i) << "~" + std::to_string(i+size) << "\n"; 
      process->MemAlloc(i, i + size);
      auto it = std::lower_bound(processes_in_mem.begin(), processes_in_mem.end(), process,
                                 [](const std::shared_ptr<Process>& p1, const std::shared_ptr<Process>& p2) {
                                     return p1->getStartLoc() < p2->getStartLoc();
                                 });
      processes_in_mem.insert(it, process);    
      return &processes_in_mem.back();
    }
  }
  return nullptr;
}

/*
 * deallocAt given index or sets unordered_map[index] value to false
 */
void FlatMemoryAllocator::deallocAt(size_t index)
{
  // mark mem as deallocated
  alloc_map[index] = false;
}

void FlatMemoryAllocator::deallocate(std::shared_ptr<Process> process)
{
  // std::cout << "Before Deallocation: \n";
  // std::cout << visualizeMemory();
  std::scoped_lock<std::mutex> lock(mem_mtx);
  // finds the process if it exists in process_in_mem
  auto it = std::find(processes_in_mem.begin(), processes_in_mem.end(), process);
  if (it != processes_in_mem.end())
  {
    processes_in_mem.erase(it);
  }
  size_t index = process->getStartLoc();
  for (size_t i = 0 ; i < process->getMemReq(); ++i) {
    if (alloc_map[index]) {
      deallocAt(index);
      index += 1;
    }
  }
  process->MemDealloc();
  // std::cout << "After Deallocation: \n";
  // std::cout << visualizeMemory();
}

int FlatMemoryAllocator::compexternalFrag()
{
  size_t low = 0;
  size_t up = 0;
  size_t extfrag = 0;

  if (processes_in_mem.empty()){
    return 0;
  }

  for (int i = processes_in_mem.size() - 1; i >= 0; --i) {
      // std::shared_ptr<Process> curr_process = processes_in_mem[i];
      low = processes_in_mem[i]->getStartLoc();
      up = processes_in_mem[i-1]->getEndLoc();

      if (low < up){
        extfrag += (low - up);
      }
  }

  return extfrag;
}

std::string FlatMemoryAllocator::visualizeMemory() {
  // printMemMap();
  auto now = std::chrono::system_clock::now(); // Get the current time.
  auto now_c = std::chrono::system_clock::to_time_t(now); // Convert to time_t.
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

  std::stringstream str_stream;
  str_stream << "\n";
  // str_stream << "Timestamp: " << "(MM/DD/YYYY HH:MM:YY)\n";
  str_stream << "Timestamp: " << std::put_time(std::localtime(&now_c), "%m-%d-%Y %H:%M:%S %p") << "\n";
  str_stream << "Number of process in memory: " << processes_in_mem.size() << "\n";
  str_stream << "Total external fragmentation in KB: " << compexternalFrag() << "\n"; // << add some calculation for fragmentation here
  str_stream << "----end---- = " << max_size << "\n";
  for (int i = processes_in_mem.size() - 1; i >= 0; --i) {
      std::shared_ptr<Process> curr_process = processes_in_mem[i];
      str_stream << "\n" << curr_process->getEndLoc()
        << "\n" << curr_process->getName() 
        << "\n" << curr_process->getStartLoc()
        << "\n";
  }
  str_stream << "----start---- = " << 0 << "\n"; 
  str_stream << "\n";

  // str_stream << "----end---- = " << max_size << "\n";
  // str_stream << "\n";
  // for (auto& elem : alloc_map) {
  //   str_stream << "Mem: " << elem.first << " allocated: " << elem.second << "\n";
  // }
  // str_stream << "----start---- = " << 0 << "\n"; 
  // str_stream << "\n";

  return std::string(str_stream.str());
}

void FlatMemoryAllocator::printMemMap() const
{
  for (auto& elem : alloc_map) {
    std::cout << "First: " << elem.first << " Second: " << elem.second << std::endl;
  }
}
