#include "Drawables/RopeDrawable.hpp"

#include "Geometry/RotatedRectangle.hpp"
#include "MathUtility.hpp"

namespace Dwarf
{
    namespace Graphics
    {
        static const std::string RopeTexturePath = "Skeletons/Characters/Rope/rope.png";

        RopeDrawable::RopeDrawable(Content::ContentManager* cm)
            : _texture(nullptr)
            , _color(Color::White)
            , _chain()
            , _width(25.0f)
        {
            _texture = cm->Load<Graphics::Texture>(RopeTexturePath);
        }

        RopeDrawable::~RopeDrawable()
        {
            SafeRelease(_texture);
        }

        void RopeDrawable::SetChain(const Chainf& chain)
        {
            _chain = chain;
            updateVertices();
        }

        void RopeDrawable::SetSpline(const Splinef& spline, uint32_t numSegments)
        {
            _chain.Clear();
            for (uint32_t i = 0; i < (numSegments + 1); i++)
            {
                float t = float(i) / numSegments;
                _chain.AddPoint(spline.Evalulate(t));
            }
            updateVertices();
        }

        void RopeDrawable::SetColor(const Color& color)
        {
            _color = color;
        }

        void RopeDrawable::updateVertices()
        {
            _vertices.clear();
            for (uint32_t i = 0; i + 1 < _chain.Size(); i++)
            {
                Vector2f a = _chain.At(i);
                Vector2f b = _chain.At(i + 1);

                float len = Vector2f::Distance(a, b);
                RotatedRectanglef linkRectangle((a + b) * 0.5f, Vector2f(_width, len * 0.5f), Rotatorf::Zero - Rotatorf(b - a) + Rotatorf::PiOver2);
                float lenScale = (len / _width) * ((_texture->Width() + 0.0f) / _texture->Height());

                vertex va{ linkRectangle.TopLeft() , Vector2f(0.0f, 0.0f) };
                vertex vb{ linkRectangle.TopRight() , Vector2f(1.0f, 0.0f) };
                vertex vc{ linkRectangle.BottomLeft() , Vector2f(0.0f, lenScale) };
                vertex vd{ linkRectangle.BottomRight() , Vector2f(1.0f, lenScale) };

                _vertices.push_back(va);
                _vertices.push_back(vb);
                _vertices.push_back(vc);

                _vertices.push_back(vb);
                _vertices.push_back(vc);
                _vertices.push_back(vd);
            }
        }

        const Rectanglef& RopeDrawable::GetDrawBounds() const
        {
            return _chain.Bounds();
        }

        void RopeDrawable::Draw(Graphics::SpriteRenderer* spriteRenderer) const
        {
            for (uint32_t vertexIdx = 0; vertexIdx < _vertices.size(); vertexIdx += 3)
            {
                const vertex& a = _vertices[vertexIdx + 0];
                const vertex& b = _vertices[vertexIdx + 1];
                const vertex& c = _vertices[vertexIdx + 2];

                spriteRenderer->DrawTriangle(_texture, a.pos, b.pos, c.pos, a.texcoord, b.texcoord, c.texcoord, _color);
            }
        }

        bool RopeDrawable::ContainsPoint(const Vector2f& pt) const
        {
            if (!Rectanglef::Contains(_chain.Bounds(), pt))
            {
                return false;
            }

            for (uint32_t i = 1; i < _chain.Size(); i++)
            {
                if (Math::PointToLineDistance(_chain[i - 1], _chain[i], pt))
                {
                    return true;
                }
            }

            return false;
        }
    }

    template <>
    void EnumeratePreloads<Graphics::RopeDrawable>(PreloadSet& preloads)
    {
        preloads.insert(Graphics::RopeTexturePath);
    }
}
