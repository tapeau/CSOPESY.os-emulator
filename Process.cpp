#include "Process.h"

/**
 * @brief Constructor for Process.
 * @param pid The process ID.
 * @param name The process name.
 * @param time The time when the process was created.
 * @param core The core ID assigned to the process.
 * @param min_ins Minimum number of instructions.
 * @param max_ins Maximum number of instructions.
 * @param mem_per_proc Memory required per process.
 * @param mem_per_frame Memory per frame.
 */
Process::Process(int pid, const std::string& name, const std::string& time, int core, int min_ins, int max_ins, size_t mem_per_proc, size_t mem_per_frame)
    : pid_(pid), name_(name), time_(time), cpu_core_id_(core), process_state_(READY), mem_per_proc_(mem_per_proc), mem_per_frame_(mem_per_frame), memory_(nullptr)
{
    calculateFrame();
}

/**
 * @brief Method to execute the current command in the process's command list.
 */
void Process::executeCurrentCommand()
{
    if (command_counter_ < command_list_.size())
    {
        command_list_[command_counter_]->setCore(cpu_core_id_);
        command_list_[command_counter_]->execute();
        command_counter_++;
    }
}

/**
 * @brief Calculate the number of frames required by the process.
 */
void Process::calculateFrame()
{
    num_pages_ = static_cast<size_t>(std::ceil(static_cast<double>(mem_per_proc_) / mem_per_frame_));
}

/**
 * @brief Getter for the command counter.
 * @return The current command count.
 */
int Process::getCommandCounter() const
{
    return command_counter_;
}

/**
 * @brief Set the memory pointer for the process.
 * @param memory Pointer to the memory allocated to the process.
 */
void Process::setMemory(void* memory)
{
    memory_ = memory;
}

/**
 * @brief Get the memory pointer for the process.
 * @return Pointer to the memory allocated to the process.
 */
void* Process::getMemory() const
{
    return memory_;
}

/**
 * @brief Getter for the number of lines of code.
 * @return The number of commands in the command list.
 */
int Process::getLinesOfCode() const
{
    return command_list_.size();
}

/**
 * @brief Get the memory required by the process.
 * @return The memory size required.
 */
size_t Process::getMemoryRequired() const
{
    return mem_per_proc_;
}

/**
 * @brief Getter for the CPU core ID.
 * @return The CPU core ID assigned to the process.
 */
int Process::getCPUCoreID() const
{
    return cpu_core_id_;
}

/**
 * @brief Setter for the CPU core ID.
 * @param core The new CPU core ID to assign to the process.
 */
void Process::setCPUCoreID(int core)
{
    cpu_core_id_ = core;
}

/**
 * @brief Getter for the process state.
 * @return The current state of the process.
 */
Process::ProcessState Process::getState() const
{
    return process_state_;
}

/**
 * @brief Set the process state.
 * @param state The new state of the process.
 */
void Process::setState(ProcessState state)
{
    process_state_ = state;
}

/**
 * @brief Getter for the process ID (PID).
 * @return The PID of the process.
 */
size_t Process::getPID() const
{
    return pid_;
}

/**
 * @brief Getter for the process name.
 * @return The name of the process.
 */
std::string Process::getName() const
{
    return name_;
}

/**
 * @brief Getter for the process creation time.
 * @return The creation time of the process.
 */
std::string Process::getTime() const
{
    return time_;
}

/**
 * @brief Generate print commands for the process.
 * @param min_ins Minimum number of instructions.
 * @param max_ins Maximum number of instructions.
 */
void Process::generateCommands(int min_ins, int max_ins)
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)) + pid_);

    int num_commands = min_ins + (std::rand() % (max_ins - min_ins + 1));

    for (int i = 1; i <= num_commands; ++i)
    {
        std::shared_ptr<ICommand> cmd = std::make_shared<PrintCommand>(pid_, cpu_core_id_, "Hello World From " + name_ + " started.", name_);
        command_list_.push_back(cmd);
    }
}

/**
 * @brief Set the allocation time for the process.
 */
void Process::setAllocTime()
{
    allocation_time_ = std::chrono::system_clock::now();
}

/**
 * @brief Getter for the allocation time.
 * @return The allocation time as a chrono time_point.
 */
std::chrono::time_point<std::chrono::system_clock> Process::getAllocTime() const
{
    return allocation_time_;
}

/**
 * @brief Getter for the number of pages required by the process.
 * @return The number of pages required.
 */
size_t Process::getNumPages() const
{
    return num_pages_;
}
