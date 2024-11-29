#ifndef CONSOLE_MANAGER_H
#define CONSOLE_MANAGER_H

#include "ProcessManager.h"
#include "ConsoleScreen.h"
#include "Clock.h"

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <mutex>

/**
 * @class ConsoleManager
 * @brief Manages console views (screens) and handles commands for creating, managing, and displaying screen sessions.
 */
class ConsoleManager
{
private:
    int num_cpu;                        ///< Number of CPUs
    std::string scheduler;              ///< Scheduler type used
    int quantum_cycles;                 ///< Quantum cycles for round-robin scheduling
    int batch_process_freq;             ///< Frequency of batch process generation
    int min_ins;                        ///< Minimum instructions per process
    int max_ins;                        ///< Maximum instructions per process
    int delays_per_exec;                ///< Number of delays per process execution
    bool initialized = false;           ///< Indicates if the console manager has been initialized
    bool scheduler_running = false;     ///< Indicates if the scheduler is running
    Clock* cpu_clock;                ///< Pointer to Clock for timekeeping
    size_t max_mem;                     ///< Maximum overall memory
    size_t mem_per_frame;               ///< Memory per frame for paging
    size_t min_mem_per_proc;            ///< Minimum memory per process
    size_t max_mem_per_proc;            ///< Maximum memory per process

    // Structure for storing screen information
    struct Screen
    {
        std::string process_name;       ///< Name of the process associated with the screen
        int current_line;               ///< Current line number on the screen
        int total_lines;                ///< Total number of lines in the screen
        std::string timestamp;          ///< Timestamp of when the screen was created
    };

    std::map<std::string, Screen> screens; ///< Map to store all screens
    ConsoleScreen screen_manager;          ///< Console screen manager for display operations
    ProcessManager* process_manager;       ///< Pointer to manage processes

public:
    /**
     * @brief Create a new console session.
     * @param name Name of the console session to be created.
     */
    void createSession(const std::string& name);

    /**
     * @brief Generate a new console session.
     * @param name Name of the console session to be generated.
     */
    void generateSession(const std::string& name);

    /**
     * @brief Display all screens managed by ConsoleManager.
     */
    void displayAllScreens();

    /**
     * @brief Generate a report with all current screen data and save it to a file.
     */
    void reportUtil();

    /**
     * @brief Handle user commands and delegate to the appropriate function.
     * @param command Command provided by the user.
     */
    void handleCommand(const std::string& command);
};

#endif
