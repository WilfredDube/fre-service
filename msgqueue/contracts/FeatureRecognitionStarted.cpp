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

    this->userID = j3["user_id"];
    this->cadFileID = j3["cadfile_id"];
    this->URL = j3["url"];
}

json FeatureRecognitionStarted::toJson()
{
    json jsonObject;
    return jsonObject;
}
