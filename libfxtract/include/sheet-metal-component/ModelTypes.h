#pragma once

#include <memory>

namespace Fxt
{
namespace SheetMetalComponent
{
namespace ModelTypes
{
    using FaceID = unsigned int;
    using EdgeID = unsigned int;

    enum class FaceType { NONE, THICKNESS_DEFINING_FACE, BEND_FACE, BEND_SIDE, FACE };

    enum class BendOrientation { PARALLEL,  PERPENDICULAR };
}
}
}

using FaceID = Fxt::SheetMetalComponent::ModelTypes::FaceID;
using EdgeID = Fxt::SheetMetalComponent::ModelTypes::EdgeID;
using FaceType = Fxt::SheetMetalComponent::ModelTypes::FaceType;

class TopoDS_Face;
class TopoDS_Edge;
class gp_Dir;

using TopoDS_FacePtr = std::shared_ptr<TopoDS_Face>;
using TopoDS_EdgePtr = std::shared_ptr<TopoDS_Edge>;
using gp_DirPtr = std::shared_ptr<gp_Dir>;
