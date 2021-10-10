#include "../include/StandardOutputLogger.h"

#include <iostream>

using namespace Fxt::Logging;

void StandardOutputLogger::writeInfoEntry(std::string_view caller, int line, std::string_view message)
{
    std::cout << "INFO: " << formatMessage(caller, line, message) << '\n';
}

void StandardOutputLogger::writeWarnEntry(std::string_view caller, int line, std::string_view message)
{
    std::cout << "WARN: " << formatMessage(caller, line, message) << '\n';
}

void StandardOutputLogger::writeErrorEntry(std::string_view caller, int line, std::string_view message)
{
    std::cout << "ERROR: " << formatMessage(caller, line, message) << '\n';
}