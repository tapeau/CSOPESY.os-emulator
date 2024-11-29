#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include "Process.h"

class IMemoryAllocator {
  public:
    virtual void* allocate(std::shared_ptr<Process> process) = 0;
    virtual void deallocate(std::shared_ptr<Process> process) = 0;
    virtual std::string visualizeMemory() = 0;
    virtual size_t getPageIn() const = 0;
    virtual size_t getPageOut() const = 0;
    virtual size_t getPageTot() const = 0;
    virtual uint64_t getMem() const = 0;
};
