#include <string>
#include <iostream>
#include "ConsoleManager.h"
#include "ConsoleScreen.h"
#include "MemoryManager.h"

int main()
{
    ConsoleManager console_manager;
    ConsoleScreen console_screen;
    std::string user_command;

    MemoryManager::initialize();

    system("cls"); // Clear the screen
    console_screen.printHeader(); // Display the header

    // Infinite loop for continuous user_command polling
    while (true)
    {
        std::cout << "Enter a command: ";
        std::getline(std::cin, user_command);
        console_manager.processCommand(user_command);
        
        if (user_command == "exit")
        {
            break;
        }
    }

    MemoryManager::destroy();

    return 0;
}
