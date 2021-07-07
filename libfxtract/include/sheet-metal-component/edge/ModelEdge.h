#pragma once

#include "../ModelTypes.h"

#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepLProp_SLProps.hxx>
#include <GeomConvert.hxx>
#include <memory>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopExp_Explorer.hxx>
#include <vector>

namespace Fxt
{
namespace SheetMetalComponent
{
namespace Edge
{
    class ModelEdge
    {
    public:
        enum class EdgePosition 
        { 
            DEFAULT = -1, 
            JOINING_EDGE, 
            SIDE_EDGE, 
            DISJOINT_EDGE 
        };

        enum class EdgeType { 
            DEFAULT = -1, 
            LINE, 
            ARC 
        };

        ModelEdge() = default;
        ModelEdge(std::shared_ptr<TopoDS_Edge> edge);

        std::shared_ptr<TopoDS_Edge> getTModelEdge() const;

        std::vector<std::shared_ptr<gp_Pnt>> getEdgeEndPoints() const;

        void setIsRational(bool isRational);
        bool IsRational() const;

        void setEdgeNum(const EdgeID edgeID);
        EdgeID getEdgeNum() const;

        void setEdgeType(const GeomAbs_CurveType type);

        void setEdgePosition(const EdgePosition edgePosition);

        void setEdgeLength(const long double edgeLength);
        long double getEdgeLength() const;

        void setEdgeLineVector();
        gp_DirPtr getEdgeLineVector();

        void printEdgePosition();

        void setJoiningFaceID(const FaceID id);
        FaceID getJoiningFaceID();

        bool isLine() const;
        bool isArc() const;

        bool isJoiningEdge() const;
        bool isSideEdge() const;
        bool isDisjointEdge() const;

        bool operator==(const ModelEdge& otherEdge) const;

    private:
        EdgeID mEdgeID;
        EdgeType mEdgeType;
        EdgePosition mEdgePosition;

        std::shared_ptr<gp_Dir> mEdgeLineVector;

        long double mEdgeLength;

        FaceID mJoinedToFaceID;

        //! Whether its a straight line (rational = 0) or arc (rational = 1)
        bool mIsrational;

        //! Edge ID of an edge in the Model shared by another face.
        EdgeID mEdgeSameAsEdgeID;

        std::shared_ptr<TopoDS_Edge> mModelEdge;
    };
}
}
}

using ModelEdgePtr = std::shared_ptr<Fxt::SheetMetalComponent::Edge::ModelEdge>;