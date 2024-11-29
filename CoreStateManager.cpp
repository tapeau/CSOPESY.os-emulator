#include "CoreStateManager.h"
#include <iostream> // For error logging

// This Class requires that core IDs start at 1

/**
 * @brief Get the singleton instance of CoreStateManager.
 * @return Reference to the singleton CoreStateManager instance.
 */
CoreStateManager& CoreStateManager::getInstance()
{
    static CoreStateManager instance;
    return instance;
}

/**
 * @brief Set the state of a specific core.
 * @param core_id The ID of the core to set (1-based index).
 * @param state The new state for the core (true = busy, false = idle).
 * @param process_name The name of the process assigned to the core.
 */
void CoreStateManager::setCoreState(int core_id, bool state, std::string process_name)
{
    std::lock_guard<std::mutex> lock(core_states_mutex);

    // Adjust for core ID starting at 1
    core_id--;

    if (core_id >= 0 && core_id < core_states.size())
    {
        core_states[core_id] = state;
        process_names[core_id] = process_name;
    }
    else
    {
        std::cerr << "Error: Core ID " << (core_id + 1) << " is out of range!" << std::endl;
    }
}

/**
 * @brief Get the state of an individual core.
 * @param core_id The ID of the core (1-based index).
 * @return True if the core is busy, false if it is idle.
 */
bool CoreStateManager::getCoreState(int core_id)
{
    std::lock_guard<std::mutex> lock(core_states_mutex);

    // Adjust for core ID starting at 1
    core_id--;

    if (core_id >= 0 && core_id < core_states.size())
    {
        return core_states[core_id];
    }
    else
    {
        std::cerr << "Error: Core ID " << (core_id + 1) << " is out of range!" << std::endl;
        return false; // Default return for invalid core_id
    }
}

/**
 * @brief Get the list of processes assigned to each core.
 * @return A constant reference to a vector containing the names of processes on each core.
 */
const std::vector<std::string>& CoreStateManager::getProcess() const
{
    std::lock_guard<std::mutex> lock(core_states_mutex);
    return process_names;
}

/**
 * @brief Get the state of all cores.
 * @return A constant reference to a vector containing the states of all cores.
 */
const std::vector<bool>& CoreStateManager::getCoreStates() const
{
    std::lock_guard<std::mutex> lock(core_states_mutex);
    return core_states;
}

/**
 * @brief Initialize the cores by setting all cores to idle (false).
 * @param num_core Number of cores to initialize.
 */
void CoreStateManager::initialize(int num_core)
{
    std::lock_guard<std::mutex> lock(core_states_mutex);
    core_states.resize(num_core, false);  // Initialize all cores to idle (false)
    process_names.resize(num_core, "");   // Initialize all cores with no assigned process
}
