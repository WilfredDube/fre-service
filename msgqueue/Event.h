#pragma once

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <memory>

enum class EventType
{
    FEATURE_REC_START,
    FEACTURE_REC_COMPLETE,
    PROCESS_PLAN_START,
    PROCESS_PLAN_COMPLETE
};

struct Event
{
    double totalTime;
    EventType eventType;
    std::shared_ptr<void> pData;

    virtual std::string EventName() = 0;
    virtual json toJson() = 0;
};

struct NullEvent : public Event
{
    std::string EventName() { return ""; }

    json toJson()
    {
        json jsonObject;
        return jsonObject;
    }
};

using NullEventPtr = std::shared_ptr<NullEvent>;
using EventPtr = std::shared_ptr<Event>;
