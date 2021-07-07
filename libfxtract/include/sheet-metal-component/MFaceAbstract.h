#pragma once

#include "ModelTypes.h"
#include "BoostSerializer.h"
#include "edge/ModelEdge.h"

#include <GeomAbs_CurveType.hxx>
#include <Interface_Static.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>

#include <memory>
#include <vector>

namespace Fxt
{
namespace SheetMetalComponent
{
    class MFaceAbstract
    {
        friend class boost::serialization::access; 
        friend std::ostream & operator<<(std::ostream &os, const MFaceAbstract &mf);

        template<class Archive>
        void serialize(Archive & ar, const unsigned int /* file_version */){
            ar & mFaceID;
        }
    protected:
        enum class PlaneType { PLANAR, NON_PLANAR };

        FaceID mFaceID; /*!< an integer value  represents the Face Identification number */
        PlaneType mPlaneType;
        FaceType mFaceType;

        gp_DirPtr mFaceNormal; //!< the normal vector to the face.
        gp_DirPtr mFaceUnitNormal; //!< the unit normal vector to the face.

        std::vector<ModelEdgePtr> mFaceEdges; //!< edges of the face.

        TopoDS_FacePtr mModelFace;
    public:
        MFaceAbstract() = default;

        MFaceAbstract(const FaceID faceID = 0, TopoDS_FacePtr topoDSFace = nullptr);

        virtual void init() = 0;

        void setTModelFace(TopoDS_FacePtr& tface);
        TopoDS_FacePtr getTModelFace() const;

        void setFaceId(FaceID faceID);
        FaceID getFaceId() const;

        void setUnitNormal(gp_DirPtr unit_normal);
        gp_DirPtr getUnitNormal() const;

        gp_DirPtr getFaceNormal() const;
        void computeFaceNormal();

        void setFaceType(FaceType ftype);
        FaceType getFaceType() const;

        void setPlaneType(PlaneType ptype);
        PlaneType getPlaneType() const;

        void extractEdges();

        void setFaceEdgePosition();

        void addEdge(const ModelEdgePtr& edge);

        std::vector<ModelEdgePtr>& getFaceEdges();

        ModelEdgePtr getFaceEdgesAt(const int index) const;

        // void getFaceEdgePosition() const; 
    };
}
}