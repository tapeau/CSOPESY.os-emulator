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

    // **Set the state of a specific core**
    // Params:
    //  - core_id: ID of the core (1-based index).
    //  - state: True if the core is busy, False if idle.
    void setCoreState(int core_id, bool state);

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

    // **Mutex to ensure thread-safe access to core states** 
    // Marked `mutable` to allow modification in const methods.
    mutable std::mutex mutex;
};
