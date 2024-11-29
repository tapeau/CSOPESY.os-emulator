#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include "Process.h"
#include "Scheduler.h"
#include "Clock.h"
#include "FlatMemoryAllocator.h"
#include "PagingAllocator.h"

#include <map>
#include <memory>
#include <vector>
#include <iostream>
#include <thread>
#include <mutex>

/**
 * @class ProcessManager
 * @brief Manages all processes, including creation, memory allocation, scheduling, and statistics collection.
 *
 * This class handles the lifecycle of processes, including adding processes, memory allocation, and 
 * assigning processes to the scheduler for execution. It also provides system monitoring functionality.
 */
class ProcessManager
{
private:
    std::map<std::string, std::shared_ptr<Process>> process_list_; ///< Map of process names to shared pointers of Process.
    int pid_counter_ = 0;                                          ///< Counter for process IDs.
    Scheduler* scheduler_;                                         ///< Scheduler instance.
    std::thread scheduler_thread_;                                 ///< Thread to run the scheduler.
    int min_ins_;                                                  ///< Minimum number of instructions per process.
    int max_ins_;                                                  ///< Maximum number of instructions per process.
    Clock* cpu_clock;                                          ///< Pointer to the CPU clock.
    size_t min_mem_per_proc_;                                      ///< Minimum memory per process.
    size_t max_mem_per_proc_;                                      ///< Maximum memory per process.
    size_t max_mem_;                                               ///< Maximum total memory.
    size_t mem_per_frame_;                                         ///< Memory per frame.
    IMemoryAllocator* memory_allocator_;                           ///< Pointer to memory allocator.
    int num_cpu_;                                                  ///< Number of CPU cores.
    std::mutex process_list_mutex_;                                ///< Mutex for protecting access to process list.
    std::mutex core_states_mutex_;                                 ///< Mutex for protecting core state operations.

    /**
     * @brief Generate the memory required for a new process.
     * @return The generated memory size for the process.
     */
    size_t generateMemory();

public:
    /**
     * @brief Constructor for ProcessManager.
     * @param min_ins Minimum instructions per process.
     * @param max_ins Maximum instructions per process.
     * @param n_cpu Number of CPU cores.
     * @param scheduler_algo Scheduler algorithm to use.
     * @param delays_per_exec Number of delays per execution.
     * @param quantum_cycle Quantum cycles for round-robin scheduling.
     * @param cpu_clock Pointer to CPU clock.
     * @param max_mem Maximum total memory available.
     * @param mem_per_frame Memory per frame for paging.
     * @param min_mem_per_proc Minimum memory per process.
     * @param max_mem_per_proc Maximum memory per process.
     */
    ProcessManager(int min_ins, int max_ins, int n_cpu, std::string scheduler_algo, int delays_per_exec,
                   int quantum_cycle, Clock* cpu_clock, size_t max_mem, size_t mem_per_frame, 
                   size_t min_mem_per_proc, size_t max_mem_per_proc);

    /**
     * @brief Adds a new process to the system.
     * @param name Name of the process.
     * @param time Time of creation.
     */
    void addProcess(std::string name, std::string time);

    /**
     * @brief Retrieves a process by its name.
     * @param name The name of the process.
     * @return Shared pointer to the requested process.
     */
    std::shared_ptr<Process> getProcess(std::string name);

    /**
     * @brief Retrieves all processes in the system.
     * @return Map of process names to shared pointers of processes.
     */
    std::map<std::string, std::shared_ptr<Process>> getAllProcess();

    /**
     * @brief Destructor for ProcessManager.
     * Ensures that the scheduler thread is stopped before destruction.
     */
    ~ProcessManager()
    {
        if (scheduler_thread_.joinable())
        {
            scheduler_->stop();
            scheduler_thread_.join();
        }
    }

    /**
     * @brief Prints system memory and process information statistics.
     */
    void processSmi();

    /**
     * @brief Prints system memory statistics in a formatted style.
     */
    void vmStat();
};

#endif
