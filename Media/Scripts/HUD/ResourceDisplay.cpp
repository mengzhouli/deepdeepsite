#include "HUD/ResourceDisplay.hpp"

#include "HUD/Tooltip.hpp"
#include "ContentUtility.hpp"

namespace Dwarf
{
    namespace HUD
    {
        static const std::string ResourceDisplaySkeletonPath = "Skeletons/UI/ResourceDisplay/resource_display.skel";
        static const std::string ResourceDisplayMatsetPath = "Skeletons/UI/ResourceDisplay/resource_display.polymatset";

        static const std::string ResourceDisplayGoldSkeletonPath = "Skeletons/UI/ResourceDisplay/gold_resouce.skel";
        static const std::vector<std::string> ResourceDisplayGoldMatsetPaths =
        {
            "Skeletons/UI/ResourceDisplay/gold_resource_0.polymatset",
            "Skeletons/UI/ResourceDisplay/gold_resource_1.polymatset",
            "Skeletons/UI/ResourceDisplay/gold_resource_2.polymatset",
            "Skeletons/UI/ResourceDisplay/gold_resource_3.polymatset",
            "Skeletons/UI/ResourceDisplay/gold_resource_4.polymatset",
        };
        static const uint32_t GoldPerIcon = 10;

        static const std::pair<std::string, std::string> GoldDisplayJoint = { "gold_text", "gold_text_size" };

        static const std::string MoveInAnimation = "move_in";
        static const std::string MoveOutAnimation = "move_out";

        static const std::string FontPath = "Fonts/hud_font.ttf";

        ResourceDisplay::ResourceDisplay(Content::ContentManager* contentManager, const Localization::StringTable* strings, Input::InputBindCode clickBind)
        {
            _panelSkeleton = EmplaceResource(Content::CreateSkeletonInstance(contentManager, ResourceDisplaySkeletonPath, ResourceDisplayMatsetPath));
            _panelSkeleton->PlayAnimation(MoveOutAnimation, false, 0.0f, _panelSkeleton->GetAnimationLength(MoveOutAnimation));
            _goldSkeleton = EmplaceResource(Content::CreateSkeletonInstance(contentManager, ResourceDisplayGoldSkeletonPath, ResourceDisplayGoldMatsetPaths[0]));
            for (const auto& goldMatsetPath : ResourceDisplayGoldMatsetPaths)
            {
                _goldSizeMaterials.push_back(EmplaceResource(contentManager->Load<Graphics::PolygonMaterialSet>(goldMatsetPath)));
            }
            _tooltip = EmplaceResource(CreateSimpleTextTooltip(contentManager, strings->GetString("resource_display_tooltip")));
            _font = EmplaceResource(contentManager->Load<Graphics::Font>(FontPath));
        }

        ResourceDisplay::~ResourceDisplay()
        {
        }

        void ResourceDisplay::SetPosition(const Vector2f& pos, float scale)
        {
            _panelSkeleton->SetPosition(pos);
            _panelSkeleton->SetScale(scale);
            _scale = scale;
        }

        void ResourceDisplay::AnimateIn(bool instant)
        {
            _panelSkeleton->PlayAnimation(MoveInAnimation, false, 0.0f, instant ? _panelSkeleton->GetAnimationLength(MoveInAnimation) : 0.0f);
        }

        void ResourceDisplay::AnimateOut(bool instant)
        {
            _panelSkeleton->PlayAnimation(MoveOutAnimation, false, 0.0f, instant ? _panelSkeleton->GetAnimationLength(MoveOutAnimation) : 0.0f);
        }

        void ResourceDisplay::SetResources(const Item::Resources& resources)
        {
            if (resources > _currentResources && _goldSkeleton->IsAnimationFinished())
            {
                _goldSkeleton->PlayAnimation("collect_gold", false);
            }
            _currentResources = resources;
        }

        const Rectanglef& ResourceDisplay::GetBounds() const
        {
            return _panelSkeleton->GetDrawBounds();
        }

        bool ResourceDisplay::IsMouseOver() const
        {
            return _mouseOver;
        }

        bool ResourceDisplay::IsJustClicked() const
        {
            return _justClicked;
        }

        const Panel* ResourceDisplay::GetTooltip() const
        {
            return _tooltip;
        }

        void ResourceDisplay::Update(double totalTime, float dt, const Input::FrameInput& input, const Camera& camera)
        {
            Vector2f mousePos = input.UnProjectMousePosition(camera);
            _panelSkeleton->Update(totalTime, dt);

            _goldSkeleton->SetPosition(_panelSkeleton->GetJointPosition("gold_icon"));
            _goldSkeleton->Update(totalTime, dt);

            _mouseOver = _panelSkeleton->ContainsPoint(mousePos);
            _justClicked = _mouseOver && input.IsBindJustPressed(_clickBind);

            if (static_cast<int32_t>(_displayGold) != _currentResources.Gold)
            {
                float diff = static_cast<float>(_currentResources.Gold) - _displayGold;
                float delta = Clamp(Sign(diff) * _resourceDisplaySpeed * dt, -Abs(diff), Abs(diff));
                _displayGold += delta;
            }
            else
            {
                _displayGold = static_cast<float>(_currentResources.Gold);
            }

            uint32_t targetGoldIcon = Min(_displayGold / GoldPerIcon, _goldSizeMaterials.size() - 1);
            if (targetGoldIcon != _currentGoldIcon)
            {
                _currentGoldIcon = targetGoldIcon;
                _goldSkeleton->MergeMaterialSet(_goldSizeMaterials[_currentGoldIcon]);
            }
        }

        void ResourceDisplay::Draw(Graphics::SpriteRenderer* spriteRenderer) const
        {
            _panelSkeleton->Draw(spriteRenderer);
            _goldSkeleton->Draw(spriteRenderer);

            Vector2f goldDisplayPos = _panelSkeleton->GetJointPosition(GoldDisplayJoint.first);
            Rectanglef goldDisplayArea(goldDisplayPos, _panelSkeleton->GetJointPosition(GoldDisplayJoint.second) - goldDisplayPos);
            Graphics::PreparedText preparedText(Format("%i", static_cast<int32_t>(_displayGold)), _font, goldDisplayArea.H, Color::Gold);
            spriteRenderer->DrawString(preparedText, goldDisplayArea.BottomRight() - Vector2f(preparedText.GetSize()), Color::White);
        }
    }

    template <>
    void EnumeratePreloads<HUD::ResourceDisplay>(PreloadSet& preloads)
    {
        preloads.insert(HUD::ResourceDisplaySkeletonPath);
        preloads.insert(HUD::ResourceDisplayMatsetPath);
        preloads.insert(HUD::ResourceDisplayGoldSkeletonPath);
        preloads.insert(HUD::ResourceDisplayGoldMatsetPaths.begin(), HUD::ResourceDisplayGoldMatsetPaths.end());
        preloads.insert(HUD::FontPath);
        EnumerateTooltipPreloads(preloads);
    }
}
