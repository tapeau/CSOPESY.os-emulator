#ifndef PRINT_COMMAND_H
#define PRINT_COMMAND_H

#include "ICommand.h"

#include <ctime>
#include <string>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <chrono>

/**
 * @class PrintCommand
 * @brief Implements a print command that outputs specified text along with process information to a file.
 *
 * This class is derived from ICommand and represents a specific type of command that logs messages
 * to a file with a timestamp and core ID.
 */
class PrintCommand : public ICommand
{
public:
    /**
     * @brief Constructor for PrintCommand.
     * @param pid The process ID associated with this command.
     * @param core The core ID on which the command is executed.
     * @param to_print The message to print.
     * @param name The name of the output file.
     */
    PrintCommand(int pid, int core, const std::string& to_print, const std::string& name)
        : ICommand(pid, CommandType::PRINT), core_(core), to_print_(to_print), name_(name)
    {
    }

    /**
     * @brief Execute the print command.
     *
     * This function writes the message, core ID, and timestamp to an output file.
     */
    void execute() override
    {
        std::ofstream outfile(name_ + ".txt", std::ios::app);
        outfile << getCurrentTimestamp() << " Core:" << core_ << " \"" << to_print_ << "\"" << std::endl;
        outfile.close();
    }

    /**
     * @brief Set the core ID for the command.
     * @param core The core ID to be set.
     */
    void setCore(int core) override
    {
        core_ = core;
    }

private:
    std::string to_print_;  ///< The message to be printed.
    int core_;              ///< The core ID on which the command is executed.
    std::string name_;      ///< The name of the output file.

    /**
     * @brief Get the current timestamp.
     * @return A string representing the current timestamp.
     */
    std::string getCurrentTimestamp()
    {
        auto now = std::chrono::system_clock::now();
        std::time_t time_now = std::chrono::system_clock::to_time_t(now);
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
        std::tm local_time;
        localtime_s(&local_time, &time_now);

        std::ostringstream oss;
        oss << std::put_time(&local_time, "(%m/%d/%Y %I:%M:%S")
            << '.' << std::setfill('0') << std::setw(3) << milliseconds.count()
            << std::put_time(&local_time, "%p)");
        return oss.str();
    }
};

#endif
