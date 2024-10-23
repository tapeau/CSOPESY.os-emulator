#pragma once

#include <map>
#include <memory>
#include <string>
#include <iomanip>
#include <sstream>
#include <iostream>

class ConsoleScreen
{
public:
    void displayHeader(); // Display application header
    std::string getCurrentTimestamp(); // Fetch the current timestamp
    // TODO: Member functions to display processes

private:
    // TODO: Helper function to send process details to an output stream
};
