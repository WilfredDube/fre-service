#include "../include/Processor.h"

#include "../include/sheet-metal-component/BoostSerializer.h"
#include "../include/cad-file-reader/CadFileReader.h"
#include "../include/cad-file-reader/CadFileReaderFactory.h"
#include "../../logging/include/LoggingFacility.h"
#include "../../logging/include/StandardOutputLogger.h"

#include "../../msgqueue/contracts/FeatureRecognitionStarted.h"
#include "../../msgqueue/contracts/FeatureRecognitionComplete.h"

std::shared_ptr<Event> ProcessCadFile(EventPtr event)
{
    int startTime = clock();

    auto cadFile = dynamic_cast<FeatureRecognitionStarted *>(event.get());

    auto sheetMetalFeatureModel = std::make_shared<Fxt::SheetMetalComponent::SheetMetal>();
    auto loggingService = std::make_shared<Fxt::Logging::StandardOutputLogger>("0");
    auto cadReaderFactory = std::make_shared<Fxt::CadFileReader::CadFileReaderFactory>(loggingService);

    auto cadFileReader = cadReaderFactory->createReader(cadFile->URL.c_str());

    if (!cadFileReader->isUsable())
    {
        std::cerr << "Unknown file format : Fxtract only accepts iges and step file formats." << std::endl;
        return nullptr;
    }

    cadFileReader->extractFaces(sheetMetalFeatureModel, cadFile->URL.c_str());
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

    std::cout << sheetMetalFeatureModel << std::endl;

    return result;
}
