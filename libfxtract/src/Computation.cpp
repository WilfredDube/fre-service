#include "../include/Computation.h"

#include <stdexcept>
#include <BRepTools.hxx>
#include <GeomLProp_SLProps.hxx>

using namespace Fxt::SheetMetalComponent::Edge;

namespace Fxt 
{
namespace Computation
{
  long double roundd(long double value)
  {
    char str[40];

    sprintf(str, "%.2Lf", value);
    sscanf(str, "%Lf", &value);

    return value;
  }

  std::shared_ptr<gp_Dir> computeUnitNormal(const std::shared_ptr<TopoDS_Face>& face)
  {
    Standard_Real umin, umax, vmin, vmax;
    BRepTools::UVBounds(*face, umin, umax, vmin, vmax); // create surface
    Handle(Geom_Surface) surf=BRep_Tool::Surface(*face); // get surface properties
    GeomLProp_SLProps props(surf, umin, vmin, 1, 0.01); // get surface normal

    std::shared_ptr<gp_Dir> norm = std::make_shared<gp_Dir>(props.Normal()); // check orientation

    norm = std::make_shared<gp_Dir>(roundd(norm->X()), roundd(norm->Y()), roundd(norm->Z()));

    return norm;
  }

  std::shared_ptr<gp_Dir> computeNormal(const std::vector<std::shared_ptr<ModelEdge>>& modelEdges)
  {
    std::shared_ptr<ModelEdge> aEdge, bEdge;

    aEdge = modelEdges[0];
    for (size_t i = 0; i < modelEdges.size(); i++) {
      if (aEdge->getEdgeNum() == modelEdges[i]->getEdgeNum()) {
        continue;
      }

      // Compare edge end points if at least one is equal to one in anEdge
      // then retrieve that edge.
      if (isEqual(aEdge->getEdgeEndPoints()[0], (modelEdges[i])->getEdgeEndPoints()[0]) ||
      isEqual(aEdge->getEdgeEndPoints()[1], (modelEdges[i])->getEdgeEndPoints()[0]) ||
      isEqual(aEdge->getEdgeEndPoints()[1], (modelEdges[i])->getEdgeEndPoints()[1]) ||
      isEqual(aEdge->getEdgeEndPoints()[0], (modelEdges[i])->getEdgeEndPoints()[1])) {
        bEdge = modelEdges[i];
        break;
      }
    }

    gp_Pnt aVertex = computeLineVector(aEdge.get());
    gp_Pnt bVertex = computeLineVector(bEdge.get());

    aVertex = computeCrossProduct(aVertex, bVertex);

    return std::make_shared<gp_Dir>(aVertex.X(), aVertex.Y(), aVertex.Z());
  }

  void print(const std::shared_ptr<gp_Pnt>& vertex)
  {
    std::cout << " X : " << vertex->X() <<
                  " Y : " << vertex->Y() <<
                  " Z : " << vertex->Z() <<
                  '\n';
  }

  gp_Pnt computeLineVector(const ModelEdge* edge)
  {
    gp_Pnt vertex(
      edge->getEdgeEndPoints()[0]->X() - edge->getEdgeEndPoints()[1]->X(),
      edge->getEdgeEndPoints()[0]->Y() - edge->getEdgeEndPoints()[1]->Y(),
      edge->getEdgeEndPoints()[0]->Z() - edge->getEdgeEndPoints()[1]->Z()
    );

    return vertex;
  }

  gp_Pnt computeLineUnitVector(const gp_Pnt& lineVector)
  {
    Standard_Real magnitude = computeEuclideanNorm(lineVector);

    gp_Pnt vt (
      lineVector.X() / magnitude,
      lineVector.Y() / magnitude,
      lineVector.Z() / magnitude
    );

    return vt;
  }

  Standard_Real computePlaneEquation(
        const std::vector<std::shared_ptr<ModelEdge>>& modelEdges, 
        const std::shared_ptr<gp_Pnt>& faceNormal
  )
  {
    Standard_Real d1, d2;
    size_t count = modelEdges.size();

    for (size_t i = 0; i < count; i++) {
      std::shared_ptr<ModelEdge> edge = modelEdges[i];

      d1 = (faceNormal->X() * edge->getEdgeEndPoints()[0]->X()) + (faceNormal->Y() * edge->getEdgeEndPoints()[0]->Y()) + (faceNormal->Z() * edge->getEdgeEndPoints()[0]->Z());
      d2 = (faceNormal->X() * edge->getEdgeEndPoints()[1]->X()) + (faceNormal->Y() * edge->getEdgeEndPoints()[1]->Y()) + (faceNormal->Z() * edge->getEdgeEndPoints()[1]->Z());

      if (fabsl(d1 - d2) < (Standard_Real)0.00001) {
        return -d1;
      }

    }

    throw std::logic_error("Could not create equation of the plane.");
  }

  bool isEqual(const std::shared_ptr<gp_Pnt>& aVertex, const std::shared_ptr<gp_Pnt>& bVertex)
  {
     return aVertex->IsEqual(*bVertex, Precision::Confusion());
  }

  Standard_Real computeEuclideanNorm(const gp_Pnt& vertex)
  {
    gp_XYZ v(vertex.X(), vertex.Y(), vertex.Z());

    return v.Modulus();
  }

  gp_Pnt computeCrossProduct(const gp_Pnt& aVector, const gp_Pnt& bVector)
  {
    Standard_Real X = (aVector.Y() * bVector.Z()) - (aVector.Z() * bVector.Y());
    Standard_Real Y = - ((aVector.X() * bVector.Z()) - (aVector.Z() * bVector.X()));
    Standard_Real Z = (aVector.X() * bVector.Y()) - (aVector.Y() * bVector.X());

    gp_Pnt normal(X, Y, Z);

    return normal;
  }

  Standard_Real computeDotProduct(const gp_Pnt& aVertex, const gp_Pnt& bVertex)
  {
    Standard_Real A = aVertex.X() * bVertex.X();
    Standard_Real B = aVertex.Y() * bVertex.Y();
    Standard_Real C = aVertex.Z() * bVertex.Z();

    return (A + B + C);
  }

  Standard_Real computeLength(const Fxt::SheetMetalComponent::Edge::ModelEdge& edge)
  {
    gp_Pnt vertex(
      edge.getEdgeEndPoints()[0]->X() - edge.getEdgeEndPoints()[1]->X(),
      edge.getEdgeEndPoints()[0]->Y() - edge.getEdgeEndPoints()[1]->Y(),
      edge.getEdgeEndPoints()[0]->Z() - edge.getEdgeEndPoints()[1]->Z()
    );

    return roundd(computeEuclideanNorm(vertex));
  }

  Standard_Real computeThickness(const std::shared_ptr<Fxt::SheetMetalComponent::Face::ModelFace>& face)
  {
    // size_t count = face.getFaceEdges().size();

    // smallest (thickness) represent the length of the smallest side of a
    // THICKNESS_DEFINING_FACE.
    Standard_Real smallest = face->getFaceEdges()[0]->getEdgeLength();

    for (auto& edge : face->getFaceEdges()) {
      if (smallest > edge->getEdgeLength()) {
        smallest = edge->getEdgeLength();
      }
    }

    return smallest;
  }

  Standard_Real computeAngle(const std::shared_ptr<gp_Pnt>&  aFaceNormal, const std::shared_ptr<gp_Pnt>&  bFaceNormal)
  {
    Standard_Real cosine, angle;
    Standard_Real dotProduct = computeDotProduct(*aFaceNormal, *bFaceNormal);

    Standard_Real aEuclideanNorm = computeEuclideanNorm(*aFaceNormal);
    Standard_Real bEuclideanNorm = computeEuclideanNorm(*bFaceNormal);

    if (dotProduct == 0) {
      cosine = 0;
    } else {
      cosine = dotProduct / (aEuclideanNorm * bEuclideanNorm);
    }

    Standard_Real value = 180.0 / M_PI;

    // convert the angle from radian to degrees.
    angle = acos(roundd(cosine)) * value;

    return round(angle);
  }

  gp_Pnt computeMidPoint(const std::shared_ptr<gp_Pnt>& p1, const std::shared_ptr<gp_Pnt>& p2)
  {
    double x, y, z;         

    x = roundd(0.5 * (p1->X() + p2->X()));
    y = roundd(0.5 * (p1->Y() + p2->Y()));
    z = roundd(0.5 * (p1->Z() + p2->Z()));

    return gp_Pnt(x, y, z);
  }

  size_t computeFactorial(const size_t nBends)
  {
      int factorial {1};
      for (unsigned long long n {1}; n <= nBends; ++n)
      {
          factorial *= n;    // Calculate n! for current n
      }

      return factorial;
  }

}
}