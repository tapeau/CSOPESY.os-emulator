#pragma once
#include <cstddef>
#include <unordered_map>
#include <vector>
#include "IMemoryAllocator.h"

class FlatMemoryAllocator : public IMemoryAllocator {
  public:
    FlatMemoryAllocator(size_t max_size);

    void deallocate(void *ptr) override;
    std::string visualizeMemory() override;
    void* allocate(size_t size) override;

  private:
    size_t max_size;
    size_t alloc_size;
    std::vector<char> memory;
    std::unordered_map<size_t, bool> alloc_map;

    void initializeMemory();
    bool canAllocAt(size_t index, size_t size) const; 
    void allocAt(size_t index, size_t size);
    void deallocAt(size_t index);
};
