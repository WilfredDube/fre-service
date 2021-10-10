#pragma once

#include "LoggingFacility.h"

namespace Fxt
{
    namespace Logging
    {
        class StandardOutputLogger : public ILoggingFacility
        {
        public:
            StandardOutputLogger(std::string_view service) : ILoggingFacility(service) {}

            void writeInfoEntry(std::string_view caller, int line, std::string_view message) override;
            void writeWarnEntry(std::string_view caller, int line, std::string_view message) override;
            void writeErrorEntry(std::string_view caller, int line, std::string_view message) override;
        };
    }
}