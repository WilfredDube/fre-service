#pragma once

#include "../BoostSerializer.h"
#include <ostream>

namespace Fxt 
{
namespace SheetMetalComponent
{
namespace Bend
{
    class Point 
    {
        friend class boost::serialization::access;
        friend std::ostream & operator<<(std::ostream &os, const Point &p);

        template<class Archive>
        void serialize(Archive &ar, const unsigned int version)
        {
            // save/load base class information
            ar & X & Y & Z;
        }

    public:
        long double X, Y, Z;
  };
}
}
}