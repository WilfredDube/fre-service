#pragma once

#include "../Event.h"

#include <vector>

typedef int FaceID;

struct ProcessingPlan
{
    std::string cadFileID;
    int rotations, flips, tools, modules, quantity;
    double processingTime, totalToolDistance, Radius, Direction;
};

struct ProcessPlanningComplete : public Event
{
    std::string userID, cadFileID, taskID;
    int processLevel;
    ProcessingPlan processingPlan;
    std::vector<FaceID> bendSequence;

    std::string EventName();
    json toJson();
};