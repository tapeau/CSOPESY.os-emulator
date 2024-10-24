#pragma once

#include <memory>
#include <string>
#include <vector>
#include "ICommand.h"
#include "PrintCommand.h"

/**
 * @class Process
 * Represents a process that executes a sequence of commands.
 * Each process is assigned a unique ID, name, creation time, and is associated with a specific CPU core.
 */
class Process
{
public:
    /**
     * @struct RequirementFlags
     * Holds information about the resource requirements of a process.
     * - require_files: Indicates whether the process needs files.
     * - num_files: Number of files required.
     * - require_memory: Indicates whether memory allocation is required.
     * - memory_required: Amount of memory required (in MB).
     */
    struct RequirementFlags
    {
        bool require_files;
        int num_files;
        bool require_memory;
        int memory_required;
    };

    /**
     * @enum ProcessState
     * Represents the current state of the process.
     * - READY: Process is ready to run.
     * - RUNNING: Process is currently executing.
     * - WAITING: Process is waiting for some resources or event.
     * - FINISHED: Process has completed execution.
     */
    enum ProcessState
    {
        READY,
        RUNNING,
        WAITING,
        FINISHED
    };

    /**
     * @brief Constructor to initialize a Process object.
     * 
     * @param process_id Unique identifier for the process.
     * @param process_name Name of the process.
     * @param creation_time Time when the process was created.
     * @param core_id ID of the CPU core to which the process is assigned.
     * @param min_instructions Minimum number of instructions the process will execute.
     * @param max_instructions Maximum number of instructions the process can execute.
     */
    Process(int process_id, const std::string &process_name, const std::string &creation_time, 
            int core_id, int min_instructions, int max_instructions);

    /**
     * @brief Executes the next command in the process’s command list.
     */
    void executeCurrentCommand();

    // Getters to retrieve various properties of the process.

    /**
     * @return Current command counter (i.e., how many commands have been executed).
     */
    int getCommandCounter() const;

    /**
     * @return Total number of commands in the process.
     */
    int getLinesOfCode() const;

    /**
     * @return The ID of the CPU core assigned to the process.
     */
    int getCPUCoreID() const;

    /**
     * @brief Sets the CPU core ID for the process.
     * @param core_id New CPU core ID to assign.
     */
    void setCPUCoreID(int core_id);

    /**
     * @return The current state of the process (READY, RUNNING, WAITING, or FINISHED).
     */
    ProcessState getState() const;

    /**
     * @brief Sets the process state.
     * @param state New state to assign (READY, RUNNING, etc.).
     */
    void setState(ProcessState state);

    /**
     * @return The unique process ID (PID).
     */
    int getPID() const;

    /**
     * @return The name of the process.
     */
    std::string getName() const;

    /**
     * @return The creation time of the process.
     */
    std::string getTime() const;

    /**
     * @brief Generates a random number of print commands for the process.
     * 
     * @param min_instructions Minimum number of commands to generate.
     * @param max_instructions Maximum number of commands to generate.
     * 
     * This method creates `PrintCommand` objects and adds them to the command list.
     */
    void generatePrintCommands(int min_instructions, int max_instructions);

private:
    int process_id;  // Unique identifier for the process.
    std::string process_name;  // Name of the process.
    std::string creation_time;  // Time the process was created.
    std::vector<std::shared_ptr<ICommand>> command_list;  // List of commands the process will execute.

    int command_counter = 0;  // Counter for the number of executed commands.
    int core_id;  // ID of the CPU core assigned to the process.
    RequirementFlags requirement_flags;  // Flags indicating the resource requirements of the process.
    ProcessState process_state;  // Current state of the process.
};
