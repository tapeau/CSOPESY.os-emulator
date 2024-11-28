#pragma once

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <memory>
#include <fstream>
#include <string>
#include "FlatMemoryAllocator.h"
#include "IMemoryAllocator.h"
#include "Clock.h"

// Forward declaration of the Process class.
class Process; 

/**
 * @class Scheduler
 * @brief Manages the scheduling and execution of processes.
 */
class Scheduler
{
  public:
    /**
     * @brief Constructor that initializes the scheduler.
     */
    Scheduler();
    
    static Scheduler& getInstance();
    /**
     * @brief Adds a process to the scheduling queue.
     * @param process Shared pointer to the process to be added.
     */
    void addProcess(std::shared_ptr<Process> process); 

    int getActive();

    /**
     * @brief Sets the scheduling algorithm to be used.
     * @param algorithm Name of the scheduling algorithm.
     */
    void setAlgorithm(const std::string& algorithm); 

    /**
     * @brief Sets the number of available CPUs for scheduling.
     * @param num Number of CPUs.
     */
    void setNumCPUs(int num); 

    /**
     * @brief Configures the delay for each command execution.
     * @param delay Delay per command in milliseconds.
     */
    void setDelays(int delay); 

    /**
     * @brief Configures the quantum cycle for Round Robin scheduling.
     * @param quantum_cycle Time slice for Round Robin in milliseconds.
     */
    void setQuantumCycle(int quantum_cycle); 

    /**
     * @brief Starts the scheduler and initiates process execution.
     */
    void start(); 

    /**
     * @brief Stops the scheduler and terminates all running processes.
     */
    void stop(); 

    /**
     * @brief Sets the CPU clock object.
     */
    void setClock(Clock* cpu_clock); 

  private:
    // Scheduler() = default;
    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(const Scheduler&) = delete; 
    /**
     * @brief Main function executed by each worker thread.
     * @param core_id ID of the CPU core assigned to the thread.
     */
    void run(int core_id); 

    /**
     * @brief Executes processes using First-Come, First-Serve (FCFS) scheduling.
     */
    void scheduleFCFS(); 

    /**
     * @brief Executes processes using Round Robin (RR) scheduling.
     * @param core_id ID of the CPU core assigned to the thread.
     */
    void scheduleRR(int core_id); 

    /**
     * @brief Logs the active threads and their associated processes to a debug file.
     * @param core_id ID of the CPU core assigned to the thread.
     * @param current_process Shared pointer to the currently executing process.
     */
    void logActiveThreads(int core_id, std::shared_ptr<Process> current_process); 

    /*
     * Memory related declaration
     */
    // IMemoryAllocator& mem_alloc;


    // reordered variable declaration according to Constructor to prevent 
    // -Wreorder error when compiling with cc -Wextra flags
    bool is_running; ///< Indicates whether the scheduler is currently running.
    int active_threads; ///< Number of threads currently executing processes.
    std::ofstream debug_file; ///< Output stream for logging debug information.
    int ready_threads; ///< Number of threads ready to execute processes.
    Clock* cpu_clock; ///< CPU clock object to be used.
    int active_ticks = 0;

    int cpu_count; ///< Total number of available CPUs.
    int delay_per_execution; ///< Execution delay per command in milliseconds.
    int quantum_cycle; ///< Time slice for Round Robin scheduling in milliseconds.
    std::string scheduler_algorithm; ///< Name of the selected scheduling algorithm.

    std::deque<std::shared_ptr<Process>> process_queue; ///< Queue holding the processes to be executed.
    std::vector<std::thread> worker_threads; ///< Collection of worker threads.

    std::mutex queue_mutex; ///< Mutex for synchronizing access to the process queue.
    std::mutex active_threads_mutex; ///< Mutex for synchronizing the active thread count.
    std::condition_variable queue_condition; ///< Condition variable for process queue updates.

    std::mutex fcfs_mutex;
    std::mutex start_mutex; ///< Mutex for synchronizing thread startup.
    std::condition_variable start_condition; ///< Condition variable to signal thread startup.
};
