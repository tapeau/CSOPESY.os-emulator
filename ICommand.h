#ifndef ICOMMAND_H
#define ICOMMAND_H

#include <string>

/**
 * @class ICommand
 * @brief Abstract base class for all command types that can be executed.
 *
 * This class represents a generic command with an associated process ID (pid)
 * and command type. Derived classes should implement the `execute()` and
 * `setCore()` methods to define specific behavior.
 */
class ICommand
{
public:
    /**
     * @brief Enum representing different command types.
     */
    enum CommandType
    {
        PRINT,  ///< Represents a print command.
        // Additional command types can be added here as needed.
    };

    /**
     * @brief Constructor for ICommand.
     * @param pid The process ID associated with the command.
     * @param command_type The type of command being created.
     */
    ICommand(int pid, CommandType command_type)
        : pid_(pid), command_type_(command_type)
    {
    }

    /**
     * @brief Execute the command.
     * This function must be implemented by derived classes.
     */
    virtual void execute() = 0;

    /**
     * @brief Set the core on which the command will be executed.
     * @param core The ID of the core to assign to the command.
     */
    virtual void setCore(int core) = 0;

protected:
    int pid_;                ///< Process ID associated with the command.
    CommandType command_type_; ///< The type of the command.
};

#endif
