#include "Clock.h"

/**
 * @brief Constructor for Clock, initializes clock and active CPU count.
 */
Clock::Clock() : cpu_clock(0), active_num(0)
{
}

/**
 * @brief Get the current value of the CPU clock.
 * @return The current value of the CPU clock as an integer.
 */
int Clock::getCpuClock()
{
    return cpu_clock.load();
}

/**
 * @brief Start the CPU clock in a separate thread. Increases the clock value every millisecond.
 */
void Clock::startCpuClock()
{
    if (!is_running)
    {
        is_running = true;
        std::cout << "CPU Clock started\n";
        cpu_clock_thread = std::thread([this]()
        {
            while (is_running)
            {
                {
                    // Lock the mutex to safely increment the clock value
                    std::lock_guard<std::mutex> lock(clock_mutex);
                    cpu_clock++;
                }

                // Notify all waiting threads about the clock tick update
                cycle_condition.notify_all();

                // Sleep for a millisecond to simulate clock ticking
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });
    }
}

/**
 * @brief Stop the CPU clock and join the thread.
 */
void Clock::stopCpuClock()
{
    is_running = false;
    if (cpu_clock_thread.joinable())
    {
        cpu_clock_thread.join();
        std::cout << "CPU Clock stopped\n";
    }
}

/**
 * @brief Get the number of active CPUs.
 * @return The current number of active CPUs as an atomic integer.
 */
std::atomic<int> Clock::getActiveCpuNum()
{
    return active_num.load();
}

/**
 * @brief Increment the count of active CPUs.
 */
void Clock::incrementActiveCpuNum()
{
    active_num++;
}
