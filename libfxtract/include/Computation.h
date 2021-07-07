#pragma once

#include "./sheet-metal-component/face/ModelFace.h"
#include "./sheet-metal-component/edge/ModelEdge.h"

#include <cmath>
#include <memory>
#include <Precision.hxx>
#include <vector>

namespace Fxt 
{
namespace Computation 
{

  /*
  *   Prints a vertex
  */
  void print(const std::shared_ptr<gp_Pnt>& vertex);

  /**
  *   Rounds of the value to 2 decimal places to avoid error in floating point values
  *   such as getting 89.999999999999999999999999 instead of 90.
  */
  long double roundd(long double value);

  /**
  *   Computes the unit normal vector of a Face.
  */
  std::shared_ptr<gp_Dir> computeUnitNormal(const std::shared_ptr<TopoDS_Face>& face);

  std::shared_ptr<gp_Dir> computeNormal(const std::vector<std::shared_ptr<Fxt::SheetMetalComponent::Edge::ModelEdge>>& modelEdges);

  gp_Pnt computeLineUnitVector(const gp_Pnt& lineVector);

  /**
  *   Computes the curvature of a ModelFace and ModelBend.
  */
  template<typename T>
  Standard_Real computeCurvature(const std::shared_ptr<T>& face)
  {
    BRepAdaptor_Surface surface = BRepAdaptor_Surface(*face);
    double u = (surface.FirstUParameter() + surface.LastUParameter()) / 2.0;
    double v = (surface.FirstVParameter() + surface.LastVParameter()) / 2.0;

    BRepLProp_SLProps surfaceProps(surface, u, v, 2, gp::Resolution());

    return surfaceProps.MeanCurvature();
  }

  /**
  *   Computes the equation of a plane.
  *   @param modelEdges are the edges of a plane (ModelFace) and its normal.
  */
  Standard_Real computePlaneEquation(
        const std::vector<std::shared_ptr<Fxt::SheetMetalComponent::Edge::ModelEdge>>& modelEdges, 
        const std::shared_ptr<gp_Pnt>& faceNormal
  );

  /**
  *   Tests whether two vertices v1 and v2 are equal.
  */
  bool isEqual(const std::shared_ptr<gp_Pnt>& aVertex, const std::shared_ptr<gp_Pnt>& bVertex);

  Standard_Real computeEuclideanNorm(const gp_Pnt& vertex);
  gp_Pnt computeCrossProduct(const gp_Pnt& aVector, const gp_Pnt& bVector);
  Standard_Real computeDotProduct(const gp_Pnt& aVertex, const gp_Pnt& bVertex);

  /*
  * Computes the directional vector of an edge.
  */
  gp_Pnt computeLineVector(const Fxt::SheetMetalComponent::Edge::ModelEdge* edge);

  /*
  *   Computes the length of an edge.
  */
  Standard_Real computeLength(const Fxt::SheetMetalComponent::Edge::ModelEdge& edge);

  /*
  *   Computes the thickness of the entire model.
  *   @param face is a face with FaceType::THICKNESS_DEFINING_FACE attribute.
  */
  Standard_Real computeThickness(const std::shared_ptr<Fxt::SheetMetalComponent::Face::ModelFace>& face);

  /*
  *   Computes the angle between two faces.
  *   @param n1 and n2 are normals of the two faces.
  */
  Standard_Real computeAngle(const std::shared_ptr<gp_Pnt>&  aFaceNormal, const std::shared_ptr<gp_Pnt>&  bFaceNormal);

  /*
  *   Computer the midpoint of 2 points
  * */
  gp_Pnt computeMidPoint(const std::shared_ptr<gp_Pnt>& p1, const std::shared_ptr<gp_Pnt>& p2);

  /*
  *   Compute the factorial
  * */
  size_t computeFactorial(const size_t nBends);

}
}