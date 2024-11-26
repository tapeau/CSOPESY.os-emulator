#include <algorithm>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <mutex>
#include "Scheduler.h"
#include "Process.h" 
#include "CoreStateManager.h"
#include "Clock.h"
#include "MemoryManager.h"

// Constructor initializes the scheduler's state and member variables.
Scheduler::Scheduler() : is_running(false), active_threads(0), debug_file("debug.txt"), ready_threads(0), cpu_clock(nullptr) {}

// Add a new process to the scheduling queue.
void Scheduler::addProcess(std::shared_ptr<Process> process)
{
  std::unique_lock<std::mutex> lock(queue_mutex); // Lock the queue to ensure thread-safe access.
  process_queue.push_back(process); // Add the process to the queue.
  queue_condition.notify_all(); // Notify a waiting thread that a new process is available.
}

// Set the scheduling algorithm to be used (e.g., Round Robin or FCFS).
void Scheduler::setAlgorithm(const std::string& algorithm)
{
  scheduler_algorithm = algorithm;
}

// Set the number of CPUs available for scheduling and initialize core management.
void Scheduler::setNumCPUs(int num)
{
  cpu_count = num;
  CoreStateManager::getInstance().initialize(cpu_count); // Initialize core states.
}

// Set the delay (in milliseconds) for each command execution.
void Scheduler::setDelays(int delay)
{
  delay_per_execution = delay;
}

// Set the quantum cycle length for Round Robin scheduling.
void Scheduler::setQuantumCycle(int quantum_cycle)
{
  this->quantum_cycle = quantum_cycle;
}

// Set the Clock object to be used as the CPU clock
void Scheduler::setClock(Clock* cpu_clock)
{
  this->cpu_clock = cpu_clock;
}

// Start the scheduler by launching worker threads equal to the number of CPUs.
void Scheduler::start()
{
  is_running = true; // Set the scheduler's running state to true.

  // Create a worker thread for each CPU.
  for (int i = 1; i <= cpu_count; ++i)
  {
    worker_threads.emplace_back(&Scheduler::run, this, i); // Start worker threads.
  }

  // Wait until all worker threads are ready.
  {
    std::unique_lock<std::mutex> lock(start_mutex);
    start_condition.wait(lock, [this] { return ready_threads == cpu_count; }); // Wait until all threads are ready.
  }
}

// Stop the scheduler and join all worker threads.
void Scheduler::stop()
{
  is_running = false; // Set the running state to false to stop threads.
  queue_condition.notify_all(); // Notify all threads to wake up and exit.

  // Join all worker threads to ensure proper shutdown.
  for (auto &thread : worker_threads)
  {
    if (thread.joinable())
    {
      thread.join();
    }
  }
  debug_file.close(); // Close the debug file.
}

// Main function for each worker thread. Runs the selected scheduling algorithm.
void Scheduler::run(int core_id)
{
  {
    std::lock_guard<std::mutex> lock(start_mutex);
    ready_threads++; // Mark this thread as ready.
    if (ready_threads == cpu_count)
    {
      start_condition.notify_all(); // Notify the main thread that all worker threads are ready.
    }
  }

  // Run the appropriate scheduling algorithm based on user selection.
  if (scheduler_algorithm == "rr")
  {
    scheduleRR(core_id); // Run Round Robin scheduling.
  }
  else if (scheduler_algorithm == "fcfs")
  {
    scheduleFCFS(); // Run First-Come, First-Serve scheduling.
  }
}

// FCFS (First-Come, First-Serve) scheduling algorithm.
void Scheduler::scheduleFCFS()
{
  while (is_running) {
    std::shared_ptr<Process> process;
    int assigned_core = -1; // Core to which the process will be assigned.
    {
      std::unique_lock<std::mutex> lock(queue_mutex);
      // Wait until the queue is not empty or the scheduler stops running.
      queue_condition.wait(lock, [this] { return !process_queue.empty() || !is_running; });

      if (!is_running) break; // Exit if the scheduler is stopped.

      process = process_queue.front(); // Get the first process from the queue.
      void* memory = MemoryManager::getInstance().getAllocator()->allocate(process); // memory returns nullptr if it cannot allocate the process in memory
      if (memory != nullptr) { // if process is successfully allocated in memory 
                               // std::cout << "Allocated mem for process " << process->getName() << " (ID: " << process->getPID() << ")\n";
        process_queue.pop_front();   // Remove it from the queue.
      } else {
        // std::cout <<" Insufficient memory for process " << process->getName() << "(ID: " << process->getPID() << ")\n";
        // else statment is when memory is insufficient, therefore we cannot execute the process it is put back at the ready queue
        process_queue.pop_front();
        process_queue.push_back(process);
      }
      // process_queue.pop_front(); // Remove it from the queue.
    }
    if (process->isAllocated()) {
      // Try to find an available core.
      for (int i = 1; i <= cpu_count; ++i)
      {
        if (!CoreStateManager::getInstance().getCoreState(i)) // If the core is not in use.
        {
          assigned_core = i; // Assign this core to the process.
          break;
        }
      }

      // If no core is available, put the process back into the queue.
      if (assigned_core == -1)
      {
        std::unique_lock<std::mutex> lock(queue_mutex);
        process_queue.push_back(process);
        continue; // Skip to the next iteration.
      }

      if (process) {
        {
          std::lock_guard<std::mutex> lock(active_threads_mutex);
          active_threads++; // Increment the active thread count.

          // Ensure the active thread count does not exceed available CPUs.
          if (active_threads > cpu_count)
          {
            std::cerr << "CPU limit exceeded!" << std::endl;
            active_threads--;
            continue;
          }
        }

        logActiveThreads(assigned_core, process); // Log the active threads.
        process->setState(Process::ProcessState::RUNNING); // Set the process state to RUNNING.
        process->setCPUCoreID(assigned_core); // Assign the core to the process.

        // TODO: bug, cpu utilization is not accurate when using PagingAllocator
        CoreStateManager::getInstance().flipCoreState(assigned_core, process->getName()); // Mark the core as in use.

        int last_clock_value = cpu_clock->getClock();
        bool is_first_command_executed = false;
        int cycle_counter = 0;

        // Execute the process until it completes all commands.
        while (process->getCommandCounter() < process->getLinesOfCode()) {
          // testing purposes
          MemoryManager::getInstance().writeMemInfoToFile(100);
          if (delay_per_execution != 0) {
            // Wait for the next CPU cycle
            std::unique_lock<std::mutex> lock(cpu_clock->getMutex());
            cpu_clock->getCondition().wait(lock, [&]
                {
                return cpu_clock->getClock() > last_clock_value;
                });
            last_clock_value = cpu_clock->getClock();
          }
          else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
          }

          // Execute the first command immediately, then apply delay for subsequent commands
          if (!is_first_command_executed || (++cycle_counter >= delay_per_execution)) {
            // have to add lock here bug occurs, execution > maximum instruction if not
            // since CPU implementation does not necessarily hold a certain process as its own
            std::scoped_lock lock{queue_mutex};
            process->setState(Process::ProcessState::RUNNING); // Set the process state to RUNNING.
            if (!CoreStateManager::getInstance().getCoreState(assigned_core)) // If the core is not in use.
            {
              CoreStateManager::getInstance().flipCoreState(assigned_core, process->getName()); // Mark the core as in use.
            }
            process->executeCurrentCommand();
            is_first_command_executed = true;
            cycle_counter = 0; // Reset cycle counter after each execution
          } else {
            process->setState(Process::ProcessState::WAITING); // Set the process state to WAITING.
            if (CoreStateManager::getInstance().getCoreState(assigned_core)) // If the core is in use.
            {
              CoreStateManager::getInstance().flipCoreState(assigned_core, ""); // Mark the core as idle.
            }
          }
        }

        process->setState(Process::ProcessState::FINISHED); // Set the process state to FINISHED.
        MemoryManager::getInstance().getAllocator()->deallocate(process);
        CoreStateManager::getInstance().flipCoreState(assigned_core, ""); // Mark the core as idle.

        std::lock_guard<std::mutex> lock(active_threads_mutex);
        active_threads--; // Decrement the active thread count.

        logActiveThreads(assigned_core, nullptr); // Log the thread state after completion.
        queue_condition.notify_all(); // Notify other threads of availability.
      }
    }
  }
}

// Round Robin (RR) scheduling algorithm.
void Scheduler::scheduleRR(int core_id)
{
  while (is_running)
  {
    std::shared_ptr<Process> process;
    /*
     * Process Allocation
     */
    {
      std::unique_lock<std::mutex> lock(queue_mutex);
      // Wait until the queue is not empty or the scheduler stops running.
      queue_condition.wait(lock, [this] { return !process_queue.empty() || !is_running; });

      if (!is_running) break; // Exit if the scheduler is stopped.

      process = process_queue.front(); // Get the first process from the queue.
      void* memory = MemoryManager::getInstance().getAllocator()->allocate(process); // memory returns nullptr if it cannot allocate the process in memory
      if (memory != nullptr) { // if process is successfully allocated in memory 
                               // std::cout << "Allocated mem for process " << process->getName() << " (ID: " << process->getPID() << ")\n";
        process_queue.pop_front();   // Remove it from the queue.
      } else {
        // std::cout <<" Insufficient memory for process " << process->getName() << "(ID: " << process->getPID() << ")\n";
        // else statment is when memory is insufficient, therefore we cannot execute the process it is put back at the ready queue
          process_queue.pop_front();
          process_queue.push_back(process);
      }
    }
    // std::cout << "Current Process: " << process->getName() << " in_memory: " << process->isAllocated() << "\n";

    if (process->isAllocated())
    {
      {
        std::lock_guard<std::mutex> lock(active_threads_mutex);
        active_threads++; // Increment the active thread count.

        // Ensure the active thread count does not exceed available CPUs.
        if (active_threads > cpu_count)
        {
          std::cerr << "CPU limit exceeded!" << std::endl;
          active_threads--;
          continue;
        }
      }

      logActiveThreads(core_id, process); // Log the active threads.
      process->setState(Process::ProcessState::RUNNING); // Set the process state to RUNNING.
      process->setCPUCoreID(core_id); // Assign the current core to the process.

      // TODO: add some checks in here since cpu is always at maximum for both PagingAllocator
      // and FlatMemoryAllocator
      if (process->getCPUCoreID() == core_id) {
        CoreStateManager::getInstance().flipCoreState(core_id, process->getName()); // Mark the core as in use.
      }

      int last_clock_value = cpu_clock->getClock();
      bool is_first_command_executed = false;
      int cycle_counter = 0;

      // Execute the process for a time slice (quantum).
      int quantum = 0;
      /*
       * Process Execution
       */
      while (!process->hasFinished() && quantum < quantum_cycle) {
        // dumps memory info to file 
        MemoryManager::getInstance().writeMemInfoToFile(quantum);
        if (delay_per_execution != 0) {
          // Wait for the next CPU cycle
          std::unique_lock<std::mutex> lock(cpu_clock->getMutex());
          cpu_clock->getCondition().wait(lock, [&]
              {
              return cpu_clock->getClock() > last_clock_value;
              });
          last_clock_value = cpu_clock->getClock();
        }
        else {
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        // Execute the first command immediately, then apply delay for subsequent commands
        if (!is_first_command_executed || (++cycle_counter >= delay_per_execution)) {
          // added mutex lock since issue arises when multiple cpus are trying to execute
          // a single process
          std::lock_guard<std::mutex> lock(queue_mutex);
          process->setState(Process::ProcessState::RUNNING); // Set the process state to RUNNING.
          if (!CoreStateManager::getInstance().getCoreState(core_id)) // If the core is not in use.
          {
            CoreStateManager::getInstance().flipCoreState(core_id, process->getName()); // Mark the core as in use.
          }
          process->executeCurrentCommand();
          is_first_command_executed = true;
          cycle_counter = 0; // Reset cycle counter after each execution
          ++quantum;        // Increment quantum usage after each command execution
        } else {
          process->setState(Process::ProcessState::WAITING); // Set the process state to WAITING.
          if (CoreStateManager::getInstance().getCoreState(core_id)) // If the core is in use.
          {
            CoreStateManager::getInstance().flipCoreState(core_id, ""); // Mark the core as idle.
          }
        }
      }

      // If the process is not finished, put it back in the queue.
      if (!process->hasFinished())
      {
        process->setState(Process::ProcessState::READY); // Set the process state to READY.
        std::scoped_lock lock{queue_mutex};
        // std::cout << "Items in process_queue: ";
        // for (auto& elem : process_queue) {
        //   std::cout << elem->getName() << "\n";
        // }
        // std::cout << "Items in process_queue: ";
        // push_back to queue if it is not in the queue to prevent duplicates in process_queue (ready queue)
        if ( (std::find(process_queue.begin(), process_queue.end(), process) == process_queue.end()) ) {
          process_queue.push_back(process);
        }
      } else {
        std::scoped_lock lock{queue_mutex};
        process->setState(Process::ProcessState::FINISHED); // Set the process state to FINISHED.
        MemoryManager::getInstance().getAllocator()->deallocate(process);
      }

      std::lock_guard<std::mutex> lock(active_threads_mutex);
      active_threads--; // Decrement the active thread count.

      logActiveThreads(core_id, nullptr); // Log the thread state after completion.
      queue_condition.notify_all(); // Notify other threads of availability.
      CoreStateManager::getInstance().flipCoreState(core_id, ""); // Mark the core as idle.
    } 
  }
}

// Log the current state of active threads and processes to the debug file.
void Scheduler::logActiveThreads(int core_id, std::shared_ptr<Process> current_process)
{
  auto now = std::chrono::system_clock::now(); // Get the current time.
  auto now_c = std::chrono::system_clock::to_time_t(now); // Convert to time_t.
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

  // Write timestamp, core ID, and active threads to the debug file.
  debug_file << "Timestamp: " << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S") 
    << "." << std::setfill('0') << std::setw(3) << ms.count() << " ";
  debug_file << "Core ID: " << core_id << ", Active Threads: " << active_threads << ", ";

  if (current_process)
  {
    // Log the current process details.
    debug_file << "Current Process: " << current_process->getPID() << "(" 
      << current_process->getCommandCounter() << "/" << current_process->getLinesOfCode() << "), ";
  }
  else
  {
    debug_file << "Current Process: None, ";
  }

  // Log the contents of the ready queue.
  debug_file << "Ready Queue: ";
  std::unique_lock<std::mutex> queue_lock(queue_mutex); // Lock the queue mutex.
  std::deque<std::shared_ptr<Process>> temp_queue = process_queue; // Copy the queue.
  queue_lock.unlock(); // Unlock the queue mutex.

  while (!temp_queue.empty())
  {
    debug_file << temp_queue.front()->getName() << " "; // Log each process ID.
    temp_queue.pop_front();
  }
  debug_file << std::endl;
}
