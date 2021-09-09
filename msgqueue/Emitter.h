#pragma once

#include "Event.h"
#include <string>

class EventEmitter
{
public:
    virtual bool Emit(std::shared_ptr<Event> event) = 0;
};