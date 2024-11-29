#include "ProcessManager.h"
#include "CoreStateManager.h"
#include <random>
#include <cmath>

/**
 * @brief Constructor for ProcessManager.
 */
ProcessManager::ProcessManager(int min_ins, int max_ins, int n_cpu, std::string scheduler_algo, int delays_per_exec,
                               int quantum_cycle, Clock* cpu_clock, size_t max_mem, size_t mem_per_frame,
                               size_t min_mem_per_proc, size_t max_mem_per_proc)
    : min_ins_(min_ins), max_ins_(max_ins), cpu_clock(cpu_clock), 
      min_mem_per_proc_(min_mem_per_proc), max_mem_per_proc_(max_mem_per_proc),
      max_mem_(max_mem), mem_per_frame_(mem_per_frame), num_cpu_(n_cpu)
{
    if (max_mem == mem_per_frame)
    {
        memory_allocator_ = new FlatMemoryAllocator(max_mem, mem_per_frame);
    }
    else
    {
        memory_allocator_ = new PagingAllocator(max_mem, mem_per_frame);
    }

    scheduler_ = new Scheduler(scheduler_algo, delays_per_exec, n_cpu, quantum_cycle, cpu_clock, memory_allocator_);
    scheduler_->setNumCPUs(n_cpu);

    scheduler_thread_ = std::thread(&Scheduler::start, scheduler_);
}

/**
 * @brief Adds a new process to the system.
 */
void ProcessManager::addProcess(std::string name, std::string time)
{
    pid_counter_++;
    auto process = std::make_shared<Process>(pid_counter_, name, time, -1, min_ins_, max_ins_, generateMemory(), mem_per_frame_);
    process_list_[name] = process;
    process->generateCommands(min_ins_, max_ins_);
    scheduler_->addProcess(process);
}

/**
 * @brief Retrieves a process by its name.
 */
std::shared_ptr<Process> ProcessManager::getProcess(std::string name)
{
    if (process_list_.find(name) != process_list_.end())
    {
        return process_list_[name];
    }
    return nullptr;
}

/**
 * @brief Retrieves all processes in the system.
 */
std::map<std::string, std::shared_ptr<Process>> ProcessManager::getAllProcess()
{
    return process_list_;
}

/**
 * @brief Prints system memory and process information statistics.
 */
void ProcessManager::processSmi()
{
    static std::mutex process_list_mutex;
    std::stringstream running;
    size_t memory_usage = 0;
    int core_usage = 0;

    const std::vector<std::string>& run = CoreStateManager::getInstance().getProcess();
    const std::vector<bool>& core_states = CoreStateManager::getInstance().getCoreStates();

    for (bool core_state : core_states)
    {
        if (core_state)
        {
            core_usage++;
        }
    }

    auto process_list_alloc = memory_allocator_->getProcessList();
    for (auto it = process_list_alloc.rbegin(); it != process_list_alloc.rend(); ++it)
    {
        const auto& pair = *it;
        size_t index = pair.first;
        std::shared_ptr<Process> process = pair.second;

        size_t size = process->getMemoryRequired();
        const std::string& proc_name = process->getName();

        std::stringstream temp;
        temp << std::left << std::setw(30) << proc_name << " ";
        memory_usage += size;
        temp << size << " KB" << std::endl;
        running << temp.str() << std::endl;
    }

    std::cout << "--------------------------------------------\n";
    std::cout << "| PROCESS-SMI V01.00 Driver Version: 01.00 |\n";
    std::cout << "--------------------------------------------\n";

    std::cout << "CPU-Util: " << (static_cast<double>(core_usage) / num_cpu_) * 100 << "%" << std::endl;
    std::cout << "Memory Usage: " << memory_usage << "KB" << " / " << max_mem_ << "KB" << std::endl;
    std::cout << "Memory Util: " << (static_cast<double>(memory_usage) / max_mem_) * 100 << "%" << std::endl;

    std::cout << "============================================\n"; 
    std::cout << "Running processes and memory usage:\n";
    std::cout << "--------------------------------------------\n";
    
    std::cout << running.str();
    std::cout << "--------------------------------------------\n";
}

/**
 * @brief Generates the memory required for a new process.
 * @return The memory size for the process.
 */
size_t ProcessManager::generateMemory()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    size_t min_exp = static_cast<size_t>(std::log2(min_mem_per_proc_));
    size_t max_exp = static_cast<size_t>(std::log2(max_mem_per_proc_));
    std::uniform_int_distribution<size_t> dist(min_exp, max_exp);
    size_t exp = dist(gen);
    return static_cast<size_t>(std::pow(2, exp));
}

/**
 * @brief Prints system memory statistics in a formatted style.
 */
void ProcessManager::vmStat()
{
    static std::mutex process_list_mutex;

    std::cout << "==========================================" << std::endl;
    std::cout << std::setw(12) << max_mem_ << " KB total memory" << std::endl;
    std::cout << std::setw(12) << max_mem_ - memory_allocator_->getExternalFragmentation() << " KB used memory" << std::endl;
    std::cout << std::setw(12) << memory_allocator_->getExternalFragmentation() << " KB free memory" << std::endl;
    std::cout << std::setw(12) << cpu_clock->getCpuClock() - cpu_clock->getActiveCpuNum() << " idle cpu ticks" << std::endl;
    std::cout << std::setw(12) << cpu_clock->getActiveCpuNum() << " active cpu ticks" << std::endl;
    std::cout << std::setw(12) << cpu_clock->getCpuClock() << " total cpu ticks" << std::endl;
    std::cout << std::setw(12) << memory_allocator_->getPageIn() << " pages paged in" << std::endl;
    std::cout << std::setw(12) << memory_allocator_->getPageOut() << " pages paged out" << std::endl;
    std::cout << "==========================================" << std::endl;
}
