#include "ProcessPlanningComplete.h"

std::string ProcessPlanningComplete::EventName()
{
    return "processPlanningComplete";
};

json ProcessPlanningComplete::toJson()
{
    json jsonObject;
    jsonObject["user_id"] = userID;
    jsonObject["cadfile_id"] = cadFileID;
    return jsonObject;
}
