#pragma once

// #include "../BoostSerializer.h"
// #include "../edge/ModelEdge.h"
// #include "../MFaceAbstract.h"
#include "../../Computation.h"

#include <gp_Lin.hxx>
#include <TopoDS_Face.hxx>
#include <vector>

namespace Fxt
{
namespace SheetMetalComponent
{
namespace Bend
{
    struct BendFeature
    {
    private:
        friend class boost::serialization::access;
        friend std::ostream & operator<<(std::ostream &os, const BendFeature &br);

        template<class Archive>
        void serialize(Archive &ar, const unsigned int version)
        {
            // save/load base class information
            ar & mBendAngle & mBendDirection;
            ar & mBendLength;
            ar & mJoinedFaceId1 & mJoinedFaceId2;
        }

        long double mBendLength;

        //!< face curvature value : zero for planar type faces and non-zero for non-planar type faces.
        long double mCurvature;

        //!< the face ID of the face connected to the bend face via its Straight edge.
        FaceID mJoinedFaceId1;

        //!< the face ID of the second face connected to the bend face via its Straight edge.
        FaceID mJoinedFaceId2;

        long double mBendingForce; //!< force required to create the bend.
        long double mBendAngle; //!< bend angle.
        long double mBendRadius; //!< bend radius : zero if face is planar and non-zero if face is a bend.
        size_t mBendDirection;

    public:

        void setJoiningFaceID1(const FaceID mFaceID);
        FaceID getJoiningFaceID1() const;

        void setJoiningFaceID2(const FaceID mFaceID);
        FaceID getJoiningFaceID2() const;

        void setBendAngle(const long double angle);
        long double getBendAngle() const;

        void setBendDirection(const size_t bend_direction);
        size_t getBendDirection() const;

        void setBendLength(const long double bendLength);
        long double getBendLength() const;

        void setCurvature(const long double cv);
        void setBendRadius(long double bendRadius);
        long double getBendRadius() const;
        
        // TODO : calculate Bending force, direction, orientation & distance
    };
}
}
}