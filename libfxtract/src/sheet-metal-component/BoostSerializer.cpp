#include "../../include/sheet-metal-component/BoostSerializer.h"

#include "../../include/sheet-metal-component/SheetMetal.h"

std::string save(const std::shared_ptr<Fxt::SheetMetalComponent::SheetMetal>& sheetMetalFeatureModel)
{    
    std::stringstream ss;
    boost::archive::text_oarchive oa(ss);
    oa << *sheetMetalFeatureModel;

    return ss.str();
}

std::shared_ptr<Fxt::SheetMetalComponent::SheetMetal> restore(std::string restoreStr)
{
    auto restored = std::make_unique<Fxt::SheetMetalComponent::SheetMetal>();

    std::stringstream iss(restoreStr);
    boost::archive::text_iarchive oa(iss);
    oa >> *restored;

    return restored;
}