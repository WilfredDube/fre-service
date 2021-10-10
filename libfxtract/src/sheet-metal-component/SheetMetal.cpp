#include "../../include/sheet-metal-component/SheetMetal.h"
#include "../../include/sheet-metal-component/bend/BendFeature.h"

using namespace Fxt::SheetMetalComponent;

void SheetMetal::addModelFace(std::shared_ptr<Face::ModelFace> &modelFace)
{
    mModelFaces[modelFace->getFaceId()] = modelFace;
}

void SheetMetal::addModelBend(std::shared_ptr<Bend::ModelBend> &modelBend)
{
    mModelBends[modelBend->getFaceId()] = modelBend;
}

std::shared_ptr<gp_Dir> SheetMetal::getNormalByFaceID(const FaceID faceID) const
{
    auto modelFace = mModelFaces.find(faceID)->second;
    return modelFace->getFaceNormal();
}

FaceID SheetMetal::getIdOfBendWithJoiningFaceID(const FaceID currbendID, const FaceID faceID) const
{
    auto modelBend = mModelBends.find(currbendID);

    if (modelBend != mModelBends.end())
    {
        auto bend = modelBend->second;

        if (faceID == bend->getBendFeature()->getJoiningFaceID1() ||
            faceID == bend->getBendFeature()->getJoiningFaceID2())
            return bend->getFaceId();
    }

    return -1;
}

bool SheetMetal::splitModelBends(const FaceID id, std::map<FaceID, std::shared_ptr<Bend::ModelBend>> &innerBends,
                                 std::map<FaceID, std::shared_ptr<Bend::ModelBend>> &outerBends,
                                 std::deque<FaceID> &queue)
{
    bool found = false;

    // TODO: Find appropriate STL <algorithm> functions
    for (auto bendIter = innerBends.begin(); bendIter != innerBends.end();)
    {
        auto bend = bendIter->second;
        if (id == bend->getBendFeature()->getJoiningFaceID1())
        {
            outerBends[bend->getFaceId()] = bend;

            queue.push_back(bend->getBendFeature()->getJoiningFaceID1());
            queue.push_back(bend->getBendFeature()->getJoiningFaceID2());

            found = true;
            bendIter = innerBends.erase(bendIter);
        }
        else if (id == bend->getBendFeature()->getJoiningFaceID2())
        {
            outerBends[bend->getFaceId()] = bend;

            queue.push_back(bend->getBendFeature()->getJoiningFaceID1());
            queue.push_back(bend->getBendFeature()->getJoiningFaceID2());

            found = true;
            bendIter = innerBends.erase(bendIter);
        }
        else
        {
            ++bendIter;
        }
    }

    return found;
}

std::map<FaceID, std::shared_ptr<Face::ModelFace>> SheetMetal::getFaces() const
{
    return mModelFaces;
}

std::map<FaceID, std::shared_ptr<Bend::ModelBend>> SheetMetal::getBends() const
{
    return mModelBends;
}

double SheetMetal::getThickness() const
{
    return mThickness;
}

void SheetMetal::assignFaceAttributes(const FaceID faceID, std::shared_ptr<TopoDS_Shape> &aShape)
{
    mTopologicalShape = aShape;

    auto pTopoDSFace{std::make_shared<TopoDS_Face>(TopoDS::Face(*mTopologicalShape))};

    Standard_Real curvature = Computation::computeCurvature(pTopoDSFace);

    if (curvature == 0.0)
    {

        auto pModelFace{std::make_shared<Face::ModelFace>(faceID, pTopoDSFace)};

        addModelFace(pModelFace);
    }
    else
    {

        auto pModelFace{std::make_shared<Bend::ModelBend>(faceID, pTopoDSFace)};

        pModelFace->getBendFeature()->setCurvature(curvature);

        addModelBend(pModelFace);
    }
}

void SheetMetal::classifyFaces()
{
    std::map<FaceID, std::shared_ptr<Face::ModelFace>> tempFaces;

    // Extract all the useful faces
    for (auto &[bendId, bend] : mModelBends)
    {
        for (auto &edge : bend->getFaceEdges())
        {
            if (edge->isLine())
            {
                for (auto &[faceId, face] : mModelFaces)
                {
                    if (face->getFaceType() == FaceType::NONE)
                    {
                        for (auto &faceEdge : face->getFaceEdges())
                        {
                            if (abs(faceEdge->getEdgeLength() - edge->getEdgeLength()) < 0.01)
                            {
                                if (*faceEdge == *edge)
                                {
                                    faceEdge->setEdgePosition(Edge::ModelEdge::EdgePosition::JOINING_EDGE);
                                    face->setFaceType(FaceType::FACE);

                                    tempFaces.insert({faceId, face});
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    setThicknessDefiningFaceAttributes();

    setFaceIdWrapper(mModelFaces);
    setBendIdWrapper(mModelBends);

    mModelFaces = tempFaces;

    connectBendsToNewFaceId();

    for (auto &[faceId, face] : mModelFaces)
        face->setFaceEdgePosition();
}

void SheetMetal::setBendIdWrapper(std::map<FaceID, std::shared_ptr<Bend::ModelBend>> &mFaces)
{
    size_t faceID = 0;

    for (auto &[faceId, face] : mFaces)
    {
        ++faceID;
        face->setFaceId(faceID);
    }
}

void SheetMetal::setFaceIdWrapper(std::map<FaceID, std::shared_ptr<Face::ModelFace>> &mFaces)
{
    size_t faceID = 0;

    for (auto &[faceId, face] : mFaces)
    {
        ++faceID;
        face->setFaceId(faceID);
    }
}

void SheetMetal::setThicknessDefiningFaceAttributes()
{
    for (auto &[faceId, face] : mModelFaces)
    {
        if (face->getFaceType() == FaceType::NONE)
        {
            face->setFaceType(FaceType::THICKNESS_DEFINING_FACE);
            face->setFaceId(0);

            if (mThickness == 0.0)
            {
                mThickness = Computation::computeThickness(face);
            }
        }
    }
}

bool SheetMetal::reduceModelSize()
{
    bool done = false;
    if (removeOuterBendFaces())
    {
        removeOuterFaces();

        reAssignFaceId(true);
        reAssignFaceId();

        connectBendsToNewFaceId();

        done = true;
    }

    if (!done)
    {
        throw "Unable to reduce model size.";
    }

    return done;
}

bool SheetMetal::removeOuterBendFaces()
{
    FaceID searchID;
    std::map<FaceID, std::shared_ptr<Bend::ModelBend>> &allModelBends = mModelBends;
    std::map<FaceID, std::shared_ptr<Bend::ModelBend>> outerBends;
    std::deque<FaceID> queue;

    auto &firstBend = allModelBends.begin()->second;
    outerBends[firstBend->getFaceId()] = firstBend;

    queue.push_back(firstBend->getBendFeature()->getJoiningFaceID1());
    queue.push_back(firstBend->getBendFeature()->getJoiningFaceID2());

    searchID = firstBend->getBendFeature()->getJoiningFaceID1();

    allModelBends.erase(allModelBends.begin());

    while (outerBends.size() != allModelBends.size() && !queue.empty())
    {
        splitModelBends(searchID, allModelBends, outerBends, queue);

        queue.pop_front();

        searchID = queue.front();
    }

    if (outerBends.size() == allModelBends.size())
        allModelBends = outerBends;

    std::cout << "All : " << allModelBends.size() << std::endl;
    std::cout << "Out : " << outerBends.size() << std::endl;
    return (outerBends.size() == allModelBends.size());
}

void SheetMetal::removeOuterFaces()
{
    std::map<FaceID, std::shared_ptr<Face::ModelFace>> innerFaces;

    for (auto &[id, bend] : mModelBends)
    {
        auto face = mModelFaces.find(bend->getBendFeature()->getJoiningFaceID1());

        if (face != mModelFaces.end())
        {
            innerFaces.insert({face->first, face->second});
        }

        face = mModelFaces.find(bend->getBendFeature()->getJoiningFaceID2());

        if (face != mModelFaces.end())
        {
            innerFaces.insert({face->first, face->second});
        }
    }

    if (innerFaces.size() == (mModelFaces.size() / 2))
        mModelFaces = innerFaces;
}

void SheetMetal::reAssignFaceId(bool isBend)
{
    size_t newFaceId = 0;

    if (isBend)
    {
        std::map<FaceID, std::shared_ptr<Bend::ModelBend>> tempBends;

        for (auto &bend : mModelBends)
        {
            ++newFaceId;

            bend.second->setFaceId(newFaceId);
            bend.second->getBendFeature()->setJoiningFaceID1(0);
            bend.second->getBendFeature()->setJoiningFaceID2(0);

            tempBends[newFaceId] = bend.second;
        }

        mModelBends = tempBends;
    }
    else
    {
        std::map<FaceID, std::shared_ptr<Face::ModelFace>> tempFaces;

        for (auto &face : mModelFaces)
        {
            ++newFaceId;

            face.second->setFaceId(newFaceId);

            tempFaces[newFaceId] = face.second;
        }

        mModelFaces = tempFaces;
    }
}

// TODO: toooooo many for loops
void SheetMetal::connectBendsToNewFaceId()
{
    for (auto &[bendId, bend] : mModelBends)
    {
        std::vector<std::shared_ptr<Edge::ModelEdge>> straightEdges = bend->getStraightEdges();

        for (const auto &edge : straightEdges)
        {
            for (auto &[faceId, face] : mModelFaces)
            {
                if (face->getFaceType() == Fxt::SheetMetalComponent::ModelTypes::FaceType::FACE)
                {
                    for (const auto &faceEdge : face->getFaceEdges())
                    {
                        if (abs(faceEdge->getEdgeLength() - edge->getEdgeLength()) < 0.01)
                        {
                            if (*faceEdge == *edge)
                            {
                                faceEdge->setEdgePosition(Edge::ModelEdge::EdgePosition::JOINING_EDGE);
                                if ((bend->getBendFeature()->getJoiningFaceID1() == 0) &&
                                    (bend->getBendFeature()->getJoiningFaceID2() == 0))
                                {
                                    bend->getBendFeature()->setJoiningFaceID1(faceId);
                                    edge->setJoiningFaceID(faceId);
                                }
                                else if (bend->getBendFeature()->getJoiningFaceID2() == 0)
                                {
                                    bend->getBendFeature()->setJoiningFaceID2(faceId);
                                    edge->setJoiningFaceID(faceId);
                                }
                                else
                                {
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void SheetMetal::computeBendAngles()
{
    for (auto &[bendId, bend] : mModelBends)
    {

        if ((bend->getBendFeature()->getJoiningFaceID1() != 0) && (bend->getBendFeature()->getJoiningFaceID2() != 0))
        {
            auto fn1 = getNormalByFaceID(bend->getBendFeature()->getJoiningFaceID1());
            auto fn2 = getNormalByFaceID(bend->getBendFeature()->getJoiningFaceID2());

            double angle = round(fn1->Angle(*fn2) * (180.0 / M_PI));

            bend->getBendFeature()->setBendAngle(Computation::roundd(angle));
        }
    }
}

int SheetMetal::getNumberOfModules()
{
    const Standard_Real kMachineModuleLength = 1000.0;
    long double longest = 0.0;

    for (auto [bendId, bend] : getBends())
    {
        if (longest < bend->getBendFeature()->getBendLength())
        {
            longest = bend->getBendFeature()->getBendLength();
        }
    }

    int num_of_modules = 0;

    if (longest > kMachineModuleLength)
    {
        num_of_modules = longest / kMachineModuleLength - 1;

        if ((static_cast<int>(longest) % static_cast<int>(kMachineModuleLength)) > 0)
            ++num_of_modules;
    }

    return num_of_modules;
}

bool SheetMetal::isParallel(FaceID bend1, FaceID bend2)
{
    bool parallel = false;
    auto b1 = mModelBends.find(bend1);
    auto b2 = mModelBends.find(bend2);

    if (b1 != mModelBends.end() && b2 != mModelBends.end())
    {
        parallel = b1->second->isParallel(b2->second);
    }

    return parallel;
}

bool SheetMetal::isSameAngle(FaceID bend1, FaceID bend2)
{
    bool parallel = false;
    auto b1 = mModelBends.find(bend1);
    auto b2 = mModelBends.find(bend2);

    if (b1 != mModelBends.end() && b2 != mModelBends.end())
    {
        parallel = (b1->second->getBendFeature()->getBendAngle() == b2->second->getBendFeature()->getBendAngle());
    }

    return parallel;
}

double SheetMetal::distance(FaceID bend1, FaceID bend2)
{
    double distance = 0;
    auto b1 = mModelBends.find(bend1);
    auto b2 = mModelBends.find(bend2);

    if (b1 != mModelBends.end() && b2 != mModelBends.end())
    {
        if (b1->second->isParallel(b2->second))
        {
            distance = b1->second->computeBendDistance(b2->second);
        }
    }

    return distance;
}

bool SheetMetal::isSameDirection(FaceID bend1, FaceID bend2)
{
    auto b1 = mModelBends.find(bend1);
    auto b2 = mModelBends.find(bend2);

    if (b1 != mModelBends.end() && b2 != mModelBends.end())
        return (b1->second->getBendFeature()->getBendDirection() == b2->second->getBendFeature()->getBendDirection());

    return false;
}

void SheetMetal::assignBendDirection()
{
    auto max = std::numeric_limits<long double>::min();
    auto min = std::numeric_limits<long double>::max();

    for (auto &[id, bend] : mModelBends)
    {
        if (max < bend->getBendFeature()->getBendRadius())
        {
            max = bend->getBendFeature()->getBendRadius();

            bend->getBendFeature()->setBendDirection(1);
        }

        if (min > bend->getBendFeature()->getBendRadius())
        {
            min = bend->getBendFeature()->getBendRadius();

            bend->getBendFeature()->setBendDirection(2);
        }
    }

    for (auto &[id, bend] : mModelBends)
    {
        if (max == bend->getBendFeature()->getBendRadius())
        {
            bend->getBendFeature()->setBendDirection(1);
        }

        if (min == bend->getBendFeature()->getBendRadius())
        {
            bend->getBendFeature()->setBendDirection(2);
        }

        // std::cerr << "bend dir: " << bend->getBendFeature()->getBendDirection() << '\n';
        bend->getBendFeature()->setBendRadius(min);
    }

    // std::cerr << "Max radius: " << max << '\n';
    // std::cerr << "Min radius: " << min << '\n';
    // std::cerr << "Actual radius: " << (min + max) / 2 << '\n';
}