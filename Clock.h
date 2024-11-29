#ifndef CLOCK_H
#define CLOCK_H

#include <atomic>
#include <condition_variable>
#include <iostream>
#include <thread>
#include <mutex>

/**
 * @class Clock
 * @brief Manages a simulated CPU clock, including tracking active CPUs and providing clock cycle updates.
 */
class Clock
{
public:
    Clock();

    /**
     * @brief Get the current CPU clock value.
     * @return The current value of the CPU clock.
     */
    int getCpuClock();

    /**
     * @brief Start the CPU clock in a separate thread.
     */
    void startCpuClock();

    /**
     * @brief Stop the CPU clock.
     */
    void stopCpuClock();

    /**
     * @brief Get the number of active CPUs.
     * @return The current number of active CPUs.
     */
    std::atomic<int> getActiveCpuNum();

    /**
     * @brief Increment the count of active CPUs.
     */
    void incrementActiveCpuNum();

    /**
     * @brief Accessor to use the condition variable externally.
     * @return Reference to the condition variable.
     */
    std::condition_variable& getCondition() 
    { 
        return cycle_condition; 
    }

    /**
     * @brief Accessor to use the mutex externally.
     * @return Reference to the mutex.
     */
    std::mutex& getMutex() 
    { 
        return clock_mutex; 
    }

private:
    std::atomic<int> cpu_clock;           ///< The simulated CPU clock counter.
    bool is_running = false;              ///< Flag to indicate whether the clock is running.
    std::thread cpu_clock_thread;         ///< Thread to simulate the CPU clock.
    std::condition_variable cycle_condition; ///< Condition variable to notify on each clock tick.
    std::mutex clock_mutex;               ///< Mutex to protect access to the clock.
    std::atomic<int> active_num;          ///< Number of active CPUs.
};

#endif
