#ifndef CONSOLE_SCREEN_H
#define CONSOLE_SCREEN_H

#include "Process.h"

#include <map>
#include <memory>
#include <string>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <mutex>

/**
 * @class ConsoleScreen
 * @brief Manages the display of process information on the console.
 */
class ConsoleScreen
{
public:
    /**
     * @brief Displays the header information on the console.
     */
    void displayHeader();

    /**
     * @brief Displays all processes in the provided map.
     * @param process_list Map of process names to process objects.
     * @param num_cpu Number of CPU cores.
     */
    void displayAllProcess(std::map<std::string, std::shared_ptr<Process>> process_list, int num_cpu);

    /**
     * @brief Displays updated information of a process.
     * @param process Shared pointer to the process to be updated.
     */
    void displayUpdatedProcess(std::shared_ptr<Process> process);

    /**
     * @brief Displays a specific process.
     * @param process Shared pointer to the process to be displayed.
     */
    void displayScreen(std::shared_ptr<Process> process);

    /**
     * @brief Displays all processes and writes the information to an output stream.
     * @param process_list Map of process names to process objects.
     * @param num_cpu Number of CPU cores.
     * @param out Output stream where the data is to be written.
     */
    void displayAllProcessToStream(std::map<std::string, std::shared_ptr<Process>> process_list, int num_cpu, std::ostream& out);

    /**
     * @brief Gets the current timestamp.
     * @return Current timestamp as a string.
     */
    std::string getCurrentTimestamp();

    std::mutex process_list_mutex; ///< Mutex for managing access to process list.
    std::mutex core_states_mutex;  ///< Mutex for managing access to core states.
};

#endif
