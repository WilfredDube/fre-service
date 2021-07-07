#pragma once

#include "../MFaceAbstract.h"

namespace Fxt
{
namespace SheetMetalComponent
{
namespace Face
{
    class ModelFace : public Fxt::SheetMetalComponent::MFaceAbstract
    {
    public:
        ModelFace(const FaceID faceID = 0, std::shared_ptr<TopoDS_Face> topoDSFace = nullptr);

        void init();  
    };
}
}
}