#include "../../../include/sheet-metal-component/bend/BendFeature.h"
#include "../../../include/sheet-metal-component/bend/ModelBend.h"

using namespace Fxt::SheetMetalComponent::Bend;

std::ostream & operator<<(std::ostream &os, const BendFeature &bf)
{
    os << std::setprecision(3);
    os << bf.getBendAngle() << " "
       << bf.getBendDirection() << " "
       << bf.getBendLength() << " "
       << bf.getJoiningFaceID1() << " "
       << bf.getJoiningFaceID2() << " ";

    os << '\n';
        
    return os;
}

void BendFeature::setJoiningFaceID1(const FaceID mFaceID)
{
    mJoinedFaceId1 = mFaceID;
}

FaceID BendFeature::getJoiningFaceID1() const 
{ 
    return mJoinedFaceId1; 
}

void BendFeature::setJoiningFaceID2(FaceID mFaceID)
{
    mJoinedFaceId2 = mFaceID;
}

FaceID BendFeature::getJoiningFaceID2() const
{
    return mJoinedFaceId2;
}

void BendFeature::setBendAngle(const long double angle)
{
    mBendAngle = angle;
}

long double BendFeature::getBendAngle() const
{
    return mBendAngle;
}

void BendFeature::setBendDirection(const size_t bend_direction)
{
    mBendDirection = bend_direction;
}

size_t BendFeature::getBendDirection() const
{
    return mBendDirection;
}

void BendFeature::setBendLength(const long double bendLength)
{
    mBendLength = bendLength;
}

long double BendFeature::getBendLength() const
{
    return mBendLength;
}

void BendFeature::setCurvature(const long double cv)
{
    mCurvature = cv;

    if (mCurvature != 0.0) {
      // Divide by 2 to get radius else you get the diameter.
      mBendRadius = Computation::roundd(((mCurvature < 0.0) ? -(1 / mCurvature) : (1 / mCurvature)) / 2);
    }
}

void BendFeature::setBendRadius(long double bendRadius)
{
    mBendRadius = bendRadius;
}

long double BendFeature::getBendRadius() const
{
    return mBendRadius;
}