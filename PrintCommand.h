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
    void execute() override
    {
        // Open the output file in append mode using the process name as the filename.
        std::ofstream outfile(process_name + ".txt", std::ios::app);
        
        // Write the timestamp, core ID, and text to the file.
        outfile << getCurrentTimestamp() << " Core:" << core_id << " \"" << text_to_print << "\"" << std::endl;
        
        // Close the output file.
        outfile.close();
    }

    /**
     * Override the setCore() function to change the core ID of this process.
     * 
     * @param core_id - The new core ID to be set.
     */
    void setCore(int core_id) override
    {
        this->core_id = core_id;  // Update the core ID with the new value.
    }

private:
    // Private member variables
    std::string text_to_print;  // The text to be printed to the output file.
    int core_id;                // The core assigned to the process.
    std::string process_name;   // Name of the process.

    /**
     * Helper function to get the current timestamp in a formatted string, including milliseconds.
     * 
     * @return A string representing the current date, time, and milliseconds.
     */
    std::string getCurrentTimestamp()
    {
        // Get the current system time with high precision.
        auto now = std::chrono::system_clock::now();
        
        // Convert to time_t to extract the calendar time.
        std::time_t time_now = std::chrono::system_clock::to_time_t(now);
        
        // Extract the milliseconds part from the time point.
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
        
        std::tm local_time;  // Structure to store local time.
        
        // Convert time to local time safely using localtime_s.
        localtime_s(&local_time, &time_now);
        
        // Use a string stream to format the timestamp.
        std::ostringstream oss;
        oss << std::put_time(&local_time, "(%m/%d/%Y %I:%M:%S")
            << '.' << std::setfill('0') << std::setw(3) << milliseconds.count()  // Append milliseconds.
            << std::put_time(&local_time, "%p)");
        
        return oss.str();  // Return the formatted timestamp.
    }
};
