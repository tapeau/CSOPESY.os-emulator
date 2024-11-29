#include "ConsoleManager.h"
#include "ConsoleScreen.h"

#include <map>
#include <ctime>
#include <string>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <iostream>

/**
 * @brief Main entry point for the Console Manager application.
 *
 * This program is a command-line interface that allows users to manage and interact with
 * various processes through commands. It uses ConsoleManager and ConsoleScreen classes
 * to execute commands, create sessions, display screens, and more.
 *
 * The program runs in an infinite loop, asking the user for commands until the "exit" command
 * is entered, at which point it breaks the loop and terminates.
 *
 * @return 0 on successful execution.
 */
int main()
{
    // Create instances of ConsoleManager and ConsoleScreen
    ConsoleManager manager;
    ConsoleScreen screen_manager;
    std::string command;

    // Clear the console screen and display the welcome header
    system("cls");
    screen_manager.displayHeader();

    // Command loop
    while (true)
    {
        std::cout << "Enter a command: ";
        std::getline(std::cin, command);

        // If the command is "exit", handle it and break the loop to end the program
        if (command == "exit")
        {
            manager.handleCommand(command);
            break;
        }

        // Handle other commands
        manager.handleCommand(command);
    }

    return 0;
}
