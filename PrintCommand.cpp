#include "PrintCommand.h"
#include <ctime>     

using namespace std;

void PrintCommand::execute()
{
  // Open the output file in append mode using the process name as the filename.
  std::ofstream outfile(process_name + ".txt", std::ios::app);

  // Write the timestamp, core ID, and text to the file.
  outfile << getCurrentTimestamp() << " Core:" << core_id << " \"" << text_to_print << "\"" << std::endl;

  // Close the output file.
  outfile.close();
};
/**
 * Override the setCore() function to change the core ID of this process.
 * 
 * @param core_id - The new core ID to be set.
 */
void PrintCommand::setCore(int core_id)
{
  this->core_id = core_id;  // Update the core ID with the new value.
};

/**
 * Helper function to get the current timestamp in a formatted string.
 * 
 * @return A string representing the current date and time.
 */
std::string PrintCommand::getCurrentTimestamp()
{
  // Get the current system time with high precision.
  auto now = std::chrono::system_clock::now();

  // Convert to time_t to extract the calendar time.
  std::time_t time_now = std::chrono::system_clock::to_time_t(now);

  // Extract the milliseconds part from the time point.
  auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

  std::tm local_time;  // Structure to store local time.

  // Convert time to local time safely using localtime_s.
  localtime_s(&local_time, &time_now);

  // Use a string stream to format the timestamp.
  std::ostringstream oss;
  oss << std::put_time(&local_time, "(%m/%d/%Y %I:%M:%S")
    << '.' << std::setfill('0') << std::setw(3) << milliseconds.count()  // Append milliseconds.
    << std::put_time(&local_time, "%p)");

  return oss.str();  // Return the formatted timestamp.
};
