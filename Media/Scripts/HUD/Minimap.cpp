#include "HUD/Minimap.hpp"

#include "ContentUtility.hpp"

namespace Dwarf
{
    namespace HUD
    {
        static const std::string PanelSkeletonPath = "Skeletons/UI/Minimap/minimap.skel";
        static const std::string PanelMatsetPath = "Skeletons/UI/Minimap/minimap.polymatset";

        static const std::string MoveInAnimation = "move_in";
        static const std::string MoveOutAnimation = "move_out";

        static const std::string PanelRoot = "origin";
        static const std::pair<std::string, std::string> PanelMinimapArea{ "minimap_pos", "minimap_size" };

        static RotatedRectanglef getRotatedRectangle(const Animation::SkeletonInstance* skeleton, const std::string& rootJoint,
            const std::pair<std::string, std::string>& jointNames)
        {
            Rotatorf rotation = -skeleton->GetJointTransformation(rootJoint).Rotation;
            const Vector2f& posA = skeleton->GetJointPosition(jointNames.first);
            const Vector2f& posB = skeleton->GetJointPosition(jointNames.second);
            Vector2f origin = (posA + posB) * 0.5f;

            Vector2f rotatedA = Vector2f::Rotate(origin, posA, -rotation.Angle);
            Vector2f rotatedB = Vector2f::Rotate(origin, posB, -rotation.Angle);
            Vector2f extents = Vector2f::Abs(rotatedB - rotatedA) * 0.5f;

            return RotatedRectanglef(origin, extents, rotation);
        }

        Minimap::Minimap(Graphics::Minimap* minimap, Content::ContentManager* contentManager, Input::InputBindCode clickBind)
            : _clickBind(clickBind)
            , _panel(nullptr)
            , _minimap(minimap)
            , _mouseOver(false)
            , _clicked(false)
            , _worldPosition()
        {
            assert(_minimap && contentManager);
            SafeAddRef(_minimap);

            _panel = Content::CreateSkeletonInstance(contentManager, PanelSkeletonPath, PanelMatsetPath);
            _panel->PlayAnimation(MoveOutAnimation, false, 0.0f, _panel->GetAnimationLength(MoveOutAnimation));

            updateMinimapSize();
        }

        Minimap::~Minimap()
        {
            SafeRelease(_minimap);
            SafeRelease(_panel);
        }

        void Minimap::SetPosition(const Vector2f& pos, float scale)
        {
            _panel->SetPosition(pos);
            _panel->SetScale(scale);
        }

        const Rectanglef& Minimap::GetBounds() const
        {
            return _panel->GetDrawBounds();
        }

        float Minimap::GetScale() const
        {
            return _panel->GetScale();
        }

        void Minimap::AnimateIn(bool instant)
        {
            _panel->PlayAnimation(MoveInAnimation, false, 0.0f, instant ? _panel->GetAnimationLength(MoveInAnimation) : 0.0f);
        }

        void Minimap::AnimateOut(bool instant)
        {
            _panel->PlayAnimation(MoveOutAnimation, false, 0.0f, instant ? _panel->GetAnimationLength(MoveOutAnimation) : 0.0f);
        }

        bool Minimap::IsMouseOver() const
        {
            return _mouseOver;
        }

        bool Minimap::IsClicked(Vector2f& worldPosition) const
        {
            worldPosition = _worldPosition;
            return _clicked;
        }

        void Minimap::Update(double totalTime, float dt, const Input::FrameInput& input, const Camera& camera)
        {
            _panel->Update(totalTime, dt);
            updateMinimapSize();

            Vector2f mousePos = input.UnProjectMousePosition(camera);

            RotatedRectanglef minimapArea = getRotatedRectangle(_panel, PanelRoot, PanelMinimapArea);
            if (RotatedRectanglef::Contains(minimapArea, mousePos))
            {
                Vector2f rightDir = Vector2f::Normalize(minimapArea.TopRight() - minimapArea.TopLeft());
                Vector2f downDir = Vector2f::Normalize(minimapArea.BottomLeft() - minimapArea.TopLeft());

                Vector2f mouseDir = mousePos - minimapArea.TopLeft();

                Vector2f coords = Vector2f(Vector2f::Dot(mouseDir, rightDir), Vector2f::Dot(mouseDir, downDir));

                Vector2f rescale = Vector2f(_minimap->GetSize()) / (minimapArea.Extents * 2.0f);
                Vector2f relPos = coords * rescale;

                _worldPosition = _minimap->Unproject(relPos);
            }

            _mouseOver = _panel->ContainsPoint(mousePos);
            if (!input.IsBindPressed(_clickBind))
            {
                _clicked = false;
            }
            else if (_mouseOver && input.IsBindJustPressed(_clickBind))
            {
                _clicked = true;
            }
        }


        void Minimap::Draw(Graphics::SpriteRenderer* spriteRenderer) const
        {
            RotatedRectanglef minimapArea = getRotatedRectangle(_panel, PanelRoot, PanelMinimapArea);
            _minimap->Draw(spriteRenderer, minimapArea);

            _panel->Draw(spriteRenderer);
        }

        void Minimap::updateMinimapSize()
        {
            RotatedRectanglef minimapArea = getRotatedRectangle(_panel, PanelRoot, PanelMinimapArea);
            _minimap->SetSize(Vector2u(minimapArea.Width(), minimapArea.Height()) * 2u);
        }
    }

    template <>
    void EnumeratePreloads<HUD::Minimap>(PreloadSet& preloads)
    {
        preloads.insert(HUD::PanelSkeletonPath);
        preloads.insert(HUD::PanelMatsetPath);
    }
}
