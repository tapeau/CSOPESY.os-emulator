#include <cstdlib>
#include "Memory.h"

// Constructor implementation.
Memory::Memory(int maximumSize, int allocatedSize)
    : maximumSize(maximumSize), allocatedSize(0)//
{}

//Return maximum size of the memory
int Memory::getMaximumSize() const
{
    return maximumSize;
}

//Return the allocated size of memory
int Memory::getAllocatedSize() const
{
    return allocatedSize;
}

//Initialize the memory
void Memory::initializeMemory()
{
    // std::fill(memory.begin(), memory.end(), '.'); //'.' unallocated memory
    // std::fill(allocationMap.begin(), allocationMap.end(), false); //false all cuz free space

    for (size_t i = 0; i < memory.size(); i++) {
        memory[i] = '.'; //unallocated memory
    }

    for (size_t i = 0; i < allocationMap.size(); i++) { //im not sure with the allocationMap.size()
        allocationMap[i] = false; //false all cuz free space
    }
}

//Return the state of the memory block
bool Memory::canAllocateAt(int index, int size) const
{
    return (index + size <= maximumSize);
}

//Allocates the process to the memory
void Memory::allocateAt(int index, int size)
{
    // std::fill(allocationMap.begin() + index, allocationMap.begin() + index + size, true);
    for (int i = index; i < index + size; ++i) {
        allocationMap[i] = true;
    }
    allocatedSize += size;
}

//Deallocates the process from the memory
void Memory::deallocateAt(int index)
{
    //idk if we remove and decrement size
    allocationMap[index] = false;
}