#pragma once

#include "Vector2.hpp"
#include "Geometry/Chain.hpp"

namespace Dwarf
{
    namespace Graphics
    {
        Chainf ComputeRopePositions(const Vector2f& start, const Vector2f& end, uint32_t numSegments, const Vector2f& up, float a);
    }
}
