#ifndef PROCESS_H
#define PROCESS_H

#include "ICommand.h"
#include "PrintCommand.h"

#include <memory>
#include <string>
#include <vector>
#include <ctime>
#include <cmath>
#include <chrono>

/**
 * @class Process
 * @brief Represents a process that can execute commands and manage memory.
 *
 * This class holds information about a process, including its ID, name, memory requirements,
 * and command list. It provides methods for generating commands, executing commands,
 * and managing memory.
 */
class Process
{
public:
    /**
     * @struct RequirementFlags
     * @brief Flags that indicate resource requirements for the process.
     */
    struct RequirementFlags
    {
        bool require_files;   ///< Indicates if the process requires files.
        int num_files;        ///< Number of files required by the process.
        bool require_memory;  ///< Indicates if the process requires memory.
        int memory_required;  ///< Memory required by the process.
    };

    /**
     * @enum ProcessState
     * @brief Represents the current state of the process.
     */
    enum ProcessState
    {
        READY,     ///< Process is ready to execute.
        RUNNING,   ///< Process is currently running.
        WAITING,   ///< Process is waiting for resources.
        FINISHED   ///< Process has finished execution.
    };

    // Constructor
    Process(int pid, const std::string& name, const std::string& time, int core, int min_ins, int max_ins, size_t mem_per_proc, size_t mem_per_frame);

    // Method to execute the current command
    void executeCurrentCommand();

    // Getters and Setters
    int getCommandCounter() const;
    int getLinesOfCode() const;
    int getCPUCoreID() const;
    size_t getMemoryRequired() const;
    void setCPUCoreID(int core);
    ProcessState getState() const;
    void setState(ProcessState state);
    size_t getPID() const;
    std::string getName() const;
    std::string getTime() const;
    void setMemory(void* memory);
    void* getMemory() const;
    void setAllocTime();
    std::chrono::time_point<std::chrono::system_clock> getAllocTime() const;
    size_t getNumPages() const;
    void calculateFrame();

    // Method to generate print commands
    void generateCommands(int min_ins, int max_ins);

private:
    size_t pid_;                        ///< Process ID.
    std::string name_;                  ///< Process name.
    std::string time_;                  ///< Time the process was created.
    std::vector<std::shared_ptr<ICommand>> command_list_; ///< List of commands for the process.
    std::chrono::time_point<std::chrono::system_clock> allocation_time_; ///< Allocation time for memory.

    size_t mem_per_proc_;               ///< Memory required per process.
    size_t mem_per_frame_;              ///< Memory per frame.
    size_t num_pages_;                  ///< Number of pages required.
    int command_counter_ = 0;           ///< Command counter.
    int cpu_core_id_;                   ///< CPU core ID assigned to the process.
    RequirementFlags requirement_flags_; ///< Flags indicating process requirements.
    ProcessState process_state_;        ///< Current state of the process.
    void* memory_;                      ///< Pointer to the memory allocated to the process.
};

#endif
