#include "ConsoleScreen.h"
#include "CoreStateManager.h"

// Console color codes
const char RESET[] = "\033[0m";
const char CYAN[]  = "\033[36m";
const char GREEN[] = "\033[32m";
const char PINK[]  = "\e[38;5;212m";

// Display the application header and welcome message
void ConsoleScreen::displayHeader() const
{
    std::cerr << 
    "  ____ ____   ___  ____  _____ ______   __\n"
    " / ___/ ___| / _ \\|  _ \\| ____/ ___\\ \\ / /\n"
    "| |   \\___ \\| | | | |_) |  _| \\___ \\\\ V / \n"
    "| |___ ___) | |_| |  __/| |___ ___) || |\n"
    " \\____|____/ \\___/|_|   |_____|____/ |_|\n\n"
    << GREEN << "Hello. Welcome to the CSOPESY Command Line!\n"
    << "Type 'exit' to quit, 'clear' to clear the screen" 
    << RESET << std::endl;
}

// Display all processes in the system
void ConsoleScreen::displayAllProcesses(
    const std::map<std::string, std::shared_ptr<Process>>& process_list, int core_count)
{
    displayProcessesToStream(process_list, core_count, std::cout);
}

// Helper function to group and display processes to a given stream
void ConsoleScreen::displayProcessesToStream(
    const std::map<std::string, std::shared_ptr<Process>>& process_list, 
    int core_count, std::ostream& out)
{
    if (process_list.empty())
    {
        out << "No screens available.\n";
        return;
    }

    // Track ready, running, and finished process outputs
    std::stringstream ready_stream, running_stream, finished_stream;
    int core_usage = std::count(CoreStateManager::getInstance().getCoreStates().begin(), 
                                CoreStateManager::getInstance().getCoreStates().end(), true);

    // Print CPU and core usage
    out << "CPU utilization: " << (double(core_usage) / core_count) * 100 << "%\n"
        << "Cores used: " << core_usage << "\n"
        << "Cores available: " << core_count - core_usage << "\n"
        << "------------------------------------------------\n";

    // Categorize processes by their state and format their output
    for (const auto& [name, process] : process_list)
    {
        std::stringstream info;
        info << std::left << std::setw(30) << process->getName() 
             << " (" << process->getTime() << ") ";

        if (process->getState() == Process::READY)
        {
            ready_stream << info.str() 
                         << "  READY   " 
                         << process->getCommandCounter() << " / " 
                         << process->getLinesOfCode() << '\n';
        }
        else if (process->getState() == Process::RUNNING)
        {
            running_stream << info.str() 
                           << "  Core: " << process->getCPUCoreID() << "   "
                           << process->getCommandCounter() << " / " 
                           << process->getLinesOfCode() << '\n';
        }
        else
        {
            finished_stream << info.str() 
                            << "  FINISHED   " 
                            << process->getCommandCounter() << " / " 
                            << process->getLinesOfCode() << '\n';
        }
    }

    // Display categorized process streams
    out << "Ready Processes:\n" << ready_stream.str()
        << "==========================================\n"
        << "Running Processes:\n" << running_stream.str()
        << "==========================================\n"
        << "Finished Processes:\n" << finished_stream.str()
        << "------------------------------------------------\n";
}

// Display a single process's details (updated view)
void ConsoleScreen::displayUpdatedProcess(const std::shared_ptr<Process>& process)
{
    std::cout << CYAN << "Screen: " << process->getName() << RESET << '\n';

    if (process->getState() == Process::RUNNING)
    {
        std::cout << "Current instruction line: " << process->getCommandCounter() 
                  << "\nLines of code: " << process->getLinesOfCode() << '\n';
    }
    else
    {
        std::cout << "Finished!\n";
    }
    std::cout << std::endl;
}

// Allow interaction with a specific process
void ConsoleScreen::displaySingleProcess(const std::shared_ptr<Process>& process)
{
    std::cout << CYAN << "Screen: " << process->getName() << RESET << '\n'
              << "Instruction: Line " << process->getCommandCounter() 
              << " / " << process->getLinesOfCode() << '\n'
              << "Created at: " << process->getTime() << '\n'
              << "Type 'exit' to return to the main menu.\n";

    std::string command;
    while (true)
    {
        std::cout << "Enter a command: ";
        std::getline(std::cin, command);

        if (command == "process-smi")
        {
            displayUpdatedProcess(process);
        }
        else if (command == "exit")
        {
            system("cls");
            displayHeader();
            break;
        }
        else
        {
            std::cout << "Unknown command. Please try again.\n";
        }
    }
}

// Get the current timestamp in a formatted string
std::string ConsoleScreen::getCurrentTimestamp()
{
    std::time_t now = std::time(nullptr);
    std::tm local_time;
    localtime_s(&local_time, &now); // Thread-safe time conversion
    std::ostringstream oss;
    oss << std::put_time(&local_time, "%m/%d/%Y, %I:%M:%S %p");
    return oss.str();
}
