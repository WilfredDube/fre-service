#include "../include/StandardOutputLogger.h"

#include <iostream>

using namespace Fxt::Logging;

void StandardOutputLogger::writeInfoEntry(std::string_view entry, std::string_view other)
{
    std::cout << "INFO: " << formatMessage(entry, other) << '\n';
}

void StandardOutputLogger::writeWarnEntry(std::string_view entry, std::string_view other)
{
    std::cout << "WARN: " << formatMessage(entry, other) << '\n';
}

void StandardOutputLogger::writeErrorEntry(std::string_view entry, std::string_view other)
{
    std::cout << "ERROR: " << formatMessage(entry, other) << '\n';
}