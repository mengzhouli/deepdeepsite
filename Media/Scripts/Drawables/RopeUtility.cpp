#include "Drawables/RopeUtility.hpp"

namespace Dwarf
{
    namespace Graphics
    {
        Chainf ComputeRopePositions(const Vector2f& start, const Vector2f& end, uint32_t numSegments, const Vector2f& up, float a)
        {
            Chainf pathChain;
            pathChain.AddPoint(start);

            float width = Abs(start.X - end.X);
            const float widthA = width * a;
            float sideOffset = widthA * HyperbolicCosine((width * 0.5f) / widthA) - width;

            for (int32_t i = 1; i < static_cast<int32_t>(numSegments) - 1; i++)
            {
                float perc = static_cast<float>(i) / (numSegments - 1);

                Vector2f pos = Vector2f::Lerp(start, end, perc);
                pos += up * ((widthA * HyperbolicCosine(((perc - 0.5f) * width) / widthA)) - width - sideOffset);

                pathChain.AddPoint(pos);
            }

            pathChain.AddPoint(end);

            return pathChain;
        }
    }
}
