#include "ConsoleScreen.h"
#include "CoreStateManager.h"

const char GREEN[] = "\033[32m";
const char CYAN[] = "\033[36m";
const char RESET[] = "\033[0m";

/**
 * @brief Display the header information on the console.
 */
void ConsoleScreen::displayHeader()
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
    << "Last updated: 11-29-2024\n"
    << RESET << "------------------------------------------\n"
    << "Type 'exit' to quit, 'clear' to clear the screen" 
    << RESET << std::endl;
}

/**
 * @brief Display all processes in the given map.
 * @param process_list Map of process names to process objects.
 * @param num_cpu Number of CPU cores.
 */
void ConsoleScreen::displayAllProcess(std::map<std::string, std::shared_ptr<Process>> process_list, int num_cpu)
{
    displayAllProcessToStream(process_list, num_cpu, std::cout);
}

/**
 * @brief Displays all processes and writes the information to an output stream.
 * @param process_list Map of process names to process objects.
 * @param num_cpu Number of CPU cores.
 * @param out Output stream where the data is to be written.
 */
void ConsoleScreen::displayAllProcessToStream(std::map<std::string, std::shared_ptr<Process>> process_list, int num_cpu, std::ostream& out)
{
    static std::mutex process_list_mutex;

    // Lock the mutex for the scope of this function
    std::lock_guard<std::mutex> lock(process_list_mutex);

    if (process_list.empty())
    {
        out << "No screens available." << std::endl;
        return;
    }

    std::stringstream ready;
    std::stringstream running;
    std::stringstream finished;
    int core_usage = 0;

    // Get core states from the CoreStateManager
    const std::vector<std::string>& running_processes = CoreStateManager::getInstance().getProcess();
    const std::vector<bool>& core_states = CoreStateManager::getInstance().getCoreStates();

    // Calculate core usage
    for (bool core_state : core_states)
    {
        if (core_state)
        {
            core_usage++;
        }
    }

    out << "Existing Screens:" << std::endl;
    for (const auto& pair : process_list)
    {
        const std::shared_ptr<Process> process = pair.second;

        // Construct the screen listing
        std::stringstream temp;
        temp << std::left << std::setw(30) << process->getName()
             << " (" << process->getTime() << ") ";

        if (process->getState() == Process::RUNNING)
        {
            temp << "  Core: " << process->getCPUCoreID() << "   "
                 << process->getCommandCounter() << " / "
                 << process->getLinesOfCode() << std::endl;
            running << temp.str() << std::endl;
        }
        if (process->getState() == Process::FINISHED)
        {
            temp << "  FINISHED " << "   "
                 << process->getCommandCounter() << " / "
                 << process->getLinesOfCode() << std::endl;
            finished << temp.str() << std::endl;
        }
    }

    out << "CPU utilization: " << (static_cast<double>(core_usage) / num_cpu) * 100 << "%\n";
    out << "Cores used: " << core_usage << "\n";
    out << "Cores available: " << num_cpu - core_usage << "\n";
    out << "------------------------------------------------\n";
    out << "\nRunning Processes: \n"
        << running.str();
    out << "==========================================\n";
    out << "\nFinished Processes: \n"
        << finished.str();
    out << "------------------------------------------------\n";
}

/**
 * @brief Displays updated information of a process.
 * @param process Shared pointer to the process to be updated.
 */
void ConsoleScreen::displayUpdatedProcess(std::shared_ptr<Process> process)
{
    if (process->getState() == Process::RUNNING || process->getState() == Process::READY)
    {
        std::cout << CYAN << "Screen: " << process->getName() << RESET << std::endl;
        std::cout << "Current instruction line: " << process->getCommandCounter() << std::endl;
        std::cout << "Lines of code: " << process->getLinesOfCode() << std::endl;
        std::cout << std::endl;
    }
    else
    {
        std::cout << CYAN << "Screen: " << process->getName() << RESET << std::endl;
        std::cout << "Finished!" << std::endl;
        std::cout << std::endl;
    }
}

/**
 * @brief Display a specific process on the console.
 * @param process Shared pointer to the process to be displayed.
 */
void ConsoleScreen::displayScreen(std::shared_ptr<Process> process)
{
    std::cout << CYAN << "Screen: " << process->getName() << RESET << std::endl;
    std::cout << "Instruction: Line " << process->getCommandCounter() << " / "
              << process->getLinesOfCode() << std::endl;
    std::cout << "Created at: " << process->getTime() << std::endl;
    std::cout << "Type 'exit' to return to the main menu." << std::endl;

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
            std::cout << "Unknown command. Please try again." << std::endl;
        }
    }
}

/**
 * @brief Get the current timestamp when a process is created.
 * @return Current timestamp as a formatted string.
 */
std::string ConsoleScreen::getCurrentTimestamp()
{
    std::time_t now = std::time(nullptr);
    std::tm local_time;
    localtime_s(&local_time, &now); // Use localtime_s for thread safety
    std::ostringstream oss;
    oss << std::put_time(&local_time, "%m/%d/%Y, %I:%M:%S %p");
    return oss.str();
}
