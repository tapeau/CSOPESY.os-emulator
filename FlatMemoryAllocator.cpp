#include "FlatMemoryAllocator.h"
#include <string>

FlatMemoryAllocator::FlatMemoryAllocator(size_t max_size) : max_size(max_size), alloc_size(0) {
  memory.reserve(max_size);
  initializeMemory();
}

bool FlatMemoryAllocator::canAllocAt(size_t index, size_t size) const
{
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
  std::fill(memory.begin(), memory.end(), '.'); // '.' represents unallocated memory
  // initialize all existing elements in alloc_map to false
  for (auto& elem : alloc_map) {
    elem.second = false;
  }
}


void* FlatMemoryAllocator::allocate(size_t size)
{
  for (size_t i = 0; i < max_size - size + 1; ++i) {
    if (!alloc_map[i] && canAllocAt(i, size)) {
      allocAt(i, size);
      return &memory[i];
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

void FlatMemoryAllocator::deallocate(void* ptr)
{
  size_t index = static_cast<char*>(ptr) - &memory[0];
  if (alloc_map[index]) {
    deallocAt(index);
  }

}

std::string FlatMemoryAllocator::visualizeMemory() {
  return std::string(memory.begin(), memory.end());
}
