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
            ILoggingFacility(std::string_view service) : service{service} {}
            virtual ~ILoggingFacility() = default;

            virtual void writeInfoEntry(std::string_view caller, int line, std::string_view message) = 0;
            virtual void writeWarnEntry(std::string_view caller, int line, std::string_view message) = 0;
            virtual void writeErrorEntry(std::string_view caller, int line, std::string_view message) = 0;

            inline void setLoggingID(std::string_view userId, std::string_view cadfileId)
            {
                this->userID = userId;
                this->cadfileID = cadfileId;
            }

            inline std::string formatMessage(std::string_view caller, int line, std::string_view message) noexcept
            {
                std::ostringstream ostr;
                ostr << dateTimeString() << "; UserID: "
                     << userID << "; CADfileID: "
                     << cadfileID << "; Service: "
                     << service << "; Caller: "
                     << caller << ":" << line << "; Message: "
                     << message;

                return ostr.str();
            }

        private:
            std::string_view userID, cadfileID, service;

            inline std::string dateTimeString() noexcept
            {
                std::time_t t_t = time(nullptr); // Get current system time
                std::tm *t = localtime(&t_t);    // Convert to local tim

                char buffer[256];
                strftime(buffer, sizeof(buffer), "%c", t);

                return buffer;
            }
        };
    }
}

using Logger = std::shared_ptr<Fxt::Logging::ILoggingFacility>;