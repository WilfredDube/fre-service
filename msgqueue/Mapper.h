#pragma once

#include <string>
#include "Event.h"

class EventMapper
{
public:
    EventPtr MapEvent(std::string eventName, std::string serialized, int size);
    std::string GetEventType(std::string data);
};