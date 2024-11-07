#include <ctime>     
#include <memory>    
#include <string>    
#include <vector>    
#include <fstream>   
#include <iomanip>   
#include <sstream>   
#include <iostream>  
#include "ICommand.h"

using namespace std;

// PrintCommand class inherits from ICommand and implements the print functionality.
class PrintCommand : public ICommand
{
public:
    /**
     * Constructor to initialize the PrintCommand object.
     * 
     * @param process_id     - Unique identifier for the process.
     * @param core_id        - Identifier of the core assigned to this process.
     * @param text_to_print  - The text that will be written to the output file.
     * @param process_name   - Name of the process (used to name the output file).
     */
    PrintCommand(int process_id, int core_id, const std::string &text_to_print, std::string &process_name)
        : ICommand(process_id, CommandType::PRINT),  // Initialize base class (ICommand) with process ID and command type.
          core_id(core_id), 
          text_to_print(text_to_print), 
          process_name(process_name)
    {}

    /**
     * Override the execute() function to perform the print command.
     * This function writes the text to a file named <process_id>.txt.
     */
    void execute() override
    {
        // Open the output file in append mode.
        ofstream outfile(to_string(process_id) + ".txt", std::ios::app);
        // Write the text to the file followed by a new line.
        outfile << text_to_print << std::endl;
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
    string text_to_print;   // The text to be printed to the output file.
    int core_id;            // The core assigned to the process.
    int process_id;         // Unique identifier for the process.
    string process_name;    // Name of the process.

    /**
     * Helper function to get the current timestamp in a formatted string.
     * 
     * @return A string representing the current date and time.
     */
    string getCurrentTimestamp()
    {
        std::time_t now = std::time(nullptr);  // Get the current time.
        std::tm local_time;                    // Structure to store local time.
        
        // Convert time to local time safely using localtime_s.
        localtime_s(&local_time, &now); 
        
        // Use a string stream to format the date and time.
        std::ostringstream oss;
        oss << std::put_time(&local_time, "%m/%d/%Y, %I:%M:%S %p");
        
        return oss.str();  // Return the formatted timestamp.
    }
};
