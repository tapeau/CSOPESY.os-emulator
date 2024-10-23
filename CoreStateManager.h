#pragma once

#include <vector>
#include <mutex>

// Manages core states (busy or idle) in a thread-safe, singleton pattern.
class CoreStateManager
{
public:
    // Singleton access to the CoreStateManager instance.
    static CoreStateManager& getInstance();

    // Set the state of a specific core (true = busy, false = idle).
    void setCoreState(int core_id, bool is_busy);

    // Get the state of a specific core (true = busy, false = idle).
    bool getCoreState(int core_id) const;

    // Retrieve the states of all cores.
    const std::vector<bool>& getAllCoreStates() const;

    // Initialize all cores to idle state (false).
    void initializeCores(int core_count);

private:
    CoreStateManager() = default;                      // Private constructor for singleton pattern.
    CoreStateManager(const CoreStateManager&) = delete; // Disable copy construction.
    CoreStateManager& operator=(const CoreStateManager&) = delete; // Disable copy assignment.

    std::vector<bool> core_states_; // Stores the busy/idle state for each core.
    mutable std::mutex mutex_;      // Ensures thread-safe access to core states.
};
