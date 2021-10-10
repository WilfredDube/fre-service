#pragma once

#include "Mapper.h"
#include "../logging/include/LoggingFacility.h"
#include "Emitter.h"

#include <string>

class EventListener
{
public:
    virtual void Listen(std::vector<std::string> &list,
                        std::function<std::shared_ptr<Event>(EventPtr event, Logger loggingService)> &) = 0;
    // virtual EventMapper Mapper() = 0;
};