#ifndef CORE_STATE_MANAGER_H
#define CORE_STATE_MANAGER_H

#include <vector>
#include <mutex>
#include <string>

/**
 * @class CoreStateManager
 * @brief Manages the state of CPU cores in a singleton pattern.
 *
 * This class provides thread-safe access to the state of CPU cores, where each core
 * can be marked as busy or idle. The class follows the Singleton pattern to ensure
 * there is only one instance managing the core states throughout the system.
 */
class CoreStateManager
{
public:
    /**
     * @brief Get the singleton instance of CoreStateManager.
     * @return Reference to the singleton CoreStateManager instance.
     */
    static CoreStateManager& getInstance();

    /**
     * @brief Set the state of a specific core.
     * @param core_id The ID of the core to set.
     * @param state The new state for the core (true = busy, false = idle).
     * @param process_name The name of the process assigned to the core.
     */
    void setCoreState(int core_id, bool state, std::string process_name);

    /**
     * @brief Get the state of an individual core.
     * @param core_id The ID of the core.
     * @return True if the core is busy, false if it is idle.
     */
    bool getCoreState(int core_id);

    /**
     * @brief Get the state of all cores.
     * @return A constant reference to a vector containing the states of all cores.
     */
    const std::vector<bool>& getCoreStates() const;

    /**
     * @brief Initialize the cores by setting all to idle.
     * @param num_core Number of cores to initialize.
     */
    void initialize(int num_core);

    /**
     * @brief Get the list of processes assigned to each core.
     * @return A constant reference to a vector containing the names of processes on each core.
     */
    const std::vector<std::string>& getProcess() const;

private:
    /**
     * @brief Private constructor for singleton pattern.
     */
    CoreStateManager() = default;

    // Disable copy constructor and assignment operator to enforce Singleton pattern.
    CoreStateManager(const CoreStateManager&) = delete;
    CoreStateManager& operator=(const CoreStateManager&) = delete;

    std::vector<bool> core_states;              ///< Vector to store the state of each core (true = busy, false = idle).
    std::vector<std::string> process_names;     ///< Vector to store the process assigned to each core.

    mutable std::mutex core_states_mutex;       ///< Mutex for thread-safe access to core states.
};

#endif
