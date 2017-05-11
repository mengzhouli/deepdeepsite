#include "HUD/PauseButton.hpp"

#include "HUD/Tooltip.hpp"
#include "ContentUtility.hpp"

namespace Dwarf
{
    namespace HUD
    {
        static const std::string PauseButtonSkeletonPath = "Skeletons/UI/PauseButton/pause_button.skel";
        static const std::string PauseButtonUnselectedMatsetPath = "Skeletons/UI/PauseButton/pause_button_unselected.polymatset";
        static const std::string PauseButtonSelectedMatsetPath = "Skeletons/UI/PauseButton/pause_button_selected.polymatset";

        static const std::string MoveInAnimation = "move_in";
        static const std::string MoveOutAnimation = "move_out";

        PauseButton::PauseButton(Content::ContentManager* contentManager, const Localization::StringTable* strings, Input::InputBindCode clickBind)
            : _clickBind(clickBind)
        {
            _skeleton = Content::CreateSkeletonInstance(contentManager, PauseButtonSkeletonPath, PauseButtonUnselectedMatsetPath);
            _skeleton->PlayAnimation(MoveOutAnimation, false, 0.0f, _skeleton->GetAnimationLength(MoveOutAnimation));
            _unselectedMatset = contentManager->Load<Graphics::PolygonMaterialSet>(PauseButtonUnselectedMatsetPath);
            _selectedMatset = contentManager->Load<Graphics::PolygonMaterialSet>(PauseButtonSelectedMatsetPath);
            _tooltip = CreateSimpleTextTooltip(contentManager, strings->GetString("pause_tooltip"));
        }

        PauseButton::~PauseButton()
        {
            SafeRelease(_skeleton);
            SafeRelease(_unselectedMatset);
            SafeRelease(_selectedMatset);
            SafeRelease(_tooltip);
        }

        void PauseButton::SetPosition(const Vector2f& pos, float scale)
        {
            _skeleton->SetPosition(pos);
            _skeleton->SetScale(scale);
        }

        void PauseButton::AnimateIn(bool instant)
        {
            _skeleton->PlayAnimation(MoveInAnimation, false, 0.0f, instant ? _skeleton->GetAnimationLength(MoveInAnimation) : 0.0f);
        }

        void PauseButton::AnimateOut(bool instant)
        {
            _skeleton->PlayAnimation(MoveOutAnimation, false, 0.0f, instant ? _skeleton->GetAnimationLength(MoveOutAnimation) : 0.0f);
        }

        const Rectanglef& PauseButton::GetBounds() const
        {
            return _skeleton->GetDrawBounds();
        }

        bool PauseButton::IsMouseOver() const
        {
            return _mouseOver;
        }

        bool PauseButton::IsJustClicked() const
        {
            return _justClicked;
        }

        const Panel* PauseButton::GetTooltip() const
        {
            return _tooltip;
        }

        void PauseButton::Update(double totalTime, float dt, const Input::FrameInput& input, const Camera& camera)
        {
            Vector2f mousePos = input.UnProjectMousePosition(camera);
            _skeleton->Update(totalTime, dt);
            _mouseOver = _skeleton->ContainsPoint(mousePos);
            _justClicked = _mouseOver && input.IsBindJustPressed(_clickBind);
            _skeleton->MergeMaterialSet(_mouseOver ? _selectedMatset : _unselectedMatset);
        }

        void PauseButton::Draw(Graphics::SpriteRenderer* spriteRenderer) const
        {
            _skeleton->Draw(spriteRenderer);
        }
    }

    template <>
    void EnumeratePreloads<HUD::PauseButton>(PreloadSet& preloads)
    {
        EnumerateTooltipPreloads(preloads);
        preloads.insert(HUD::PauseButtonSkeletonPath);
        preloads.insert(HUD::PauseButtonUnselectedMatsetPath);
        preloads.insert(HUD::PauseButtonSelectedMatsetPath);
    }
}
