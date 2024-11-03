#include "ProcessManager.h"

// Constructor for ProcessManager initializes the scheduler and starts its thread.
ProcessManager::ProcessManager(int min_instructions, int max_instructions, int cpu_count, 
                               std::string scheduler_algorithm, int delays_per_execution, int quantum_cycle, Clock* cpu_clock)
{
    // Store the min and max instructions for each process.
    this->min_instructions = min_instructions;
    this->max_instructions = max_instructions;

    // Set the CPU clock object
    this->cpu_clock = cpu_clock;

    // Configure the scheduler with user-specified settings.
    scheduler.setAlgorithm(scheduler_algorithm);       // Set scheduling algorithm (e.g., FCFS, Round-Robin).
    scheduler.setDelays(delays_per_execution);         // Set delays allowed per execution.
    scheduler.setNumCPUs(cpu_count);                   // Set the number of CPUs to use.
    scheduler.setQuantumCycle(quantum_cycle);          // Set quantum cycle (for Round-Robin algorithms).
    scheduler.setClock(cpu_clock);

    // Launch the scheduler in a separate thread to manage processes asynchronously.
    scheduler_thread = std::thread(&Scheduler::start, &scheduler);
}

// Adds a new process to the manager and hands it off to the scheduler.
void ProcessManager::addProcess(string process_name, string creation_time)
{
    // Increment the process ID counter for each new process.
    process_id_counter++;

    // Create a shared pointer to a new Process object with the given name and time.
    shared_ptr<Process> process(new Process(process_id_counter, process_name, 
                                            creation_time, -1, min_instructions, max_instructions));

    // Store the process in the map using the process name as the key.
    process_list[process_name] = process;

    // Generate print commands (instructions) for the process.
    process->generatePrintCommands(min_instructions, max_instructions);

    // Add the process to the scheduler for management.
    scheduler.addProcess(process);
}

// Retrieves a specific process by its name. Returns a shared pointer to the process or nullptr if not found.
shared_ptr<Process> ProcessManager::getProcess(string process_name)
{
    // Check if the process exists in the map. If found, return it.
    if (process_list.find(process_name) != process_list.end())
    {
        return process_list[process_name];
    }
    else
    {
        // Return nullptr if the process does not exist.
        return nullptr;
    }
}

// Returns a map containing all processes managed by this ProcessManager.
map<string, std::shared_ptr<Process>> ProcessManager::getAllProcesses()
{
    return process_list;
}
