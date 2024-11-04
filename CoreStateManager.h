#pragma once

#include <vector>
#include <mutex>

// **CoreStateManager Class** 
// Manages the state (busy/idle) of multiple CPU cores using a Singleton pattern.
// Core IDs are expected to start from 1.
class CoreStateManager
{
public:
    // **Retrieve the Singleton instance** 
    // Ensures that only one instance of the class is ever created.
    static CoreStateManager& getInstance();

    // **Flip the state of a specific core**
    // Params:
    //  - core_id: ID of the core (1-based index).
    void flipCoreState(int core_id, std::string process_name);

    // **Get the state of an individual core**
    // Params:
    //  - core_id: ID of the core (1-based index).
    // Returns:
    //  - True if the core is busy, False if idle.
    //  - Prints an error if the core ID is out of range.
    bool getCoreState(int core_id);

    // **Retrieve the states of all cores**
    // Returns:
    //  - A const reference to the vector containing the state of all cores (true = busy, false = idle).
    const std::vector<bool>& getCoreStates() const;

    // **Retrieve the processes of the manager**
    // Returns:
    //  - A const reference to the vector containing the names of all processes in the manager.
    const std::vector<std::string>& getProcesses() const;

    // **Initialize all cores to idle (false)**
    // Params:
    //  - num_cores: Number of cores to manage.
    // Resizes the internal vector and sets the state of all cores to idle.
    void initialize(int num_cores);

private:
    // **Private constructor** 
    // Ensures that instances can only be created through `getInstance()`.
    CoreStateManager() = default;

    // **Delete copy constructor and assignment operator**
    // Prevents copying or assigning the Singleton instance.
    CoreStateManager(const CoreStateManager&) = delete;
    CoreStateManager& operator=(const CoreStateManager&) = delete;

    // **Vector to store the state of each core** 
    // true = busy, false = idle.
    std::vector<bool> core_states;

    // **Vector to store the names of each processes in the manager**
    std::vector<std::string> processes;

    // **Mutex to ensure thread-safe access to core states** 
    // Marked `mutable` to allow modification in const methods.
    mutable std::mutex mutex;
};
