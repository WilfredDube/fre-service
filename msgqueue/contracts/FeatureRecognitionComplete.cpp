#include "FeatureRecognitionComplete.h"

#include "../../libfxtract/include/sheet-metal-component/BoostSerializer.h"
#include "../../libfxtract/include/cad-file-reader/CadFileReader.h"

#include <iostream>

std::string FeatureRecognitionComplete::EventName()
{
    return "featureRecognitionComplete";
};

json FeatureRecognitionComplete::toJson()
{
    json jsonObject;
    // auto featureMap = std::static_pointer_cast<std::map<Fxt::SheetMetalComponent::FaceID, std::shared_ptr<Fxt::SheetMetalComponent::Bend::ModelBend>>>(pData);

    std::vector<json> features;

    for (auto &bend : bendFeatures)
    {
        json jsonFeature = {
            {"bend_id", bend->getFaceId()},
            {"first_face_id", bend->getBendFeature()->getJoiningFaceID1()},
            {"second_face_id", bend->getBendFeature()->getJoiningFaceID2()},
            {"angle", bend->getBendFeature()->getBendAngle()},
            {"length", bend->getBendFeature()->getBendLength()},
            {"radius", bend->getBendFeature()->getBendRadius()},
            {"direction", bend->getBendFeature()->getBendDirection()}};

        features.push_back(jsonFeature);
    }

    jsonObject["features"] = features;

    json jprops = {
        {"serial_data", featureProps.serializedData},
        {"thickness", featureProps.Thickness},
        {"fre_time", totalTime},
        {"bending_force", -1},
        {"process_level", 1},
        {"bend_count", bendFeatures.size()}};

    jsonObject["feature_props"] = jprops;
    jsonObject["user_id"] = userID;
    jsonObject["cadfile_id"] = cadFileID;

    return jsonObject;
}