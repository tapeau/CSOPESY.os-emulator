#pragma once

#include <map>
#include <memory>
#include <string>
#include <iomanip>
#include <sstream>
#include <iostream>
#include "Process.h"

// The ConsoleScreen class manages the console display of processes and user interactions.
class ConsoleScreen
{
public:
    // Displays the header information on the console.
    void printHeader();

    // Displays all processes in the given map.
    void showAllProcesses(std::map<std::string, std::shared_ptr<Process>> process_list, int cpu_count);

    // Displays updated information for a specific process.
    void showProcessUpdated(std::shared_ptr<Process> process);

    // Displays a specific process and allows user interaction.
    void showScreen(std::shared_ptr<Process> process);

    // Streams all processes to a given output stream.
    void streamAllProcesses(std::map<std::string, std::shared_ptr<Process>> process_list, int num_core, std::ostream& out);

    // Retrieves the current timestamp.
    std::string getCurrentTimestamp();
};
