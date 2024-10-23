#pragma once

#include <string>
#include <map>
#include <memory>
#include <thread>
#include <iostream>
#include <fstream>

#include "ConsoleScreen.h"

class ConsoleManager
{
    // Configuration and state management variables
    int cpu_count{}, quantum_cycles{}, batch_frequency{};
    int min_instructions{}, max_instructions{}, delay_per_exec{};
    std::string scheduler_type;
    bool is_initialized = false, scheduler_running = false;

    // Structure for screen sessions
    struct Screen
    {
        std::string process_name;
        int current_line = 0, total_lines = 100;
        std::string timestamp;
    };

    std::map<std::string, Screen> screen_sessions;  // Active screen sessions
    ConsoleScreen screen_manager;

public:
    // Core methods for managing sessions and processes
    void createSession(const std::string& name);
    void generateSession(const std::string& name);
    void displayAllScreens();
    void saveReport();
    void handleCommand(const std::string& command);  // Entry point for user commands
};
