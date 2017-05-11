#pragma once

#include "Content/ContentManager.hpp"
#include "Content/Preload.hpp"
#include "Localization/StringTable.hpp"
#include "Input/FrameInput.hpp"
#include "Graphics/SpriteRenderer.hpp"
#include "HUD/Panel.hpp"
#include "Animation/SkeletonInstance.hpp"

namespace Dwarf
{
    namespace HUD
    {
        class PauseButton final
        {
        public:
            PauseButton(Content::ContentManager* contentManager, const Localization::StringTable* strings, Input::InputBindCode clickBind);
            ~PauseButton();

            void SetPosition(const Vector2f& pos, float scale);

            void AnimateIn(bool instant);
            void AnimateOut(bool instant);

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

            Animation::SkeletonInstance* _skeleton = nullptr;
            const Graphics::PolygonMaterialSet* _unselectedMatset = nullptr;
            const Graphics::PolygonMaterialSet* _selectedMatset = nullptr;

            const Panel* _tooltip = nullptr;
        };
    }

    template <>
    void EnumeratePreloads<HUD::PauseButton>(PreloadSet& preloads);
}
