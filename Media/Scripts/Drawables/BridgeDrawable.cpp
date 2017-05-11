#include "Drawables/BridgeDrawable.hpp"
#include "Drawables/RopeUtility.hpp"

namespace Dwarf
{
    namespace Graphics
    {
        BridgeDrawable::BridgeDrawable()
            : _leftSide()
            , _rightSide()
            , _buildPerc(1.0f)
            , _buildDirection(1)
            , _color(Color::White)
            , _pathChain()
            , _contentLoaded(false)
            , _links()
            , _leftEnd()
            , _rightEnd()
            , _rope(NULL)
            , _bounds()
        {
        }

        void BridgeDrawable::SetEndpoints(const Vector2f& a, const Vector2f& b)
        {
            Vector2f left;
            Vector2f right;
            int32_t dir;
            if (a.X < b.X)
            {
                left = a;
                right = b;
                dir = 1;
            }
            else
            {
                left = b;
                right = a;
                dir = -1;
            }

            if (_buildDirection != dir ||
                Vector2f::DistanceSquared(_leftSide, left) > 0.1f ||
                Vector2f::DistanceSquared(_rightSide, right) > 0.1f)
            {
                _leftSide = left;
                _rightSide = right;
                _buildDirection = dir;
                UpdateBridgeSegments();
            }
        }

        void BridgeDrawable::SetColor(const Color& color)
        {
            _color = color;
            UpdateLinkColors();
        }

        void BridgeDrawable::SetBuildPercent(float buildPerc)
        {
            _buildPerc = buildPerc;
            UpdateLinkColors();
        }

        const Chainf& BridgeDrawable::GetPathChain() const
        {
            return _pathChain;
        }

        void BridgeDrawable::LoadContent(Content::ContentManager* contentManager)
        {
            const Animation::Skeleton* endSkel = contentManager->Load<Animation::Skeleton>("Skeletons/Characters/Bridge/bridge_end.skel");
            const Graphics::PolygonMaterialSet* endMatset = contentManager->Load<Graphics::PolygonMaterialSet>("Skeletons/Characters/Bridge/bridge_end.polymatset");

            _leftEnd = new Animation::SkeletonInstance(endSkel, endMatset);
            _rightEnd = new Animation::SkeletonInstance(endSkel, endMatset);

            SafeRelease(endSkel);
            SafeRelease(endMatset);

            _linkSkeleton = contentManager->Load<Animation::Skeleton>("Skeletons/Characters/Bridge/bridge_link.skel");
            _linkMaterialSet = contentManager->Load<Graphics::PolygonMaterialSet>("Skeletons/Characters/Bridge/bridge_link.polymatset");

            _rope = new Graphics::RopeDrawable(contentManager);

            _contentLoaded = true;

            UpdateBridgeSegments();
        }

        void BridgeDrawable::UnloadContent()
        {
            SafeRelease(_linkSkeleton);
            SafeRelease(_linkMaterialSet);
            SafeReleaseAndClearContainer(_links);
            SafeRelease(_leftEnd);
            SafeRelease(_rightEnd);
            SafeRelease(_rope);
            _contentLoaded = false;
        }

        void BridgeDrawable::Update(double totalTime, float dt)
        {
            _leftEnd->Update(totalTime, dt);
            _rightEnd->Update(totalTime, dt);
            for (uint32_t i = 0; i < _links.size(); i++)
            {
                _links[i]->Update(totalTime, dt);
            }
        }

        const Rectanglef& BridgeDrawable::GetDrawBounds() const
        {
            return _bounds;
        }

        void BridgeDrawable::Draw(Graphics::SpriteRenderer* spriteRenderer) const
        {
            _rope->Draw(spriteRenderer);
            _leftEnd->Draw(spriteRenderer);
            _rightEnd->Draw(spriteRenderer);
            _rightEnd->Draw(spriteRenderer);
            for (uint32_t i = 0; i < _links.size(); i++)
            {
                _links[i]->Draw(spriteRenderer);
            }
        }

        void BridgeDrawable::UpdateBridgeSegments()
        {
            if (!_contentLoaded)
            {
                return;
            }

            if (_links.empty())
            {
                _links.push_back(new Animation::SkeletonInstance(_linkSkeleton, _linkMaterialSet));
            }

            Animation::SkeletonInstance* linkSkeleton = _links.front();
            linkSkeleton->SetScale(1.0f);

            const Vector2f& leftLink = linkSkeleton->GetJointPosition("left_link");
            const Vector2f& rightLink = linkSkeleton->GetJointPosition("right_link");
            float linkWidth = Vector2f::Distance(leftLink, rightLink);

            float gapWidth = Vector2f::Distance(_leftSide, _rightSide);

            uint32_t numLinks = uint32_t(Ceiling(gapWidth / linkWidth));
            float bridgeWidth = Max(numLinks * linkWidth, 1.0f);
            float bridgeScale = gapWidth / bridgeWidth;

            while (numLinks > 0 && _links.size() >= numLinks)
            {
                SafeRelease(_links.back());
                _links.resize(_links.size() - 1);
            }
            while (_links.size() < numLinks)
            {
                _links.push_back(new Animation::SkeletonInstance(_linkSkeleton, _linkMaterialSet));
            }

            Rotatorf bridgeRotation(0.0f);

            _leftEnd->SetPosition(_leftSide);
            _leftEnd->SetRotation(Rotatorf::Reflect(bridgeRotation, Rotatorf::Pi));
            _leftEnd->SetScale(bridgeScale);
            _leftEnd->SetInvertedX(true);

            _rightEnd->SetPosition(_rightSide);
            _rightEnd->SetRotation(bridgeRotation);
            _rightEnd->SetScale(bridgeScale);
            _rightEnd->SetInvertedX(false);

            _bounds = Rectanglef::Merge(_leftEnd->GetDrawBounds(), _rightEnd->GetDrawBounds());

            Chainf pathChain;

            Chainf ropeChain;
            ropeChain.AddPoint(_leftEnd->GetJointPosition("rope_connect"));

            Chainf linkChain = ComputeRopePositions(_leftSide, _rightSide, numLinks, -Vector2f::UnitY, 1.25f);

            Vector2f prevRightWalk;
            for (uint32_t i = 0; i < linkChain.Size() - 1; i++)
            {
                Animation::SkeletonInstance* link = _links[i];

                link->SetColor(Color::FromFloats(1.0f, 1.0f, 1.0f, 0.0f));
                
                const Vector2f& leftPos = linkChain[i];
                const Vector2f& rightPos = linkChain[i + 1];

                Vector2f centerPos = (leftPos + rightPos) * 0.5f;

                float linkScale = Vector2f::Distance(leftPos, rightPos) / linkWidth;

                Rotatorf linkRotation(rightPos - leftPos);

                link->SetPosition(centerPos);
                link->SetRotation(linkRotation);
                link->SetScale(linkScale);

                const Vector2f& rightWalk = link->GetJointPosition("right_walk");
                const Vector2f& leftWalk = link->GetJointPosition("left_walk");
                if (i == 0)
                {
                    pathChain.AddPoint(_leftSide);
                }
                else
                {
                    pathChain.AddPoint((leftWalk + prevRightWalk) * 0.5f);
                }
                prevRightWalk = rightWalk;

                ropeChain.AddPoint(link->GetJointPosition("rope_connect"));

                _bounds = Rectanglef::Merge(_bounds, link->GetDrawBounds());
            }

            pathChain.AddPoint(_rightSide);
            _pathChain = pathChain;

            ropeChain.AddPoint(_rightEnd->GetJointPosition("rope_connect"));

            _rope->SetChain(ropeChain);

            UpdateLinkColors();
        }

        void BridgeDrawable::UpdateLinkColors()
        {
            if (!_contentLoaded)
            {
                return;
            }

            Color color = _color;

            _rope->SetColor(color);
            _leftEnd->SetColor(color);
            _rightEnd->SetColor(color);

            if (_links.size() > 0)
            {
                float buildPercPerLink = 1.0f / _links.size();
                for (uint32_t i = 0; i < _links.size(); i++)
                {
                    uint32_t linkIdx = (_buildDirection > 0) ? i : _links.size() - 1 - i;
                    float linkBuildPerc = Clamp((_buildPerc - (i * buildPercPerLink)) / buildPercPerLink, 0.0f, 1.0f);

                    color.A = uint8_t(((_color.A / 255.0f) * linkBuildPerc) * 255);

                    _links[linkIdx]->SetColor(color);
                }
            }
        }
    }

    template <>
    void EnumeratePreloads<Graphics::BridgeDrawable>(PreloadSet& preloads)
    {
        EnumeratePreloads<Graphics::RopeDrawable>(preloads);
        preloads.insert("Skeletons/Characters/Bridge/bridge_link.skel");
        preloads.insert("Skeletons/Characters/Bridge/bridge_link.polymatset");
        preloads.insert("Skeletons/Characters/Bridge/bridge_end.skel");
        preloads.insert("Skeletons/Characters/Bridge/bridge_end.polymatset");
        preloads.insert("Skeletons/Characters/Rope/rope.png");
    }
}
