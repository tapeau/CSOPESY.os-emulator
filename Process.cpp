#include "Process.h"
#include <cstdlib>  
#include <ctime>

// Constructor implementation.
Process::Process(int process_id, const std::string &process_name, const std::string &creation_time, 
                 int core_id, int min_instructions, int max_instructions)
    : process_id(process_id), process_name(process_name), creation_time(creation_time), 
      core_id(core_id), process_state(READY) 
{
    // Generate the list of print commands when the process is created.
    generatePrintCommands(min_instructions, max_instructions);
}

// Executes the current command in the command list.
void Process::executeCurrentCommand()
{
    // Ensure there are still commands to execute.
    if (command_counter < command_list.size())
    {
        // Set the core ID for the command and execute it.
        command_list[command_counter]->setCore(core_id);
        command_list[command_counter]->execute();

        // Increment the command counter to move to the next command.
        command_counter++;
    }
}

// Returns the number of commands executed so far.
int Process::getCommandCounter() const
{
    return command_counter;
}

// Returns the total number of commands for this process.
int Process::getLinesOfCode() const
{
    return command_list.size();
}

// Returns the CPU core ID assigned to the process.
int Process::getCPUCoreID() const
{
    return core_id;
}

// Sets a new CPU core ID for the process.
void Process::setCPUCoreID(int core_id)
{
    this->core_id = core_id;  // Assign the new core ID.
}

// Returns the current state of the process.
Process::ProcessState Process::getState() const
{
    return process_state;
}

// Sets the process state to a new value.
void Process::setState(ProcessState state)
{
    process_state = state;
}

// Returns the unique process ID (PID).
int Process::getPID() const
{
    return process_id;
}

// Returns the name of the process.
std::string Process::getName() const
{
    return process_name;
}

// Returns the creation time of the process.
std::string Process::getTime() const
{
    return creation_time;
}

// Generates a random number of print commands for the process.
void Process::generatePrintCommands(int min_instructions, int max_instructions)
{
    // Seed the random number generator using the current time and process ID.
    std::srand(static_cast<unsigned int>(std::time(nullptr)) + process_id);

    // Generate a random number of commands within the specified range.
    int num_commands = min_instructions + (std::rand() % (max_instructions - min_instructions + 1));

    // Create the print commands and add them to the command list.
    for (int i = 1; i <= num_commands; ++i)
    {
        // Create a new PrintCommand and add it to the command list.
        std::shared_ptr<ICommand> cmd = std::make_shared<PrintCommand>(
            process_id, core_id, "Hello World From " + process_name + " started.", process_name);
        command_list.push_back(cmd);
    }
}
