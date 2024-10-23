#include <iostream>
#include <string>
#include <cstdlib>

#include "ConsoleManager.h"
#include "ConsoleScreen.h"

int main()
{
    ConsoleManager console_manager;   // Object to handle commands
    ConsoleScreen screen_manager;     // Object to manage screen display
    std::string user_command;         // Store user input

    system("cls");  // Clear console screen (for Windows)
    screen_manager.displayHeader();  // Display initial header or welcome message

    // Main command loop to keep taking user input until "exit" is entered
    while (true)
    {
        std::cout << "Enter a command: ";
        std::getline(std::cin, user_command);  // Read user input including spaces

        console_manager.handleCommand(user_command);  // Process the command

        // Exit the loop if the user entered "exit"
        if (user_command == "exit")
        {
            break;
        }
    }

    return 0;
}
