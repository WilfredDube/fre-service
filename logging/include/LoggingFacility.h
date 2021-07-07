#pragma once

#include <ctime>
#include <memory>
#include <sstream>
#include <string_view>

namespace Fxt 
{
namespace Logging
{
    class ILoggingFacility
    {
    public:
        ILoggingFacility(std::string_view username) : username { username } {}
        virtual ~ILoggingFacility() = default;
        
        virtual void writeInfoEntry(std::string_view entry, std::string_view other = "") = 0;
        virtual void writeWarnEntry(std::string_view entry, std::string_view other = "") = 0;
        virtual void writeErrorEntry(std::string_view entry, std::string_view other = "") = 0;

        inline std::string formatMessage(std::string_view entry, std::string_view other = "") noexcept
        {
            std::ostringstream ostr;
            ostr << dateTimeString() << " : { " 
                 << username << " } : [ " 
                 << other << " ] -> " 
                 << entry;

            return ostr.str();
        }

    private:
        std::string_view username;

        inline std::string dateTimeString() noexcept
        {
            std::time_t t_t = time(nullptr); // Get current system time
            std::tm* t = localtime(&t_t); // Convert to local tim

            char buffer[256];
            strftime(buffer, sizeof(buffer), "%c", t);

            return buffer;
        }
    };
}
}

using Logger = std::shared_ptr<Fxt::Logging::ILoggingFacility>;