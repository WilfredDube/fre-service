#pragma once

#include "ModelTypes.h"
#include "../../include/sheet-metal-component/bend/ModelBend.h"
#include "../../include/sheet-metal-component/face/ModelFace.h"

#include <TopoDS_Shape.hxx>

#include <ostream>
#include <map>
#include <memory>
#include <deque>

namespace Fxt
{
namespace SheetMetalComponent
{
    using FaceID = Fxt::SheetMetalComponent::ModelTypes::FaceID;

    class SheetMetal
    {
        friend class boost::serialization::access;
        friend std::ostream & operator<<(std::ostream &os, const SheetMetal &m);

        template<class Archive>
        void serialize(Archive &ar, const unsigned int version)
        {
            ar & mModelBends;
        }

        std::map<FaceID, std::shared_ptr<Face::ModelFace>> mModelFaces;
        std::map<FaceID, std::shared_ptr<Bend::ModelBend>> mModelBends;

        long double mThickness {0.0};

        std::shared_ptr<TopoDS_Shape> mTopologicalShape;

        void removeOuterFaces();

        void reAssignFaceId(bool isBend = false);

        void setThicknessDefiningFaceAttributes();

        void setFaceIdWrapper(std::map<FaceID, std::shared_ptr<Face::ModelFace>>& mFaces);
        void setBendIdWrapper(std::map<FaceID, std::shared_ptr<Bend::ModelBend>>& mBends);

        /**
         * After reassigning the bend and face ID's the is a disconnection between faces and bends
         * because the JoiningFaceId to which the bends were pointing no long exit.
         * 
         * Method : searches the face list (mModelFaces) to find the faces to which each bend is 
         *           connected too.
         */
        void connectBendsToNewFaceId();

    public:
        void addModelFace(std::shared_ptr<Face::ModelFace>& modelFace);

        void addModelBend(std::shared_ptr<Bend::ModelBend>& modelBend);

        std::shared_ptr<gp_Dir> getNormalByFaceID(const FaceID faceID) const;

        FaceID getIdOfBendWithJoiningFaceID(const FaceID currbendID, const FaceID faceID) const;
        
        /**
         * Seperates bends into inner and outer bend faces.
         */
        bool splitModelBends(const FaceID id, std::map<FaceID, 
                        std::shared_ptr<Bend::ModelBend>>& innerBends, 
                        std::map<FaceID, std::shared_ptr<Bend::ModelBend>>& outerBends,
                        std::deque<FaceID>& queue);

        std::map<FaceID, std::shared_ptr<Face::ModelFace>> getFaces() const;
        std::map<FaceID, std::shared_ptr<Bend::ModelBend>> getBends() const;

        double getThickness() const;

        void assignFaceAttributes(const FaceID faceID, std::shared_ptr<TopoDS_Shape>& aShape);

        void classifyFaces();

        /**
         *  Method removes outer faces leaving inner faces for easy computation and representation 
         *  of bend features.
         *  Fixes the problem of processing a bends twice because of it has an inner and outer face.
         */
        bool removeOuterBendFaces();

        bool reduceModelSize();

        void computeBendAngles();

        int getNumberOfModules();

        bool isParallel(FaceID bend1, FaceID bend2);
        bool isSameAngle(FaceID bend1, FaceID bend2);
        bool isSameDirection(FaceID bend1, FaceID bend2);

        void assignBendDirection();

        double distance(FaceID bend1, FaceID bend2);

        friend std::ostream& operator<<(std::ostream& os, const std::shared_ptr<SheetMetal>& sheetMetal)
        {
            using namespace Fxt::SheetMetalComponent::Bend;

            os << "Thickness : " << sheetMetal->getThickness() << '\n';

            for(const auto& [bendId, bend] : sheetMetal->getBends()){
                os << bend << '\n';
            }
            
            return os;
        }
    };
}
}

using SheetMetalPtr = std::shared_ptr<Fxt::SheetMetalComponent::SheetMetal>;