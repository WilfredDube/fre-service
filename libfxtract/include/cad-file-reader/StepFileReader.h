#pragma once

#include "CadFileReader.h"

class STEPControl_Reader;

namespace Fxt 
{
namespace CadFileReader
{
    class StepFileReader : public CadFileReader 
    {
        std::shared_ptr<STEPControl_Reader> mMyStepReader { nullptr };
    public:
        void makeReader(const std::string& filename);
        void extractFaces(SheetMetalPtr& model, const std::string& filename);
    };
}
}