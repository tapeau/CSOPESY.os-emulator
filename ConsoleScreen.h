#pragma once

#include <map>
#include <memory>
#include <string>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <mutex>
#include "Process.h"
#include "Clock.h"

// The ConsoleScreen class manages the console display of processes and user interactions.
class ConsoleScreen
{
public:
    // Mutex object for storing process list
    std::mutex process_list_mutex;

    // Mutex object for storing core states
    std::mutex core_states_mutex;
    
    // Displays the header information on the console.
    void printHeader();

    // Displays the header information on the console using Windows.h for colors.
    void printHeaderUsingHandle() const;

    // Displays all processes in the given map.
    void showAllProcesses(std::map<std::string, std::shared_ptr<Process>> process_list, int cpu_count);

    // Displays updated information for a specific process.
    void showProcessUpdated(std::shared_ptr<Process> process);

    // Displays a specific process and allows user interaction.
    void showScreen(std::shared_ptr<Process> process);

    // Streams all processes to a given output stream.
    void streamAllProcesses(std::map<std::string, std::shared_ptr<Process>> process_list, int num_core, std::ostream& out);

    void process_smi(std::map<std::string, std::shared_ptr<Process>> process_list, int num_core, int max_mem);

    void vmstat(std::map<std::string, std::shared_ptr<Process>> process_list, int cpu_count, int max_mem, Clock* clock);

    // Retrieves the current timestamp.
    std::string getCurrentTimestamp();
};
