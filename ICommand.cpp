#include <ctime>   
#include <memory>  
#include <string>  
#include <vector>  
#include <fstream> 
#include <iomanip> 
#include <sstream> 
#include <iostream>

using namespace std;

/**
 * @class ICommand
 * @brief Abstract base class representing a command in the system.
 * 
 * This class acts as an interface for different types of commands that can be executed.
 * Each command will have a process ID, a specific type, and a behavior defined by derived classes.
 */
class ICommand
{
public:
    /**
     * @enum CommandType
     * @brief Represents the types of commands available.
     * 
     * This can be extended with more command types in the future (e.g., SAVE, LOAD, etc.).
     */
    enum CommandType
    {
        PRINT  ///< A command type used for print-related operations.
    };

    /**
     * @brief Constructor for ICommand.
     * 
     * @param process_id The ID of the process that owns this command.
     * @param command_type The type of the command (from the CommandType enum).
     */
    ICommand(int process_id, CommandType command_type)
    {
        this->process_id = process_id;      // Store the process ID.
        this->command_type = command_type;  // Store the command type.
    }

    /**
     * @brief Pure virtual function to execute the command.
     * 
     * This function must be implemented by derived classes to define how the command will execute.
     */
    virtual void execute() = 0;

    /**
     * @brief Pure virtual function to set the core ID on which the command will run.
     * 
     * This function must be implemented by derived classes to specify how the core ID is assigned.
     * @param core_id The ID of the core to which this command will be assigned.
     */
    virtual void setCore(int core_id) = 0;

protected:
    int process_id;       ///< The ID of the process associated with this command.
    CommandType command_type;  ///< The type of the command (PRINT, etc.).
};