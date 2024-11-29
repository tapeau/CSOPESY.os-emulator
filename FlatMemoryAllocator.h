#pragma once
#include <cstddef>
#include <memory>
#include <mutex>
#include <set>
#include <tuple>
#include <unordered_map>
#include <vector>
#include "IMemoryAllocator.h"

class FlatMemoryAllocator : public IMemoryAllocator {
  public:
    FlatMemoryAllocator(size_t max_size);
    ~FlatMemoryAllocator();

    void deallocate(std::shared_ptr<Process> process) override;
    void* allocate(std::shared_ptr<Process> process) override;
    uint64_t getMem() const override {
        return alloc_size;
    }

    size_t getPageIn() const override;
    size_t getPageOut() const override;
    size_t getPageTot() const override;
    void printMemMap() const;

    std::string visualizeMemory() override;
  private:
    size_t max_size;
    uint64_t alloc_size; // still not sure what this is for? maybe to keep track of used mem space 
    std::vector<char> memory;
    std::unordered_map<size_t, bool> alloc_map;
    std::vector<std::shared_ptr<Process>> processes_in_mem;
    std::mutex mem_mtx;

    void initializeMemory();
    bool canAllocAt(size_t index, size_t size) const; 
    void allocAt(size_t index, size_t size);
    void deallocAt(size_t index);
    int compexternalFrag();
};
