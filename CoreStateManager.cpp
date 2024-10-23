#include <iostream>
#include "CoreStateManager.h"

// Return the singleton instance of CoreStateManager.
CoreStateManager& CoreStateManager::getInstance()
{
    static CoreStateManager instance;
    return instance;
}

// Set the state of a core, adjusting for 1-based core IDs.
void CoreStateManager::setCoreState(int core_id, bool is_busy)
{
    std::lock_guard<std::mutex> lock(mutex_); // Ensure thread-safe access.

    --core_id; // Convert to 0-based index.
    if (core_id >= 0 && core_id < core_states_.size())
    {
        core_states_[core_id] = is_busy;
    }
    else
    {
        std::cerr << "Error: Core ID " << (core_id + 1) << " is out of range!\n";
    }
}

// Get the state of a core, adjusting for 1-based core IDs.
bool CoreStateManager::getCoreState(int core_id) const
{
    std::lock_guard<std::mutex> lock(mutex_); // Ensure thread-safe access.

    --core_id; // Convert to 0-based index.
    if (core_id >= 0 && core_id < core_states_.size())
    {
        return core_states_[core_id];
    }

    std::cerr << "Error: Core ID " << (core_id + 1) << " is out of range!\n";
    return false; // Default value for invalid core ID.
}

// Get the states of all cores (no locking needed as it's a const method).
const std::vector<bool>& CoreStateManager::getAllCoreStates() const
{
    return core_states_;
}

// Initialize all cores to idle state (false).
void CoreStateManager::initializeCores(int core_count)
{
    std::lock_guard<std::mutex> lock(mutex_); // Ensure thread-safe access.
    core_states_.assign(core_count, false);   // Set all cores to idle.
}
