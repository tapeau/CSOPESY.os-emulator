#pragma once

#include <ctime>     
#include <string>    
#include <fstream>   
#include <iomanip>   
#include <sstream>   
#include <chrono>    
#include "ICommand.h"

// PrintCommand class that implements the ICommand interface.
// This class represents a print operation that writes text to a file.
class PrintCommand : public ICommand
{
public:
    /**
     * Constructor to initialize the PrintCommand object.
     * 
     * @param process_id     - Unique identifier for the process.
     * @param core_id        - Identifier of the core assigned to this process.
     * @param text_to_print  - The text that will be written to the output file.
     * @param process_name   - Name of the process (used as the output file name).
     */
    PrintCommand(int process_id, int core_id, const std::string &text_to_print, const std::string &process_name)
        : ICommand(process_id, CommandType::PRINT),  // Initialize the base class.
          core_id(core_id), 
          text_to_print(text_to_print), 
          process_name(process_name) 
    {
    }

    /**
     * Override the execute() function to perform the print operation.
     * This function writes the current timestamp, core ID, and text to a file named <process_name>.txt.
     */
    void execute() override;

    /**
     * Override the setCore() function to change the core ID of this process.
     * 
     * @param core_id - The new core ID to be set.
     */
    void setCore(int core_id) override;
    // {
    //     this->core_id = core_id;  // Update the core ID with the new value.
    // }

private:
    // Private member variables
    int process_id;         // Unique identifier for the process.
    int core_id;                // The core assigned to the process.
    std::string text_to_print;  // The text to be printed to the output file.
    std::string process_name;   // Name of the process.

    /**
     * Helper function to get the current timestamp in a formatted string, including milliseconds.
     * 
     * @return A string representing the current date, time, and milliseconds.
     */
    std::string getCurrentTimestamp();
};
