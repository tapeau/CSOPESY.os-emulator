#pragma once

#include <cstddef>
#include <map>     
#include <memory>  
#include <vector>  
#include <iostream>
#include <thread>  
#include "Process.h"  
#include "Scheduler.h"
#include "Clock.h"

using namespace std;

// The ProcessManager class manages the lifecycle of multiple processes and communicates with the scheduler.
class ProcessManager
{
  private:
    // Map to store processes, where the key is the process name, and the value is a shared pointer to a Process object.
    map<string, std::shared_ptr<Process>> process_list;

    int process_id_counter = 0;  // Counter to assign unique IDs to new processes.
    Scheduler scheduler;         // Scheduler instance to manage process scheduling.
    thread scheduler_thread;     // Thread to run the scheduler asynchronously.
    int min_instructions;        // Minimum number of instructions a process can generate.
    int max_instructions;        // Maximum number of instructions a process can generate.
    Clock* cpu_clock;            // CPU clock object
    int max_overall_mem; //Overall memory in bytes
    int mem_per_frame; //Size of memory in KB per frame. This is also the memory size per page
    size_t min_mem_per_proc; // min memory per process
    size_t max_mem_per_proc; // max memory per process

  public:
    /**
     * Constructor for ProcessManager.
     * 
     * @param min_instructions Minimum instructions per process.
     * @param max_instructions Maximum instructions per process.
     * @param cpu_count Number of CPUs the scheduler will manage.
     * @param scheduler_algorithm Scheduling algorithm to use (e.g., FCFS, Round-Robin).
     * @param delays_per_execution Number of delays allowed per execution.
     * @param quantum_cycle Time quantum (used for Round-Robin scheduling).
     * @param max_overall_mem Overall memory in bytes.
     * @param mem_per_frame Size of memory per frame/page.
     * @param mem_per_proc Amount of memory per process.
     */
    ProcessManager( int min_instructions, int max_instructions, int cpu_count, 
        std::string scheduler_algorithm, int delays_per_execution, int quantum_cycle, Clock* cpu_clock,
        int max_overall_mem, int mem_per_frame, size_t min_mem_per_proc, size_t max_mem_per_proc );

    /**
     * Adds a new process to the manager and schedules it.
     * 
     * @param process_name The name of the new process.
     * @param creation_time The creation timestamp of the process.
     */
    void addProcess(string process_name, string creation_time);

    /**
     * Retrieves a process by name.
     * 
     * @param process_name The name of the process to retrieve.
     * @return A shared pointer to the Process object, or nullptr if not found.
     */
    shared_ptr<Process> getProcess(string process_name);

    /**
     * Retrieves a map of all processes currently managed by the ProcessManager.
     * 
     * @return A map containing all processes, where the key is the process name.
     */
    map<string, std::shared_ptr<Process>> getAllProcesses();

    /**
     * Destructor for ProcessManager.
     * Ensures the scheduler thread is properly stopped before destruction to avoid undefined behavior.
     */
    ~ProcessManager()
    {
      // If the scheduler thread is still running, stop it before destruction.
      if (scheduler_thread.joinable())
      {
        scheduler.stop();  // Signal the scheduler to stop.
        scheduler_thread.join();  // Wait for the thread to finish.
      }
    }
};
