#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include "ConsoleManager.h"

// Create a new screen session, or reattach if it already exists
void ConsoleManager::createSession(const std::string& name)
{
    if (screen_sessions.count(name))
    {
        std::cout << "Screen '" << name << "' already exists. Program will now reattach to it.\n";
        return;
    }

    // Initialize a new screen session
    std::string timestamp = screen_manager.getCurrentTimestamp();
    screen_sessions[name] = { "Process-" + name, 0, 100, timestamp };

    std::cout << "Created screen: " << name << std::endl;
    system("cls");  // Clear console
    // TODO: Display new screen
}

// Generate a new session silently (without displaying status)
void ConsoleManager::generateSession(const std::string& name)
{
    if (!screen_sessions.count(name))
    {
        std::string timestamp = screen_manager.getCurrentTimestamp();
        screen_sessions[name] = { "Process-" + name, 0, 100, timestamp };
    }
}

// Display all active screens
void ConsoleManager::displayAllScreens()
{
    // TODO
}

// Save a report of all active processes to a file
void ConsoleManager::saveReport()
{
    // TODO
}

// Handle various user commands
void ConsoleManager::handleCommand(const std::string& command)
{
    static std::thread scheduler_thread;

    // Ensure initialization before executing most commands
    if (!is_initialized && command != "initialize" && command != "exit")
    {
        std::cout << "Please initialize the OS first using \"initialize\"\n";
        return;
    }

    if (command == "initialize")
    {
        system("cls");
        screen_manager.displayHeader();

        // Load configuration from file
        std::ifstream config_file("config.txt");
        if (config_file)
        {
            // TODO
        }
        else
        {
            std::cerr << "Unable to open \"config.txt\".\n";
        }
    }
    else if (command.rfind("screen -s ", 0) == 0)
    {
        createSession(command.substr(10));
    } 
    else if (command.rfind("screen -r ", 0) == 0)
    {
        // TODO
    } 
    else if (command == "screen -ls")
    {
        displayAllScreens();
    } 
    else if (command == "report-util")
    {
        saveReport();
    } 
    else if (command == "scheduler-test")
    {
        if (!scheduler_running)
        {
            scheduler_running = true;
            std::cout << "scheduler-test started.\n";
            scheduler_thread = std::thread([this]()
            {
                while (scheduler_running)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(batch_frequency));
                    generateSession("Process_" + std::to_string(screen_sessions.size()));
                }
            });
        }
        else
        {
            std::cout << "Scheduler is already running.\n";
        }
    } 
    else if (command == "scheduler-stop")
    {
        scheduler_running = false;
        if (scheduler_thread.joinable())
        {
            scheduler_thread.join();
            std::cout << "Scheduler stopped.\n";
        }
        else
        {
            std::cout << "Please start the scheduler first.\n";
        }
    } 
    else if (command == "clear")
    {
        system("cls");
        screen_manager.displayHeader();
    } 
    else if (command == "exit")
    {
        std::cout << "Program terminated.\n";
        exit(0);
    } 
    else
    {
        std::cout << "Unknown command. Please try again.\n";
    }
}
