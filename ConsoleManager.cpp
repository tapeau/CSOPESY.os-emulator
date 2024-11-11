#include <iostream>
#include <cstdlib>
#include <string>
#include "ConsoleManager.h"
#include "MemoryManager.h"

// Create a new screen session or reattach if it already exists
void ConsoleManager::createScreen(const std::string &screen_name)
{
  // Check if the screen session already exists
  if (screens.find(screen_name) != screens.end())
  {
    std::cout << "Screen \"" << screen_name << "\" already exists. Program will now reattach to it." << std::endl;
    return; // Exit if session exists
  }

  // Create a new screen with default settings
  Screen new_screen = {"Process-" + screen_name, 0, 100, screen_manager.getCurrentTimestamp()};
  screens[screen_name] = new_screen; // Store the new screen in the map

  // Add a new process associated with the screen
  process_manager->addProcess(screen_name, screen_manager.getCurrentTimestamp());

  std::cout << "Created screen: " << screen_name << std::endl;

  // Clear the console screen
  system("cls");

  // Display the newly created process on the screen
  process_manager->getProcess(screen_name);
  screen_manager.showScreen(process_manager->getProcess(screen_name));
}

/*
 * DEBUGGING PURPOSES:
 * call this after initialize if you want to simulate process execution of n processes
 */

void ConsoleManager::generateProcesses(int num_process)
{
  std::string process_prefix = "Process_";
  for (int i = 0; i < num_process; ++i) {
    process_manager->addProcess(process_prefix + std::to_string(i), screen_manager.getCurrentTimestamp());
  }
}

// Generate a new session without displaying it immediately (silently)
void ConsoleManager::createScreenSilent(const std::string &screen_name)
{
  // Check if the screen session already exists
  if (screens.find(screen_name) != screens.end())
  {
    std::cout << "Screen \"" << screen_name << "\" already exists. Program will now reattach to it." << std::endl;
    return; // Exit if session exists
  }

  // Create a new screen object and add it to the map
  Screen new_screen = {"Process-" + screen_name, 0, 100, screen_manager.getCurrentTimestamp()};
  screens[screen_name] = new_screen;

  // Register the process with the process manager
  process_manager->addProcess(screen_name, screen_manager.getCurrentTimestamp());

  // Retrieve the process (no immediate display in this function)
  process_manager->getProcess(screen_name);
}

// Display all active screens managed by the ConsoleManager
void ConsoleManager::showAllScreens()
{
  // Use the ConsoleScreen instance to display all processes with the CPU count
  screen_manager.showAllProcesses(process_manager->getAllProcesses(), cpu_count);
}

// Generate a utilization report and save it to a file
void ConsoleManager::reportUtilization()
{
  std::stringstream output; // Buffer to store the report data

  // Generate the report and store it in the stringstream
  screen_manager.streamAllProcesses(process_manager->getAllProcesses(), cpu_count, output);

  // Write the report to an external file
  std::ofstream out_file("csopesy-log.txt");
  if (out_file.is_open())
  {
    out_file << output.str(); // Write the output buffer to the file
    out_file.close();
    std::cout << "Report saved to csopesy-log.txt" << std::endl;
  }
  else
  {
    std::cerr << "Unable to open file csopesy-log.txt" << std::endl;
  }
}

// Handle user input commands and delegate to appropriate methods
void ConsoleManager::processCommand(const std::string &command)
{
  static std::thread scheduler_thread; // Thread for scheduler testing

  // Ensure the system is initialized before executing most commands
  if (!is_initialized && !(command == "exit" || command == "initialize" || command == "init"))
  {
    std::cout << "Please initialize the OS first using the \"initialize\" command." << std::endl;
    return;
  }

  // Initialize the system by loading configuration from a file
  if (command == "initialize" || command == "init")
  {
    system("cls"); // Clear console
    screen_manager.printHeader(); // Display application header

    std::ifstream config_file("config.txt"); // Open configuration file

    std::string temp; // Temporary variable to store keys in config

    if (config_file.is_open())
    {
      // Read configuration values in expected order
      config_file >> temp >> cpu_count; 
      config_file >> temp >> std::quoted(scheduler_algorithm);
      config_file >> temp >> quantum_cycles;
      config_file >> temp >> batch_process_frequency;
      config_file >> temp >> min_instructions;
      config_file >> temp >> max_instructions;
      config_file >> temp >> delays_per_execution;
      config_file >> temp >> max_overall_mem;
      config_file >> temp >> mem_per_frame;
      config_file >> temp >> mem_per_proc;

      config_file.close(); // Close the file

      // Display configuration values to the user
      std::cout << "num-cpu: " << cpu_count << std::endl;
      std::cout << "scheduler: " << scheduler_algorithm << std::endl;
      std::cout << "quantum-cycles: " << quantum_cycles << std::endl;
      std::cout << "batch-process-freq: " << batch_process_frequency << std::endl;
      std::cout << "min-ins: " << min_instructions << std::endl;
      std::cout << "max-ins: " << max_instructions << std::endl;
      std::cout << "delays-per-exec: " << delays_per_execution << std::endl;
      std::cout << "max-overall-mem: " << max_overall_mem << std::endl;
      std::cout << "mem-per-frame: " << mem_per_frame << std::endl;
      std::cout << "mem-per-proc: " << mem_per_proc << std::endl;

      // Initialize CPU clock
      cpu_clock = new Clock();
      cpu_clock->startClock();

      // Initialize the process manager with configuration parameters
      process_manager = new ProcessManager(
          min_instructions, max_instructions, cpu_count, scheduler_algorithm, delays_per_execution, quantum_cycles, cpu_clock,
          max_overall_mem, mem_per_frame, mem_per_proc
          );

      // initializes FlatMemoryAllocator in MemoryManager
      MemoryManager::getInstance().setqq(quantum_cycles);
      MemoryManager::getInstance().initAllocator(max_overall_mem);


      is_initialized = true; // Set the initialized flag
                             // Debugging Purposes (generates <param> processes)
      // generateProcesses(4);
    }
    else
    {
      std::cerr << "Unable to open file \"config.txt\"." << std::endl;
    }
  }
  // Create a new screen session with the given name
  else if (command.rfind("screen -s ", 0) == 0)
  {
    std::string session_name = command.substr(10); // Extract session name
    createScreen(session_name); // Call the session creation method
  }
  // Reattach to an existing screen session
  else if (command.rfind("screen -r ", 0) == 0)
  {
    std::string session_name = command.substr(10); // Extract session name
    std::shared_ptr<Process> process = process_manager->getProcess(session_name);

    if (process != nullptr) // Check if process exists
    {
      if (process->getState() != Process::FINISHED) // Ensure process isn't finished
      {
        system("cls"); // Clear console
        screen_manager.showScreen(process); // Display the screen
      }
      else
      {
        std::cout << "Process " << session_name << " not found." << std::endl;
      }
    }
    else
    {
      std::cout << "Process " << session_name << " not found." << std::endl;
    }
  }
  // Display all active screens
  else if (command.rfind("screen -ls", 0) == 0)
  {
    showAllScreens();
  }
  // Generate a utilization report
  else if (command == "report-util")
  {
    reportUtilization();
  }
  // Start a scheduler test in a separate thread
  else if (command == "scheduler-test")
  {
    if (!is_scheduler_running)
    {
      is_scheduler_running = true;
      std::cout << "Scheduler started." << std::endl;

      // Launch the scheduler in a new thread
      scheduler_thread = std::thread([this]()
          {
          int cpu_tick = 0;
          std::unique_lock<std::mutex> lock(cpu_clock->getMutex(), std::defer_lock);

          while (is_scheduler_running)
          {
          // Wait for each CPU tick
          lock.lock();
          cpu_clock->getCondition().wait(lock);
          lock.unlock();

          // Increment CPU tick counter on each tick
          ++cpu_tick;

          // Generate session after every batch_process_frequency ticks
          if (cpu_tick >= batch_process_frequency)
          {
          cpu_tick = 0; // Reset counter
          std::string session_name = "Process_" + std::to_string(screens.size());
          createScreenSilent(session_name);  
          }
          }
          });
    }
    else
    {
      std::cout << "Scheduler is already running." << std::endl;
    }
  }
  // Stop the running scheduler test
  else if (command == "scheduler-stop")
  {
    is_scheduler_running = false;
    if (scheduler_thread.joinable())
    {
      scheduler_thread.join(); // Wait for the thread to finish
      std::cout << "Scheduler stopped." << std::endl;
    }
    else
    {
      std::cout << "Please start the scheduler first using the \"scheduler-test\" command." << std::endl;
    }
  }
  // Clear the console
  else if (command == "clear")
  {
    system("cls");
    screen_manager.printHeader();
  }
  // Exit the application
  else if (command == "exit")
  {
    // Check first if scheduler test is running
    if (is_scheduler_running)
    {
      // Stop scheduler before terminating program
      is_scheduler_running = false;
      if (scheduler_thread.joinable())
      {
        scheduler_thread.join(); // Wait for the thread to finish
        std::cout << "Scheduler stopped." << std::endl;
      }
    }

    std::cout << "Program terminated." << std::endl;
    exit(0); // Terminate the program
  }
  // Handle unknown commands
  else
  {
    std::cout << "Unknown command. Please try again." << std::endl;
  }
}
