#pragma once

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include "ProcessManager.h"
#include "ConsoleScreen.h"
#include "Clock.h"

// ConsoleManager class to manage console screens and user commands
class ConsoleManager
{
    int cpu_count; // Number of CPUs
    std::string scheduler_algorithm; // Selected scheduling algorithm
    int quantum_cycles; // Quantum cycles for scheduling
    int batch_process_frequency; // Frequency of batch process creation
    int min_instructions; // Minimum instructions per process
    int max_instructions; // Maximum instructions per process
    int delays_per_execution; // Delays per execution cycle
    bool is_initialized = false; // Initialization status flag
    bool is_scheduler_running = false; // Scheduler running flag
    Clock* cpu_clock; // CPU clock object
    int max_overall_mem; //Overall memory in bytes
    int mem_per_frame; //Size of memory in KB per frame. This is also the memory size per page
    int mem_per_proc; //Fixed amount of memory per process

private:
    // Structure to store screen information
    struct Screen
    {
        std::string process_name; // Name of the associated process
        int current_line; // Current line displayed on screen
        int total_lines; // Total lines to display
        std::string timestamp; // Timestamp of session creation
    };

    std::map<std::string, Screen> screens; // Map to store screen sessions
    ConsoleScreen screen_manager; // Screen manager for display operations
    ProcessManager* process_manager; // Pointer to process manager

public:
    // Methods to manage screens and commands
    void createScreen(const std::string &session_name);
    void createScreenSilent(const std::string &session_name);
    void showAllScreens();
    void reportUtilization();
    void processCommand(const std::string &command);
    void generateProcesses(int num_process); // command to generate processes mainly for debugging 
};
