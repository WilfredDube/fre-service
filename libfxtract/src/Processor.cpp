#include "../include/Processor.h"

#include "../include/sheet-metal-component/BoostSerializer.h"
#include "../include/cad-file-reader/CadFileReader.h"
#include "../include/cad-file-reader/CadFileReaderFactory.h"
#include "../../logging/include/LoggingFacility.h"
#include "../../logging/include/StandardOutputLogger.h"

#include "../../msgqueue/contracts/FeatureRecognitionStarted.h"
#include "../../msgqueue/contracts/FeatureRecognitionComplete.h"

#include "../../cloud/include/cloud.h"
#include "../../cloud/include/URL.h"

std::shared_ptr<Event> ProcessCadFile(EventPtr event, Logger loggingService)
{
    int startTime = clock();

    auto cadFile = dynamic_cast<FeatureRecognitionStarted *>(event.get());
    loggingService->setLoggingID(cadFile->userID, cadFile->cadFileID);

    auto sheetMetalFeatureModel = std::make_shared<Fxt::SheetMetalComponent::SheetMetal>();
    auto cadReaderFactory = std::make_shared<Fxt::CadFileReader::CadFileReaderFactory>(loggingService);

    auto cadFileReader = cadReaderFactory->createReader(cadFile->URL.c_str());

    if (!cadFileReader->isUsable())
    {
        loggingService->writeErrorEntry(__FILE__, __LINE__, "Unknown file format : Fxtract only accepts iges and step file formats.");
        return nullptr;
    }

    URL u(cadFile->URL.c_str());
    auto blob_name = u.extractBlobName();

    std::string stepfile = "temp.stp";
    std::ofstream fout(stepfile);

    // Download file from the cloud
    auto cloudService = std::make_shared<CloudStorage>();
    fout << cloudService->downloadBlob(blob_name);

    fout.close();

    cadFileReader->extractFaces(sheetMetalFeatureModel, stepfile);
    sheetMetalFeatureModel->classifyFaces();
    sheetMetalFeatureModel->computeBendAngles();
    sheetMetalFeatureModel->assignBendDirection();

    try
    {
        sheetMetalFeatureModel->reduceModelSize();
        loggingService->writeInfoEntry(__FILE__, __LINE__, "Model resize done.......");
    }
    catch (const std::exception &e)
    {
        loggingService->writeErrorEntry(__FILE__, __LINE__, e.what());
    }

    int stopTime = clock();
    auto total_time = (stopTime - startTime) / double(CLOCKS_PER_SEC);

    auto result = std::make_shared<FeatureRecognitionComplete>();

    result->cadFileID = cadFile->cadFileID;
    result->userID = cadFile->userID;
    result->taskID = cadFile->taskID;
    result->totalTime = total_time; // Duplicate
    result->eventType = EventType::FEACTURE_REC_COMPLETE;
    auto bendFeatures = sheetMetalFeatureModel->getBends();

    for (auto &[faceID, bend] : bendFeatures)
    {
        result->bendFeatures.push_back(bend);
    }

    result->featureProps.BendCount = bendFeatures.size();
    result->featureProps.serializedData = save(sheetMetalFeatureModel);
    result->featureProps.ProcessLevel = 1;
    result->featureProps.Thickness = sheetMetalFeatureModel->getThickness();
    result->featureProps.BendingForce = -1;
    result->featureProps.FREtime = total_time;

    loggingService->writeInfoEntry(__FILE__, __LINE__, "Feature recognition complete.......");
    return result;
}
