#include "ConsoleScreen.h"
#include "CoreStateManager.h"
#include <windows.h>

// ANSI escape sequences for terminal text coloring.
const char DEFAULT[] = "\033[0m";  // Resets the color back to default.
const char BLUE[] = "\033[36m";    // Cyan/Blue-colored text.
const char GREEN[] = "\033[32m";   // Green-colored text.


/*
 * Gonna use this for demo since in Linux cannot 
 */
void ConsoleScreen::printHeaderUsingHandle() const
{
    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    std::cerr << 
    "  ____ ____   ___  ____  _____ ______   __\n"
    " / ___/ ___| / _ \\|  _ \\| ____/ ___\\ \\ / /\n"
    "| |   \\___ \\| | | | |_) |  _| \\___ \\\\ V / \n"
    "| |___ ___) | |_| |  __/| |___ ___) || |\n"
    " \\____|____/ \\___/|_|   |_____|____/ |_|\n\n"
    "------------------------------------------\n";
    SetConsoleTextAttribute(console_handle, 10);
    std::cerr << "Hello. Welcome to the CSOPESY Command Line!\n\n"
    << "Developers:\n" << "Alamay, Carl Justine\n" <<"Ang, Czarina Damienne\n" 
    << "Culanag, Saimon Russel\n" << "Tapia, John Lorenzo\n"
    << "\n"
    << "Last updated: 11-04-2024\n";
    SetConsoleTextAttribute(console_handle, 7);
    std::cerr << "------------------------------------------\n";
    SetConsoleTextAttribute(console_handle, 14);
    std::cerr << "Type 'exit' to quit, 'clear' to clear the screen \n"; 
    SetConsoleTextAttribute(console_handle, 7);
}
/**
 * Displays the header information at the start of the program.
 * This includes a welcome banner and some instructions for the user.
 */
void ConsoleScreen::printHeader()
{
    std::cerr << 
    "  ____ ____   ___  ____  _____ ______   __\n"
    " / ___/ ___| / _ \\|  _ \\| ____/ ___\\ \\ / /\n"
    "| |   \\___ \\| | | | |_) |  _| \\___ \\\\ V / \n"
    "| |___ ___) | |_| |  __/| |___ ___) || |\n"
    " \\____|____/ \\___/|_|   |_____|____/ |_|\n\n"
    "------------------------------------------\n"
    << GREEN << "Hello. Welcome to the CSOPESY Command Line!\n\n"
    << "Developers:\n" << "Alamay, Carl Justine\n" <<"Ang, Czarina Damienne\n" 
    << "Culanag, Saimon Russel\n" << "Tapia, John Lorenzo\n"
    << "\n"
    << "Last updated: 11-04-2024\n"
    << DEFAULT << "------------------------------------------\n"
    << "Type 'exit' to quit, 'clear' to clear the screen" 
    << DEFAULT << std::endl;
}

/**
 * Displays a summary of all processes by printing them to the standard output.
 * @param process_list Map containing processes identified by their name.
 * @param cpu_count Number of CPU cores available.
 */
void ConsoleScreen::showAllProcesses(std::map<std::string, std::shared_ptr<Process>> process_list, int cpu_count)
{
    // Calls the streamAllProcesses method with std::cout as the output stream.
    streamAllProcesses(process_list, cpu_count, std::cout);
}

/**
 * Streams the details of all processes to the given output stream.
 * This function organizes processes by their state (READY, RUNNING, FINISHED)
 * and displays CPU core utilization.
 * 
 * @param process_list Map of processes identified by their names.
 * @param cpu_count Total number of CPU cores available.
 * @param out The output stream where the processes will be displayed.
 */
void ConsoleScreen::streamAllProcesses(std::map<std::string, std::shared_ptr<Process>> process_list, int cpu_count, std::ostream& out)
{
    static std::mutex process_list_mutex;  // Mutex for thread safety

    // Lock the mutex for the scope of this function
    std::lock_guard<std::mutex> lock(process_list_mutex);
    
    // If no processes are available, print a message and exit early.
    if (process_list.empty())
    {
        out << "There are no active screens." << std::endl;
        return;
    }

    // Buffers to store formatted output for different process states.
    std::stringstream ready, running, finished;
    int core_usage = 0;  // Tracks the number of CPU cores currently used.

    // Retrive each process in the manager
    const std::vector<std::string>& run = CoreStateManager::getInstance().getProcesses();

    // Retrieve the state of each core from CoreStateManager (singleton).
    const std::vector<bool>& core_states = CoreStateManager::getInstance().getCoreStates();

    // Count the number of active cores (core_usage).
    for (bool core_state : core_states)
    {
        if (core_state)
        {
            core_usage++;
        }
    }

    // Begin displaying process information.
    out << "Existing Screens:" << std::endl;

    // Iterate over all processes in the map.
    for (const auto &pair : process_list)
    {
        const std::shared_ptr<Process> process = pair.second;  // Retrieve process object.

        // Format each process into a string with details like name, state, and progress.
        std::stringstream temp;
        temp << std::left << std::setw(30) << process->getName() 
             << " (" << process->getTime() << ") ";

        // Check the process state and format accordingly.
        // if (process->getState() == Process::READY)
        // {
        //     temp << "  READY   "
        //          << process->getCommandCounter() << " / " 
        //          << process->getLinesOfCode() << std::endl;
        //     ready << temp.str() << std::endl;  // Store in ready processes.
        // }
        if (process->getState() == Process::RUNNING)
        {
            temp << "  Core: " << process->getCPUCoreID() << "   "
                 << process->getCommandCounter() << " / " 
                 << process->getLinesOfCode() << std::endl;
            running << temp.str() << std::endl;  // Store in running processes.
        }
        else if (process->getState() == Process::WAITING)
        {
            temp << "  WAITING   "
                 << process->getCommandCounter() << " / " 
                 << process->getLinesOfCode() << std::endl;
            running << temp.str() << std::endl;  // Store in running processes.
        }
        else if (process->getState() == Process::FINISHED)
        {
            temp << "  FINISHED   "
                 << process->getCommandCounter() << " / " 
                 << process->getLinesOfCode() << std::endl;
            finished << temp.str() << std::endl;  // Store in finished processes.
        }
    }

    // Display CPU core utilization information.
    out << "CPU utilization: " << core_usage * 100 / cpu_count << "%\n";
    out << "Cores used: " << core_usage << "\n";
    out << "Cores available: " << cpu_count - core_usage << "\n";
    out << "------------------------------------------------\n";

    // Print each category of processes.
    // out << "Ready Processes: \n" << ready.str();
    // std::cout << "==========================================\n";
    out << "Running Processes: \n" << running.str();
    std::cout << "==========================================\n";
    out << "\nFinished Processes: \n" << finished.str();
    out << "------------------------------------------------\n";
}

/**
 * Shows the updated state of a specific process on the console.
 * If the process is running, it prints the current progress. 
 * If the process is finished, it prints a completion message.
 * 
 * @param process Shared pointer to the process object.
 */
void ConsoleScreen::showProcessUpdated(std::shared_ptr<Process> process)
{
    if (process->getState() == Process::RUNNING || process->getState() == Process::READY)
    {
        std::cout << BLUE << "Process: " << process->getName() << DEFAULT << std::endl;
        std::cout << BLUE << "ID: " << process->getName() << DEFAULT << std::endl;
        std::cout << "Current instruction line: " << process->getCommandCounter() << std::endl;
        std::cout << "Lines of code: " << process->getLinesOfCode() << std::endl;
    }
    else
    {
        std::cout << "Finished!" << std::endl;
    }
    std::cout << std::endl;
}

/**
 * Displays the details of a specific process and allows the user to 
 * interact with it by entering commands in a loop.
 * 
 * @param process Shared pointer to the process object to display.
 */
void ConsoleScreen::showScreen(std::shared_ptr<Process> process)
{
    std::cout << BLUE << "Process: " << process->getName() << DEFAULT << std::endl;
    std::cout << BLUE << "ID: " << process->getName() << DEFAULT << std::endl;
    std::cout << "Current instruction line: " << process->getCommandCounter() << std::endl;
    std::cout << "Lines of code: " << process->getLinesOfCode() << std::endl;
    std::cout << "Created at: " << process->getTime() << std::endl;
    std::cout << "Type 'exit' to return to the main menu." << std::endl;

    std::string command;

    // Loop to handle user input until the 'exit' command is entered.
    while (true)
    {
        std::cout << "Enter a command: ";
        std::getline(std::cin, command);

        if (command == "process-smi")
        {
            showProcessUpdated(process);  // Show updated process info.
        }
        else if (command == "exit")
        {
            system("cls");  // Clear the console.
            printHeaderUsingHandle();  // Reprint the header.
            break;
        }
        else
        {
            std::cout << "Unknown command. Please try again." << std::endl;
        }
    }
}

/**
 * Retrieves the current timestamp in the format "MM/DD/YYYY, HH:MM:SS AM/PM".
 * @return A string representing the current timestamp.
 */
std::string ConsoleScreen::getCurrentTimestamp()
{
    std::time_t now = std::time(nullptr);  // Get current time.
    std::tm local_time;
    localtime_s(&local_time, &now);  // Convert to local time.

    // Format the timestamp.
    std::ostringstream oss;
    oss << std::put_time(&local_time, "%m/%d/%Y, %I:%M:%S %p");
    return oss.str();
}
