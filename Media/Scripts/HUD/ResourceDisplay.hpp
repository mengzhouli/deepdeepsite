#pragma once

#include "Content/ContentManager.hpp"
#include "Content/Preload.hpp"
#include "Localization/StringTable.hpp"
#include "Input/FrameInput.hpp"
#include "Graphics/SpriteRenderer.hpp"
#include "HUD/Panel.hpp"
#include "Animation/SkeletonInstance.hpp"
#include "Item/Resources.hpp"

namespace Dwarf
{
    namespace HUD
    {
        class ResourceDisplay final
        {
        public:
            ResourceDisplay(Content::ContentManager* contentManager, const Localization::StringTable* strings, Input::InputBindCode clickBind);
            ~ResourceDisplay();

            void SetPosition(const Vector2f& pos, float scale);

            void AnimateIn(bool instant);
            void AnimateOut(bool instant);

            void SetResources(const Item::Resources& resources);

            const Rectanglef& GetBounds() const;
            bool IsMouseOver() const;
            bool IsJustClicked() const;
            const Panel* GetTooltip() const;

            void Update(double totalTime, float dt, const Input::FrameInput& input, const Camera& camera);
            void Draw(Graphics::SpriteRenderer* spriteRenderer) const;

        private:
            Input::InputBindCode _clickBind = Input::InputBindCode_Invalid;
            bool _mouseOver = false;
            bool _justClicked = false;

            ResourcePointer<const Graphics::Font> _font;
            float _resourceDisplaySpeed = 40.0f;
            float _displayGold = 0.0f;
            Item::Resources _currentResources;

            ResourcePointer<Animation::SkeletonInstance> _panelSkeleton;

            ResourcePointer<Animation::SkeletonInstance> _goldSkeleton;
            uint32_t _currentGoldIcon = 0;
            std::vector<ResourcePointer<const Graphics::PolygonMaterialSet>> _goldSizeMaterials;

            ResourcePointer<const Panel> _tooltip;
            float _scale = 1.0f;
        };
    }

    template <>
    void EnumeratePreloads<HUD::ResourceDisplay>(PreloadSet& preloads);
}
