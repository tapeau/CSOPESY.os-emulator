#ifndef PAGING_ALLOCATOR_H
#define PAGING_ALLOCATOR_H

#include "IMemoryAllocator.h"
#include <vector>
#include <iostream>
#include <mutex>
#include <map>

/**
 * @class PagingAllocator
 * @brief Implements a paging memory allocation strategy.
 *
 * This class manages memory allocation using a paging system, dividing memory into fixed-size frames.
 */
class PagingAllocator : public IMemoryAllocator
{
public:
    /**
     * @brief Constructor for PagingAllocator.
     * @param maximum_size The total size of the memory pool.
     * @param mem_per_frame The size of each memory frame.
     */
    PagingAllocator(size_t maximum_size, size_t mem_per_frame);

    void* allocate(std::shared_ptr<Process> process) override;
    void deallocate(std::shared_ptr<Process> process) override;
    void visualizeMemory() override;
    int getNProcess() override;
    std::map<size_t, std::shared_ptr<Process>> getProcessList() override;
    size_t getMaxMemory() override;
    size_t getExternalFragmentation() override;
    void deallocateOldest(size_t mem_size) override;
    size_t getPageIn() override;
    size_t getPageOut() override;

private:
    size_t maximum_size;          ///< Total size of the memory pool.
    size_t num_frames;            ///< Total number of frames.
    std::unordered_map<size_t, std::shared_ptr<Process>> frame_map; ///< Mapping from frame indices to processes.
    std::vector<size_t> free_frame_list; ///< List of free frames.
    size_t n_paged_in;            ///< Number of times a page has been paged in.
    size_t n_paged_out;           ///< Number of times a page has been paged out.

    size_t mem_per_frame;         ///< Memory per frame.
    size_t allocated_size;        ///< Currently allocated memory size.
    int n_process;                ///< Number of processes.

    std::mutex memory_mutex;      ///< Mutex for thread-safe memory operations.
    std::map<size_t, std::shared_ptr<Process>> process_list; ///< Map of process list with starting memory index.

    /**
     * @brief Allocate a specified number of frames to a process.
     * @param num_frames Number of frames to allocate.
     * @param process Shared pointer to the process requesting frames.
     * @return The index of the first allocated frame.
     */
    size_t allocateFrames(size_t num_frames, std::shared_ptr<Process> process);

    /**
     * @brief Deallocate a specified number of frames starting at a frame index.
     * @param num_frames Number of frames to deallocate.
     * @param frame_index Starting frame index.
     */
    void deallocateFrames(size_t num_frames, size_t frame_index);
};

#endif
