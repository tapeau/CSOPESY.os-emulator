#pragma once

#include <string>

/**
 * @class ICommand
 * @brief Abstract base class for representing commands that can be executed.
 * 
 * This class defines the common interface for various command types. It serves as the base 
 * for more specific command implementations. Each command will have a process ID and a 
 * command type. It also requires derived classes to implement how the command executes 
 * and on which core it runs.
 */
class ICommand
{
public:
    /**
     * @enum CommandType
     * @brief Enum for defining the different types of commands.
     * 
     * This can be expanded with more commands as needed, such as LOG, SAVE, or EXIT.
     */
    enum CommandType
    {
        PRINT  ///< Command type representing print-related operations.
        // Add other command types here as needed.
    };

    /**
     * @brief Constructor to initialize the command with a process ID and command type.
     * 
     * @param process_id The ID of the process to which this command belongs.
     * @param command_type The type of the command being initialized.
     */
    ICommand(int process_id, CommandType command_type)
        : process_id(process_id), command_type(command_type) {}

    /**
     * @brief Pure virtual method to execute the command.
     * 
     * This function must be implemented by all derived classes to specify the 
     * actual behavior of the command.
     */
    virtual void execute() = 0;

    /**
     * @brief Pure virtual method to assign a core ID for the command execution.
     * 
     * This must be implemented in derived classes to specify which core the command 
     * will run on, facilitating better CPU resource management.
     * 
     * @param core_id The ID of the CPU core to assign the command to.
     */
    virtual void setCore(int core_id) = 0;

protected:
    int process_id;  ///< The ID of the process owning this command.
    CommandType command_type;  ///< The type of the command (PRINT, etc.).
};
