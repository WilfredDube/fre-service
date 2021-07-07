#pragma once

#include "Point.h"
#include "../BoostSerializer.h"
#include "../edge/ModelEdge.h"

#include <gp_Lin.hxx>

#include <memory>
#include <vector>

namespace Fxt
{
namespace SheetMetalComponent
{

namespace Bend
{        
    class BendLine
    {
        friend class boost::serialization::access;
        friend std::ostream & operator<<(std::ostream &os, const BendLine &br);

        template<class Archive>
        void serialize(Archive &ar, const unsigned int version)
        {
            // save/load base class information
            ar & mBendLinePnt & mBendLineDir;

            makeBendLine();
        }
    
        // Central in line along a bend and between the LINE type edges.
        std::shared_ptr<gp_Lin> mBendLine;
        
        std::shared_ptr<Point> mBendLinePnt, mBendLineDir;

    public:
        void computeBendLine(
            const std::vector<std::shared_ptr<Fxt::SheetMetalComponent::Edge::ModelEdge>>& arcEdges);

        void makeBendLine();
        
        std::shared_ptr<gp_Lin> getBendLine() const;

        double distance(const std::shared_ptr<Fxt::SheetMetalComponent::Bend::BendLine>& otherBendLine) const;

        double angle(const std::shared_ptr<Fxt::SheetMetalComponent::Bend::BendLine>& otherBendLine) const;
    };
}
}
}