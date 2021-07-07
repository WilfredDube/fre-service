#pragma once

#include "CadFileReader.h"

class IGESControl_Reader;

namespace Fxt
{
namespace CadFileReader
{    
    class IgesFileReader : public CadFileReader 
    {
        std::shared_ptr<IGESControl_Reader> mMyIgesReader { nullptr };
    public:
        void makeReader(const std::string& filename);
        void extractFaces(SheetMetalPtr& model, const std::string& filename);
    };
}
}