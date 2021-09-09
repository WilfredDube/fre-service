#pragma once

#include "../Event.h"
#include "../../libfxtract/include/sheet-metal-component/bend/ModelBend.h"

#include <vector>

struct FeatureProps
{
    std::string serializedData;
    int ProcessLevel, BendCount;
    float Thickness, BenndingForce, FREtime;
};

struct BendFeatures
{
    std::string toolID;
    int bendID, firstFaceID, secondFaceID;
    float Angle, Length, Radius, Direction;
};

struct FeatureRecognitionComplete : public Event
{
    std::string userID, cadFileID, taskID;
    std::vector<std::shared_ptr<Fxt::SheetMetalComponent::Bend::ModelBend>> bendFeatures;
    FeatureProps featureProps;

    std::string EventName();
    json toJson();
};