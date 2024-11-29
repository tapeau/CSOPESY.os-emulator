#include "Scheduler.h"
#include "Process.h"
#include "CoreStateManager.h"
#include "Clock.h"
#include "FlatMemoryAllocator.h"

#include <iostream>
#include <chrono>
#include <iomanip>

/**
 * @brief Constructor for Scheduler.
 */
Scheduler::Scheduler(std::string scheduler_algo, int delays_per_exec, int n_cpu, int quantum_cycle, Clock* cpu_clock, IMemoryAllocator* memory_allocator)
    : is_running(false), active_threads_(0), ready_threads(0), scheduler_algorithm(scheduler_algo), delay_per_execution(delays_per_exec),
      cpu_count(n_cpu), quantum_cycle(quantum_cycle), cpu_clock(cpu_clock), memory_allocator_(memory_allocator)
{
}

/**
 * @brief Adds a process to the scheduling queue.
 * @param process Shared pointer to the process.
 */
void Scheduler::addProcess(std::shared_ptr<Process> process)
{
    if (!memory_log_)
    {
        startMemoryLog();
    }

    std::unique_lock<std::mutex> lock(queue_mutex_);
    process_queue_.push(process);
    queue_condition_.notify_one();
}

void Scheduler::setAlgorithm(const std::string& algorithm)
{
    scheduler_algorithm = algorithm;
}

void Scheduler::setNumCPUs(int num)
{
    cpu_count = num;
    CoreStateManager::getInstance().initialize(cpu_count);
}

void Scheduler::setDelays(int delay)
{
    delay_per_execution = delay;
}

void Scheduler::setCPUClock(Clock* clock)
{
    cpu_clock = clock;
}

void Scheduler::setQuantumCycle(int quantum_cycle)
{
    quantum_cycle = quantum_cycle;
}

/**
 * @brief Starts the scheduler and creates worker threads for each core.
 */
void Scheduler::start()
{
    is_running = true;
    for (int i = 1; i <= cpu_count; ++i)
    {
        worker_threads_.emplace_back(&Scheduler::run, this, i);
    }

    {
        std::unique_lock<std::mutex> lock(start_mutex_);
        start_condition_.wait(lock, [this]
        {
            return ready_threads == cpu_count;
        });
    }
}

/**
 * @brief Starts a thread to log memory usage periodically.
 */
void Scheduler::startMemoryLog()
{
    memory_log_ = true;
    memory_logging_thread_ = std::thread([this]()
    {
        std::unique_lock<std::mutex> lock(cpu_clock->getMutex());

        while (is_running)
        {
            // Wait for CPU clock tick increment
            cpu_clock->getCondition().wait(lock);

            bool any_core_active = false;

            // Check if any core is active
            for (int i = 1; i <= cpu_count; ++i)
            {
                if (CoreStateManager::getInstance().getCoreState(i))
                {
                    any_core_active = true;
                    break;
                }
            }

            // Increment active CPU count if at least one core is active
            if (any_core_active)
            {
                cpu_clock->incrementActiveCpuNum();
            }
        }
    });
}

/**
 * @brief Stops the scheduler and joins all worker threads.
 */
void Scheduler::stop()
{
    is_running = false;
    queue_condition_.notify_all();

    if (memory_logging_thread_.joinable())
    {
        memory_logging_thread_.join();
    }

    for (auto& thread : worker_threads_)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
}

/**
 * @brief Main run loop for each worker thread.
 * @param core_id The core ID where the process will run.
 */
void Scheduler::run(int core_id)
{
    {
        std::lock_guard<std::mutex> lock(start_mutex_);
        ready_threads++;
        if (ready_threads == cpu_count)
        {
            start_condition_.notify_one();
        }
    }

    if (scheduler_algorithm == "rr")
    {
        scheduleRR(core_id);
    }
    else if (scheduler_algorithm == "fcfs")
    {
        scheduleFCFS(core_id);
    }
}

/**
 * @brief Implements First Come First Serve (FCFS) scheduling.
 * @param core_id The core ID where the process will run.
 */
void Scheduler::scheduleFCFS(int core_id)
{
    while (is_running)
    {
        std::shared_ptr<Process> process;

        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            queue_condition_.wait(lock, [this]
            {
                return !process_queue_.empty() || !is_running;
            });

            if (!is_running)
            {
                break;
            }

            process = process_queue_.front();
            process_queue_.pop();
        }

        if (process)
        {
            {
                std::lock_guard<std::mutex> lock(active_threads_mutex_);
                active_threads_++;
                if (active_threads_ > cpu_count)
                {
                    std::cerr << "Error: Exceeded CPU limit!" << std::endl;
                    active_threads_--;
                    continue;
                }
            }

            // Allocate memory for the process
            void* memory = memory_allocator_->allocate(process);

            // If allocation was successful, set allocation time and memory pointer
            if (memory)
            {
                process->setAllocTime();
                process->setMemory(memory);
            }

            // If no free memory, deallocate the oldest and allocate again
            if (!memory)
            {
                do
                {
                    memory_allocator_->deallocateOldest(process->getMemoryRequired());
                    memory = memory_allocator_->allocate(process);
                    if (memory)
                    {
                        process->setAllocTime();
                        process->setMemory(memory);
                    }
                }
                while (!memory);
            }

            process->setState(Process::ProcessState::RUNNING);
            process->setCPUCoreID(core_id);
            CoreStateManager::getInstance().setCoreState(core_id, true, process->getName());

            int last_clock = cpu_clock->getCpuClock();
            bool first_command_executed = false;
            int cycle_counter = 0;

            while (process->getCommandCounter() < process->getLinesOfCode())
            {
                {
                    // Wait for the next CPU cycle
                    std::unique_lock<std::mutex> lock(cpu_clock->getMutex());
                    cpu_clock->getCondition().wait(lock, [&]
                    {
                        return cpu_clock->getCpuClock() > last_clock;
                    });
                    last_clock = cpu_clock->getCpuClock();
                }

                if (!first_command_executed || (++cycle_counter >= delay_per_execution))
                {
                    process->executeCurrentCommand();
                    first_command_executed = true;
                    cycle_counter = 0;
                }
            }

            process->setState(Process::ProcessState::FINISHED);

            {
                memory_allocator_->deallocate(process);
                std::lock_guard<std::mutex> lock(active_threads_mutex_);
                active_threads_--;
            }

            queue_condition_.notify_one();
        }

        CoreStateManager::getInstance().setCoreState(core_id, false, "");
    }
}

/**
 * @brief Implements Round Robin (RR) scheduling.
 * @param core_id The core ID where the process will run.
 */
void Scheduler::scheduleRR(int core_id)
{
    while (is_running)
    {
        std::shared_ptr<Process> process;

        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            queue_condition_.wait(lock, [this]
            {
                return !process_queue_.empty() || !is_running;
            });

            if (!is_running)
            {
                break;
            }

            process = process_queue_.front();
            process_queue_.pop();
        }

        if (process)
        {
            {
                std::lock_guard<std::mutex> lock(active_threads_mutex_);
                active_threads_++;
                if (active_threads_ > cpu_count)
                {
                    std::cerr << "Error: Exceeded CPU limit!" << std::endl;
                    active_threads_--;
                    continue;
                }
            }

            void* memory = process->getMemory();

            if (!memory)
            {
                memory = memory_allocator_->allocate(process);

                if (memory)
                {
                    process->setAllocTime();
                    process->setMemory(memory);
                }

                if (!memory)
                {
                    do
                    {
                        memory_allocator_->deallocateOldest(process->getMemoryRequired());
                        memory = memory_allocator_->allocate(process);
                        if (memory)
                        {
                            process->setAllocTime();
                            process->setMemory(memory);
                        }
                    }
                    while (!memory);
                }
            }

            process->setState(Process::ProcessState::RUNNING);
            process->setCPUCoreID(core_id);
            CoreStateManager::getInstance().setCoreState(core_id, true, process->getName());

            int quantum = 0;
            int last_clock = cpu_clock->getCpuClock();
            bool first_command_executed = true;
            int cycle_counter = 0;

            while (process->getCommandCounter() < process->getLinesOfCode() && quantum < quantum_cycle)
            {
                if (delay_per_execution != 0)
                {
                    std::unique_lock<std::mutex> lock(cpu_clock->getMutex());
                    cpu_clock->getCondition().wait(lock, [&]
                    {
                        return cpu_clock->getCpuClock() > last_clock;
                    });
                    last_clock = cpu_clock->getCpuClock();
                }

                if (!first_command_executed || (++cycle_counter >= delay_per_execution))
                {
                    process->executeCurrentCommand();
                    first_command_executed = false;
                    cycle_counter = 0;
                    quantum++;
                }
            }

            std::this_thread::sleep_for(std::chrono::microseconds(2000));

            if (process->getCommandCounter() < process->getLinesOfCode())
            {
                process->setState(Process::ProcessState::READY);
                std::lock_guard<std::mutex> lock(queue_mutex_);
                process_queue_.push(process);
            }
            else
            {
                process->setState(Process::ProcessState::FINISHED);
                memory_allocator_->deallocate(process);
                process->setMemory(nullptr);
            }

            {
                std::lock_guard<std::mutex> lock(active_threads_mutex_);
                active_threads_--;
            }

            queue_condition_.notify_one();
        }

        CoreStateManager::getInstance().setCoreState(core_id, false, "");
    }
}

/**
 * @brief Logs the current memory state for diagnostics.
 * @param cycle The cycle number for which the memory is being logged.
 */
void Scheduler::logMemoryState(int cycle)
{
    std::string filename = "generated_files/memory_stamp_" + std::to_string(cycle) + ".txt";
    std::ofstream out_file(filename);

    if (out_file.is_open())
    {
        std::time_t current_time = std::time(nullptr);
        std::tm current_time_tm;

        if (localtime_s(&current_time_tm, &current_time) == 0)
        {
            char timestamp[100];
            std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &current_time_tm);
            out_file << "Timestamp: (" << timestamp << ")" << std::endl;
        }
        else
        {
            out_file << "Timestamp: (Error formatting time)" << std::endl;
        }

        out_file << "Number of processes in memory: " << memory_allocator_->getNProcess() << std::endl;
        out_file << "Total external fragmentation in KB: " << memory_allocator_->getExternalFragmentation() << std::endl;
        out_file << "\n----end---- = " << memory_allocator_->getMaxMemory() << std::endl << std::endl;

        auto process_list = memory_allocator_->getProcessList();
        for (auto it = process_list.rbegin(); it != process_list.rend(); ++it)
        {
            size_t index = it->first;
            std::shared_ptr<Process> process = it->second;

            size_t size = process->getMemoryRequired();
            const std::string& proc_name = process->getName();

            out_file << "Index: " << index << std::endl;
            out_file << "Process Name: " << proc_name << std::endl;
            out_file << "Memory Size: " << size << " KB" << std::endl << std::endl;
        }

        out_file << "----start---- = 0" << std::endl;

        out_file.close();
    }
    else
    {
        std::cerr << "Error: Unable to open the file for writing: " << filename << std::endl;
    }
}
