#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "Clock.h"
#include "FlatMemoryAllocator.h"
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <memory>
#include <fstream>
#include <string>
#include <map>
#include <tuple>

// Forward declaration of Process class
class Process;

/**
 * @class Scheduler
 * @brief Handles the scheduling of processes across multiple CPU cores.
 *
 * This class implements various scheduling algorithms such as First Come First Serve (FCFS)
 * and Round Robin (RR) to manage processes and allocate them to CPU cores. It also handles
 * memory logging and state management for processes.
 */
class Scheduler
{
public:
    /**
     * @brief Constructor for Scheduler.
     * @param scheduler_algo The scheduling algorithm to be used (e.g., FCFS, RR).
     * @param delays_per_exec The delay per execution cycle.
     * @param n_cpu Number of CPU cores.
     * @param quantum_cycle Quantum cycle for RR scheduling.
     * @param cpu_clock Pointer to the CPU clock.
     * @param memory_allocator Pointer to the memory allocator.
     */
    Scheduler(std::string scheduler_algo, int delays_per_exec, int n_cpu, int quantum_cycle, Clock* cpu_clock, IMemoryAllocator* memory_allocator);

    /**
     * @brief Adds a new process to the scheduler.
     * @param process Shared pointer to the process to be added.
     */
    void addProcess(std::shared_ptr<Process> process);

    void setAlgorithm(const std::string& algorithm);
    void setNumCPUs(int num);
    void setDelays(int delay);
    void setQuantumCycle(int quantum_cycle);
    void start();
    void stop();
    void setCPUClock(Clock* cpu_clock);

private:
    /**
     * @brief Main run method for the scheduler.
     * @param core_id The core ID where the process will run.
     */
    void run(int core_id);

    /**
     * @brief Implements First Come First Serve (FCFS) scheduling.
     * @param core_id The core ID where the process will run.
     */
    void scheduleFCFS(int core_id);

    /**
     * @brief Implements Round Robin (RR) scheduling.
     * @param core_id The core ID where the process will run.
     */
    void scheduleRR(int core_id);

    /**
     * @brief Logs the current memory state for diagnostics.
     * @param cycle The cycle number for which the memory is being logged.
     */
    void logMemoryState(int cycle);

    /**
     * @brief Starts a memory logging thread that tracks memory state periodically.
     */
    void startMemoryLog();

    bool memory_log_ = false;        ///< Flag to indicate if memory logging is enabled.
    bool is_running;                   ///< Flag to indicate if the scheduler is running.
    int active_threads_;             ///< Number of active worker threads.
    int cpu_count;                      ///< Number of CPU cores.
    int delay_per_execution;             ///< Delay per execution cycle.
    int quantum_cycle;              ///< Quantum cycle for RR scheduling.
    int ready_threads;              ///< Number of threads ready for execution.
    std::string scheduler_algorithm;     ///< The algorithm used for scheduling.
    std::queue<std::shared_ptr<Process>> process_queue_; ///< Queue of processes to be scheduled.
    std::vector<std::thread> worker_threads_; ///< List of worker threads for executing processes.
    std::mutex queue_mutex_;         ///< Mutex for protecting access to the process queue.
    std::mutex active_threads_mutex_;///< Mutex for protecting active thread count.
    std::condition_variable queue_condition_; ///< Condition variable for process queue.
    std::mutex start_mutex_;         ///< Mutex for synchronizing the start of threads.
    std::mutex log_mutex_;           ///< Mutex for synchronizing logging activities.
    std::condition_variable start_condition_; ///< Condition variable to start worker threads.
    Clock* cpu_clock;            ///< Pointer to the CPU clock.
    IMemoryAllocator* memory_allocator_; ///< Pointer to the memory allocator.
    std::thread memory_logging_thread_; ///< Thread for logging memory usage.
};

#endif
