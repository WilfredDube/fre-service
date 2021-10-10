#pragma once

#include "../../msgqueue/Event.h"
#include "../../logging/include/StandardOutputLogger.h"

std::shared_ptr<Event> ProcessCadFile(EventPtr event, Logger loggingService);
