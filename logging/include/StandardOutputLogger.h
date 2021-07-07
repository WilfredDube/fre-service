#pragma once

#include "LoggingFacility.h"

namespace Fxt 
{
namespace Logging
{
    class StandardOutputLogger : public ILoggingFacility
    {
    public:
        StandardOutputLogger(std::string_view username) : ILoggingFacility(username){ }

        void writeInfoEntry(std::string_view entry, std::string_view other = "") override;
        void writeWarnEntry(std::string_view entry, std::string_view other = "") override;
        void writeErrorEntry(std::string_view entry, std::string_view other = "") override;
    };
}
}