#pragma once

#include <atomic>
#include <condition_variable>
#include <iostream>
#include <thread>
#include <mutex>

// Clock class that simulates a CPU clock incrementing in a separate thread.
class Clock
{
public:
    Clock();                        // Constructor to initialize the clock to zero
    int getClock();                 // Retrieves the current clock value
    void startClock();              // Starts the clock's counting thread
    void stopClock();               // Stops the clock's counting thread

    // Provides access to the clock's condition variable for synchronization
    std::condition_variable& getCondition() 
    { 
        return cycle_condition; 
    }

    // Provides access to the clock's mutex for synchronization
    std::mutex& getMutex() 
    { 
        return clock_mutex; 
    }

private:
    std::atomic<int> cpu_clock;     // Atomic integer to store the current clock cycle count
    bool is_running = false;        // Boolean flag to control the clock thread
    std::thread clock_thread;       // Thread to run the clock incrementing process
    std::condition_variable cycle_condition; // Condition variable to notify cycle updates
    std::mutex clock_mutex;         // Mutex to protect clock updates
};
