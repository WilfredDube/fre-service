#include "Mapper.h"
#include "contracts/FeatureRecognitionStarted.h"
#include "contracts/ProcessPlanningStarted.h"

#include <nlohmann/json.hpp>

#include <iostream>

using json = nlohmann::json;

EventPtr EventMapper::MapEvent(std::string eventName, std::string serialized, int size)
{
    EventPtr event;

    if (eventName == "featureRecognitionStarted")
    {
        std::string res = serialized.substr(0, size);
        auto FREevent = std::make_shared<FeatureRecognitionStarted>();
        FREevent->createEvent(res);
        event = FREevent;
        event->eventType = EventType::FEATURE_REC_START;
    }
    else if (eventName == "processPlanningStarted")
    {
        std::string res = serialized.substr(0, size);
        auto ppEvent = std::make_shared<ProcessPlanningStarted>();
        ppEvent->createEvent(res);
        event = ppEvent;
        event->eventType = EventType::PROCESS_PLAN_START;
    }
    else
    {
        std::cerr << "unknown event type" << eventName << std::endl;
        return std::make_shared<NullEvent>();
    }

    return event;
}

std::string EventMapper::GetEventType(std::string data)
{
    auto j3 = json::parse(data);

    std::string eventName = j3["event_type"];

    return eventName;
}
