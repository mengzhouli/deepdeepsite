#include "HUD/SkeletonPanelDrawable.hpp"

namespace Dwarf
{
    namespace HUD
    {
        SkeletonPanelDrawable::SkeletonPanelDrawable(const Animation::Skeleton* skeleton)
        {
            assert(skeleton != nullptr);
            _skeleton = new Animation::SkeletonInstance(skeleton);
        }

        SkeletonPanelDrawable::~SkeletonPanelDrawable()
        {
            SafeRelease(_skeleton);
        }

        void SkeletonPanelDrawable::MergeMaterialSet(const Graphics::PolygonMaterialSet* matset)
        {
            _skeleton->MergeMaterialSet(matset);
        }

        void SkeletonPanelDrawable::SetPose(const std::string& anim, float time)
        {
            _skeleton->PlayAnimation(anim, false, 0.0f, 0.0f);
            _skeleton->Update(0.0, time);
        }

        void SkeletonPanelDrawable::SetRotation(const Rotatorf& rotation)
        {
            _skeletonRotation = rotation;
        }

        void SkeletonPanelDrawable::SetScale(float scale)
        {
            _skeletonScale = scale;
        }

        void SkeletonPanelDrawable::SetColor(const Color& color)
        {
            _skeletonColor = color;
        }

        void SkeletonPanelDrawable::SetSize(const Vector2f& size)
        {
            _skeletonSize = size;
        }

        Vector2f SkeletonPanelDrawable::GetSize(float scale, const Vector2f& maxSize) const
        {
            if (_skeletonSize.LengthSquared() > 0)
            {
                _skeleton->SetScale(1.0f);

                Rectanglef skeletonBounds = _skeleton->GetDrawBounds();
                float widthScale = (_skeletonSize.X > 0.0f ? _skeletonSize.X : std::numeric_limits<float>::max()) / skeletonBounds.W;
                float heightScale = (_skeletonSize.Y > 0.0f ? _skeletonSize.Y : std::numeric_limits<float>::max()) / skeletonBounds.H;
                float finalScale = Min(widthScale, heightScale);

                _skeleton->SetScale(finalScale);
                return Vector2f::Min(_skeleton->GetDrawBounds().Size * scale, maxSize);
            }
            else
            {
                _skeleton->SetScale(_skeletonScale * scale);
                _skeleton->SetRotation(_skeletonRotation);
                return Vector2f::Min(_skeleton->GetDrawBounds().Size, maxSize);
            }
        }

        void SkeletonPanelDrawable::Draw(Graphics::SpriteRenderer* spriteRenderer, const Color& color, const RotatedRectanglef& bounds, float scale) const
        {
            _skeleton->SetColor(_skeletonColor * color);

            _skeleton->SetScale(1.0f);
            _skeleton->SetRotation(_skeletonRotation);
            Rectanglef skeletonBounds = _skeleton->GetDrawBounds();

            float widthScale = bounds.Width() / skeletonBounds.W;
            float heightScale = bounds.Height() / skeletonBounds.H;
            float finalScale = Min(widthScale, heightScale);

            _skeleton->SetScale(finalScale);
            _skeleton->SetRotation(_skeletonRotation + bounds.R);

            skeletonBounds = _skeleton->GetDrawBounds();
            _skeleton->SetPosition(_skeleton->GetPosition() - skeletonBounds.Middle() + bounds.Center);

            _skeleton->Draw(spriteRenderer);
        }
    }
}
