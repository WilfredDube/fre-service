#include "ProcessPlanningStarted.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::string ProcessPlanningStarted::EventName()
{
    return "processPlanningStarted";
};

void ProcessPlanningStarted::createEvent(std::string data)
{
    auto j3 = json::parse(data);

    this->userID = j3["user_id"];
    this->cadFileID = j3["cadfile_id"];
    this->serializedData = j3["serialized_data"];
    this->bendCount = j3["bend_count"];
}

json ProcessPlanningStarted::toJson()
{
    json jsonObject;
    return jsonObject;
}
