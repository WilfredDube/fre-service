#include "../../../include/sheet-metal-component/edge/ModelEdge.h"
#include "../../../include/sheet-metal-component/ModelTypes.h"
#include "../../../include/Computation.h"

using namespace Fxt::SheetMetalComponent::Edge;

ModelEdge::ModelEdge(std::shared_ptr<TopoDS_Edge> edge)
: mModelEdge { edge }
{
    mEdgePosition = EdgePosition::DEFAULT;
    
    setIsRational(BRepAdaptor_Curve(*edge).IsRational());

    BRepAdaptor_Curve curve = BRepAdaptor_Curve(*edge);
    setEdgeType(BRepAdaptor_Curve(*edge).GetType());

    setEdgeLength(Computation::computeLength(*this));
    setEdgeLineVector();
    setJoiningFaceID(0);
}

std::shared_ptr<TopoDS_Edge> ModelEdge::getTModelEdge() const
{
    return mModelEdge;
}

std::vector<std::shared_ptr<gp_Pnt>> ModelEdge::getEdgeEndPoints() const
{
    std::vector<std::shared_ptr<gp_Pnt>> endPoints;

    for (TopExp_Explorer vertexEx(*mModelEdge, TopAbs_VERTEX); vertexEx.More(); vertexEx.Next())
    {
      TopoDS_Vertex vertex = TopoDS::Vertex(vertexEx.Current());
      std::shared_ptr<gp_Pnt> currentVertex { std::make_shared<gp_Pnt>(BRep_Tool::Pnt(vertex)) };
      endPoints.push_back(currentVertex);
    }

    return endPoints;
}

void ModelEdge::setIsRational(bool isRational)
{
    mIsrational = isRational;
}

bool ModelEdge::IsRational() const
{
    return mIsrational;
}

void ModelEdge::setEdgeNum(const EdgeID edgeID)
{
    mEdgeID = edgeID;
}

EdgeID ModelEdge::getEdgeNum() const
{
    return mEdgeID;
}

void ModelEdge::setEdgeType(const GeomAbs_CurveType type)
{
    switch (type) {
      case GeomAbs_Line:
      mEdgeType = EdgeType::LINE;
      break;
      case GeomAbs_Circle:
      mEdgeType = EdgeType::ARC;
      break;
      case GeomAbs_Ellipse:
      mEdgeType = EdgeType::ARC;
      break;
      case GeomAbs_Hyperbola:
      mEdgeType = EdgeType::ARC;
      break;
      case GeomAbs_Parabola:
      mEdgeType = EdgeType::ARC;
      break;
      case GeomAbs_BezierCurve:
      mEdgeType = EdgeType::ARC;
      break;
      case GeomAbs_BSplineCurve:
      mEdgeType = EdgeType::ARC;
      if (IsRational() != true)
      {
        mEdgeType = EdgeType::LINE;
      }
      break;
      case GeomAbs_OffsetCurve:
      mEdgeType = EdgeType::ARC;
      break;
      case GeomAbs_OtherCurve:
      default:;
      mEdgeType = EdgeType::ARC;
    }
}

void ModelEdge::setEdgePosition(const EdgePosition edgePosition)
{
    mEdgePosition = edgePosition;
}

void ModelEdge::setEdgeLength(const long double edgeLength)
{
    mEdgeLength = edgeLength;
}

long double ModelEdge::getEdgeLength() const
{
    return mEdgeLength;
}

void ModelEdge::setEdgeLineVector()
{
    auto point = Computation::computeLineVector(this);
    mEdgeLineVector = std::make_shared<gp_Dir>(point.X(), point.Y(), point.Z()); 
}

std::shared_ptr<gp_Dir> ModelEdge::getEdgeLineVector()
{
    return mEdgeLineVector;
}

void ModelEdge::printEdgePosition()
{}

void ModelEdge::setJoiningFaceID(const FaceID id)
{
    mJoinedToFaceID = id;
}

FaceID ModelEdge::getJoiningFaceID()
{
    return mJoinedToFaceID;
}

bool ModelEdge::isLine() const
{
    return (mEdgeType == EdgeType::LINE);
}

bool ModelEdge::isArc() const
{
    return (mEdgeType == EdgeType::ARC);
}

bool ModelEdge::isJoiningEdge() const
{
    return (mEdgePosition == EdgePosition::JOINING_EDGE);
}

bool ModelEdge::isSideEdge() const
{
    return (mEdgePosition == EdgePosition::SIDE_EDGE);
}

bool ModelEdge::isDisjointEdge() const
{
    return (mEdgePosition == EdgePosition::DISJOINT_EDGE);
}

bool ModelEdge::operator==(const ModelEdge& otherEdge) const
{
    bool equal = false;
    const std::shared_ptr<gp_Pnt> wThisEdge = getEdgeEndPoints()[0];
    const std::shared_ptr<gp_Pnt> xThisEdge = getEdgeEndPoints()[1];
    const std::shared_ptr<gp_Pnt> yOtherEdge = otherEdge.getEdgeEndPoints()[0];
    const std::shared_ptr<gp_Pnt> zOtherEdge = otherEdge.getEdgeEndPoints()[1];    

    if ((wThisEdge->IsEqual(*yOtherEdge, Precision::Approximation())) 
        && (xThisEdge->IsEqual(*zOtherEdge, Precision::Approximation())))
    {
      equal = true;
    } 
    else if ((wThisEdge->IsEqual(*zOtherEdge, Precision::Approximation())) 
        && (xThisEdge->IsEqual(*yOtherEdge, Precision::Approximation())))
    {
      equal = true;
    }

    return equal;
}
