#pragma once

#include <memory>
#include <string>
#include "Process.h"

class IMemoryAllocator {
  public:
    virtual ~IMemoryAllocator() = default;
    virtual void* allocate(std::shared_ptr<Process> process) = 0;
    virtual void deallocate(std::shared_ptr<Process> process) = 0;
    virtual std::string visualizeMemory() = 0;

    virtual size_t getMem() const = 0;
};
