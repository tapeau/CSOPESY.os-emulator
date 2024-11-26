#pragma once

#include <memory>
#include <string>
#include "Process.h"

// TODO: add backing store implementation for both FlatMemoryAllocator 
// and PagingAllocator

class IMemoryAllocator {
  public:
    virtual void* allocate(std::shared_ptr<Process> process) = 0;
    virtual void deallocate(std::shared_ptr<Process> process) = 0;
    virtual std::string visualizeMemory() = 0;
};
