#include "../../../include/sheet-metal-component/bend/ModelBend.h"
#include "../../../include/sheet-metal-component/bend/BendLine.h"
#include "../../../include/Computation.h"
#include "../../../include/sheet-metal-component/ModelTypes.h"

using namespace Fxt::SheetMetalComponent::Bend;
using namespace Fxt::SheetMetalComponent::Edge;

ModelBend::ModelBend(const FaceID faceID, std::shared_ptr<TopoDS_Face> topoDSFace)
: MFaceAbstract(faceID, topoDSFace)
{
    if(topoDSFace != nullptr)
    {
        mBendFeature = {std::make_shared<BendFeature>()};
        mBendLine = {std::make_shared<BendLine>()};

        setPlaneType(PlaneType::NON_PLANAR);
        setFaceType(FaceType::BEND_FACE);

        mBendFeature->setJoiningFaceID1(0);
        mBendFeature->setJoiningFaceID2(0);
        mBendFeature->setBendAngle(0);

        init();
    }
}

void ModelBend::init()
{
    extractEdges();

    computeFaceNormal();
    setUnitNormal(Computation::computeUnitNormal(mModelFace));

    if(mArcEdges.size() != 2)
        arcEdgeExtraction();

    setBendLength();

    mBendLine->computeBendLine(mArcEdges);
}

void ModelBend::arcEdgeExtraction()
{
    for(const auto& edge : mFaceEdges){
        if(edge->isArc()){
            mArcEdges.push_back(edge);
        } else
        {
            mStraightEdges.push_back(edge);
        }
    }
}

void ModelBend::setBendLength()
{
    auto straightEdge = getStraightEdges()[0];
    mBendFeature->setBendLength(straightEdge->getEdgeLength());
}

std::shared_ptr<Fxt::SheetMetalComponent::Bend::BendLine> ModelBend::getBendLine() const 
{
    return mBendLine;
}

double ModelBend::computeBendDistance(const std::shared_ptr<Fxt::SheetMetalComponent::Bend::ModelBend>& otherBend) const
{
    return mBendLine->distance(otherBend->getBendLine());
}

bool ModelBend::isParallel(const std::shared_ptr<ModelBend>& otherBend) const
{
    auto angle = mBendLine->angle(otherBend->getBendLine());

    return (angle == 0.0 || angle == 180);
}

std::shared_ptr< Fxt::SheetMetalComponent::Bend::BendFeature> ModelBend::getBendFeature() const
{
    return mBendFeature;
}

std::vector<std::shared_ptr<ModelEdge>> ModelBend::getStraightEdges() const
{
    return mStraightEdges;
}

std::ostream& operator<<(std::ostream& os, const std::shared_ptr<ModelBend>& modelBend)
{
    os << modelBend->getFaceId() << '\n';
    
    auto bendFeature = modelBend->getBendFeature();

    os << "F" << bendFeature->getJoiningFaceID1() << "---" << 
          "B" << modelBend->getFaceId() << "---" <<
          "F" << bendFeature->getJoiningFaceID2() << " ";
    
    os << "Angle: " << bendFeature->getBendAngle() << ", ";
    os << "Radius: " << bendFeature->getBendRadius() << "mm, ";
    os << "Bend length: " << bendFeature->getBendLength() << "mm";
    
    return os;
}
