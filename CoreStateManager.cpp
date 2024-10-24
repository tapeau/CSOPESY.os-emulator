#include <iostream>
#include "CoreStateManager.h"

// NOTE: Core IDs must start from 1.

// **Singleton instance retrieval** 
// This function ensures that only one instance of `CoreStateManager` exists throughout the program.
CoreStateManager& CoreStateManager::getInstance()
{
    static CoreStateManager instance;  // Thread-safe static initialization in C++11 and later.
    return instance;
}

// **Set the state of a specific core**
// Params:
//  - core_id: The ID of the core (1-based index).
//  - state: True if the core is busy, False if idle.
//
// Adjusts for the 1-based core ID by converting it to a 0-based index before accessing the state vector.
// If the core_id is out of range, an error message is printed.
void CoreStateManager::setCoreState(int core_id, bool state)
{
    std::lock_guard<std::mutex> lock(mutex);  // Ensure thread-safe access.

    core_id--;  // Convert 1-based core_id to 0-based index.

    if (core_id >= 0 && core_id < core_states.size())
    {
        core_states[core_id] = state;  // Update the core state.
    }
    else
    {
        std::cerr << "Core ID " << (core_id + 1) << " is out of range." << std::endl;
    }
}

// **Retrieve the state of a specific core**
// Params:
//  - core_id: The ID of the core (1-based index).
// Returns:
//  - True if the core is busy, False if idle.
//  - If core_id is out of range, it returns False by default and prints an error.
bool CoreStateManager::getCoreState(int core_id)
{
    std::lock_guard<std::mutex> lock(mutex);  // Ensure thread-safe access.

    core_id--;  // Convert 1-based core_id to 0-based index.

    if (core_id >= 0 && core_id < core_states.size())
    {
        return core_states[core_id];  // Return the current state.
    }
    else
    {
        std::cerr << "Core ID " << (core_id + 1) << " is out of range." << std::endl;
        return false;  // Return false if core_id is invalid.
    }
}

// **Get the states of all cores at once**
// Returns:
//  - A const reference to the vector containing the states of all cores (true = busy, false = idle).
const std::vector<bool>& CoreStateManager::getCoreStates() const
{
    return core_states;  // No need to lock, as we return a const reference.
}

// **Initialize the state of all cores**
// Params:
//  - cpu_count: Number of cores to initialize.
// This function resizes the internal vector to match the number of cores and sets all cores to idle (false).
void CoreStateManager::initialize(int cpu_count)
{
    std::lock_guard<std::mutex> lock(mutex);  // Ensure thread-safe access.
    core_states.resize(cpu_count, false);  // Initialize all cores to idle (false).
}
