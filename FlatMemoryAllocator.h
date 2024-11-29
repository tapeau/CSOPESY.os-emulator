#pragma once
#include <cstddef>
#include <deque>
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
        // std::deque<std::shared_ptr<Process>>& ready_queue
    ~FlatMemoryAllocator();

    void deallocate(std::shared_ptr<Process> process) override;
    std::string visualizeMemory() override;
    void* allocate(std::shared_ptr<Process> process) override;

    void printMemMap() const;

  private:
    size_t max_size;
    size_t alloc_size; // still not sure what this is for? maybe to keep track of used mem space 
    size_t oldest_process_pid;
    std::vector<char> memory;
    std::unordered_map<size_t, bool> alloc_map;
    std::vector<std::shared_ptr<Process>> processes_in_mem;
    std::deque<size_t> processes_pid;
    std::mutex mem_mtx;
    std::mutex bs_mtx;
    std::unordered_map<size_t, std::shared_ptr<Process>> backing_store;
    std::deque<size_t> backing_store_pid;

    void initializeMemory();
    bool canAllocAt(size_t index, size_t size) const; 
    void allocAt(size_t index, size_t size);
    void deallocAt(size_t index);
    int compexternalFrag();
    std::shared_ptr<Process> fromText(std::string& line);
    void storeToBS(std::shared_ptr<Process> proc_to_store);
    void retrieveFromBS();
};
