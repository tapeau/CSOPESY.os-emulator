#include "PagingAllocator.h"
#include "Process.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <limits>
#include <cstring>
#include <chrono>
#include <iomanip>
#include <memory>
#include <algorithm>

/**
 * @brief Constructor for PagingAllocator.
 * @param maximum_size The total size of the memory pool.
 * @param mem_per_frame The size of each memory frame.
 */
PagingAllocator::PagingAllocator(size_t maximum_size, size_t mem_per_frame)
    : maximum_size(maximum_size),
      num_frames(static_cast<size_t>(std::ceil(static_cast<double>(maximum_size) / mem_per_frame))),
      mem_per_frame(mem_per_frame),
      n_process(0),
      n_paged_in(0),
      n_paged_out(0)
{
    for (size_t i = 0; i < num_frames; ++i)
    {
        free_frame_list.push_back(i);
    }
}

/**
 * @brief Allocates memory for a process.
 * @param process Shared pointer to the process requesting memory.
 * @return Pointer to the allocated memory frame.
 */
void* PagingAllocator::allocate(std::shared_ptr<Process> process)
{
    std::lock_guard<std::mutex> lock(memory_mutex);
    size_t num_frames_needed = process->getNumPages();
    if (num_frames_needed > free_frame_list.size())
    {
        return nullptr;
    }

    size_t frame_index = allocateFrames(num_frames_needed, process);
    process_list[process->getPID()] = process;
    n_process++;
    return reinterpret_cast<void*>(frame_index);
}

/**
 * @brief Deallocates memory associated with a process.
 * @param process Shared pointer to the process whose memory is to be deallocated.
 */
void PagingAllocator::deallocate(std::shared_ptr<Process> process)
{
    std::lock_guard<std::mutex> lock(memory_mutex);
    process_list.erase(process->getPID());
    n_process--;

    auto it = std::find_if(frame_map.begin(), frame_map.end(),
                           [process](const auto& entry)
                           {
                               return entry.second == process;
                           });

    while (it != frame_map.end())
    {
        size_t frame_index = it->first;
        deallocateFrames(1, frame_index);
        it = std::find_if(frame_map.begin(), frame_map.end(),
                          [process](const auto& entry)
                          {
                              return entry.second == process;
                          });
    }

    size_t total_allocated_frames = frame_map.size();
    size_t total_free_frames = free_frame_list.size();

    if (total_allocated_frames + total_free_frames != num_frames)
    {
        size_t missing_frames = num_frames - (total_allocated_frames + total_free_frames);
        if (missing_frames > 0)
        {
            for (size_t i = 0; i < missing_frames; ++i)
            {
                free_frame_list.push_back(num_frames + i);
            }
        }
    }
}

/**
 * @brief Visualizes the current memory allocation state.
 */
void PagingAllocator::visualizeMemory()
{
    std::cout << "Memory Visualization:\n";

    for (size_t frame_index = 0; frame_index < num_frames; ++frame_index)
    {
        auto it = frame_map.find(frame_index);

        if (it != frame_map.end())
        {
            std::cout << "Frame " << frame_index << " -> Process " << it->second->getPID() << "\n";
        }
        else
        {
            std::cout << "Frame " << frame_index << " -> Free\n";
        }
    }
    std::cout << "---- End of memory visualization ----\n";
}

/**
 * @brief Get the number of processes currently in memory.
 * @return The number of processes currently allocated in memory.
 */
int PagingAllocator::getNProcess()
{
    std::lock_guard<std::mutex> lock(memory_mutex);
    return n_process;
}

/**
 * @brief Get a list of all processes currently allocated in memory.
 * @return A map of starting memory indices to process pointers.
 */
std::map<size_t, std::shared_ptr<Process>> PagingAllocator::getProcessList()
{
    std::lock_guard<std::mutex> lock(memory_mutex);
    return process_list;
}

/**
 * @brief Get the maximum memory capacity managed by the allocator.
 * @return The maximum memory size.
 */
size_t PagingAllocator::getMaxMemory()
{
    std::lock_guard<std::mutex> lock(memory_mutex);
    return maximum_size;
}

/**
 * @brief Get the amount of external fragmentation in memory.
 * @return The size of external fragmentation.
 */
size_t PagingAllocator::getExternalFragmentation()
{
    std::lock_guard<std::mutex> lock(memory_mutex);
    return free_frame_list.size() * mem_per_frame;
}

/**
 * @brief Deallocate the oldest process in memory.
 * @param mem_size The size of memory to free.
 */
void PagingAllocator::deallocateOldest(size_t mem_size)
{
    std::chrono::time_point<std::chrono::system_clock> oldest_time = std::chrono::time_point<std::chrono::system_clock>::max();
    size_t oldest_index = 0;
    std::shared_ptr<Process> oldest_process = nullptr;

    for (const auto& pair : process_list)
    {
        size_t index = pair.first;
        std::shared_ptr<Process> process = pair.second;

        auto alloc_time = process->getAllocTime();
        if (alloc_time < oldest_time)
        {
            oldest_time = alloc_time;
            oldest_index = index;
            oldest_process = process;
        }
    }

    if (oldest_process)
    {
        while (oldest_process->getState() == Process::ProcessState::RUNNING)
        {
            // Wait until the process is no longer running.
        }

        std::ofstream backing_store("backingstore.txt", std::ios::app);

        if (backing_store.is_open())
        {
            std::time_t alloc_time_t = std::chrono::system_clock::to_time_t(oldest_process->getAllocTime());
            std::tm alloc_time_tm;

            if (localtime_s(&alloc_time_tm, &alloc_time_t) == 0)
            {
                backing_store << "Process ID: " << oldest_process->getPID();
                backing_store << "  Name: " << oldest_process->getName();
                backing_store << "  Command Counter: " << oldest_process->getCommandCounter()
                              << "/" << oldest_process->getLinesOfCode() << "\n";
                backing_store << "Memory Size: " << oldest_process->getMemoryRequired() << " KB\n";
                backing_store << "Num Pages: " << oldest_process->getNumPages() << "\n";
                backing_store << "============================================================================\n";

                backing_store.close();
            }
            else
            {
                std::cerr << "Failed to convert time to local time format." << std::endl;
            }
        }

        if (oldest_process->getState() != Process::ProcessState::FINISHED)
        {
            deallocate(oldest_process);
            oldest_process->setMemory(nullptr);
        }
    }
}

/**
 * @brief Allocate frames for a given process.
 * @param num_frames Number of frames to allocate.
 * @param process Shared pointer to the process requesting frames.
 * @return The index of the first allocated frame.
 */
size_t PagingAllocator::allocateFrames(size_t num_frames, std::shared_ptr<Process> process)
{
    size_t frame_index = free_frame_list.back();
    for (size_t i = 0; i < num_frames; ++i)
    {
        free_frame_list.pop_back();
        frame_map[frame_index + i] = process;
        n_paged_in++;
    }
    return frame_index;
}

/**
 * @brief Deallocate frames starting at a given index.
 * @param num_frames Number of frames to deallocate.
 * @param frame_index Starting frame index.
 */
void PagingAllocator::deallocateFrames(size_t num_frames, size_t frame_index)
{
    for (size_t i = 0; i < num_frames; ++i)
    {
        frame_map.erase(frame_index + i);
        n_paged_out++;
    }

    for (size_t i = 0; i < num_frames; ++i)
    {
        free_frame_list.push_back(frame_index + i);
    }
}

/**
 * @brief Get the number of page-ins that have occurred.
 * @return The number of page-ins.
 */
size_t PagingAllocator::getPageIn()
{
    std::lock_guard<std::mutex> lock(memory_mutex);
    return n_paged_in;
}

/**
 * @brief Get the number of page-outs that have occurred.
 * @return The number of page-outs.
 */
size_t PagingAllocator::getPageOut()
{
    std::lock_guard<std::mutex> lock(memory_mutex);
    return n_paged_out;
}
