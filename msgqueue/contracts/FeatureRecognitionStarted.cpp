#include "FeatureRecognitionStarted.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::string FeatureRecognitionStarted::EventName()
{
    return "featureRecognitionStarted";
};

void FeatureRecognitionStarted::createEvent(std::string data)
{
    auto j3 = json::parse(data);

    userID = j3["user_id"];
    cadFileID = j3["cadfile_id"];
    URL = j3["url"];
    taskID = j3["task_id"];
}

json FeatureRecognitionStarted::toJson()
{
    json jsonObject;
    return jsonObject;
}
