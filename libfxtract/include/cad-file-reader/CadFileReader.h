#pragma once

#include "../../include/sheet-metal-component/SheetMetal.h"
#include "UnknownFileFormatException.h"

#include <memory>
#include <string>
#include <XSControl_Reader.hxx>

class XSControl_Reader;

namespace Fxt 
{
namespace CadFileReader
{
    class CadFileReader 
    {
    protected:
        virtual void makeReader(const std::string& filename) = 0;
    public:
        virtual void extractFaces(SheetMetalPtr& model, const std::string& filename) = 0;
        virtual bool isUsable() const noexcept { return true; }
    };
}
}

using CadFileReaderPtr =  std::shared_ptr<Fxt::CadFileReader::CadFileReader>;