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

    userID = j3["user_id"];
    cadFileID = j3["cadfile_id"];
    serializedData = j3["serialized_data"];
    bendCount = j3["bend_count"];
    taskID = j3["task_id"];
}

json ProcessPlanningStarted::toJson()
{
    json jsonObject;
    return jsonObject;
}
