#pragma once

#include "../Event.h"

struct FeatureRecognitionStarted : public Event
{
    std::string userID, cadFileID, URL;

    std::string EventName();
    void createEvent(std::string data);
    json toJson();
};