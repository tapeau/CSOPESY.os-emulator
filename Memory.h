#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

/**
 * @class Memory
 * Represents the memory that the process will be stored* in as it is being executed.
 */
class Memory
{
    private:
    int maximumSize; //total size of memory
    int allocatedSize; //currently allocated size in memory
    std::vector<char> memory; //the memory representation
    std::unordered_map<int, bool> allocationMap; //Tracker if index in memory is taken or not

    public:
        /**
         * @brief Constructor to initialize Memory object.
         * 
         * @param maximumSize //total size of memory
         * @param allocatedSize //current size
         * 
         */
        Memory(int maximumSize, int allocatedSize);

        /**
         * @brief Gets the total size of memory
         * @return The maximum size of memory
         */
        int getMaximumSize() const;

        /**
         * @brief Gets the current allocated size of memory
         * @return The current size of the memory
         */
        int getAllocatedSize() const;

        /**
         * @brief Initialize memory
         */
        void initializeMemory();
        
        /**
         * @brief Checks if memory block is enough
         * false = not enough
         * true = enough
         * @param index Memory block
         * @param size Memory size that will be used
         * @return The state of the memory block
         */
        bool canAllocateAt(int index, int size) const;

        /**
         * @brief Marks the memory block as allocated
         * @param index Memory block
         * @param size Memory size that will be used
         */
        void allocateAt(int index, int size);

         /**
         * @brief Marks the memory block as deallocated
         * @param index Memory block
         */
        void deallocateAt(int index);
};
