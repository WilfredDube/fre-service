#pragma once

#include <exception>
#include <string>
#include <string_view>

namespace Fxt 
{
namespace CadFileReader
{
    class UnknownFileFormatException : public std::exception 
    {
        std::string mMessage;
    public:
        UnknownFileFormatException(const std::string message) : mMessage(message){}
        std::string_view what(){ return mMessage; }
    };
}
}