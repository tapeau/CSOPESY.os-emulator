#include "Clock.h"

// Constructor initializes the clock to zero
Clock::Clock() : cpu_clock(0) {}

// Retrieves the current value of the clock (in clock cycles)
int Clock::getClock()
{
    return cpu_clock.load();  // Ensures atomic access to the clock count
}

// Starts the clock, launching a thread that increments the clock cycle count
void Clock::startClock()
{
    // Only start if the clock is not already running
    if (!is_running)
    {
        is_running = true; // Set running flag to true
        std::cout << "CPU Clock started." << std::endl;
        
        // Start the clock thread that increments the cycle count continuously
        clock_thread = std::thread([this]()
        {
            while (is_running)
            {
                // Lock mutex to protect clock update
                {
                    std::lock_guard<std::mutex> lock(clock_mutex);
                    ++cpu_clock;  // Increment clock count
                }
                
                // Notify all waiting threads of the updated clock cycle
                cycle_condition.notify_all();

                // Pause for 1 millisecond to simulate a clock cycle
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });
    }
}

// Stops the clock by terminating the clock thread
void Clock::stopClock()
{
    is_running = false; // Set running flag to false to stop the thread loop

    // Join the thread to ensure it stops gracefully
    if (clock_thread.joinable())
    {
        clock_thread.join();
        std::cout << "CPU Clock stopped." << std::endl;
    }
}
