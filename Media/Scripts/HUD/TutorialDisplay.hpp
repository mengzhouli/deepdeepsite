#pragma once

#include "Content/Preload.hpp"
#include "Content/ContentManager.hpp"

#include "Settings/TheDeepDeepProfile.hpp"
#include "Tutorials.hpp"

#include <set>

namespace Dwarf
{
    namespace HUD
    {
        class TutorialDisplay final
        {
        public:
            TutorialDisplay(Content::ContentManager* contentManager, const Localization::StringTable* strings, Audio::SoundManager* soundManager,
                            Input::InputBindCode clickBind, Settings::TheDeepDeepProfile* profile);
            ~TutorialDisplay();

            void AnimateIn(bool instant);
            void AnimateOut(bool instant);

            void ShowTutorial(TutorialType type, const Vector2f& hudPos);
            void StopShowingTutorial(TutorialType type);
            bool HasShownTutorial(TutorialType type);
            const std::set<TutorialType>& GetShownTutorials() const;

            void SetScale(float scale);

            bool IsMouseOver() const;

            void Update(double totalTime, float dt, const Input::FrameInput& input, const Camera& camera);
            void Draw(Graphics::SpriteRenderer* spriteRenderer) const;

        private:
            ResourcePointer<Content::ContentManager> _contentManager;
            ResourcePointer<const Localization::StringTable> _strings;
            Audio::SoundManager* _soundManager = nullptr;
            Settings::TheDeepDeepProfile* _profile = nullptr;

            struct visibleTutorial
            {
                ResourcePointer<const Panel> panel;
                Vector2f position;
                float alpha = 0.0f;
                bool clicked = false;
            };
            std::map<TutorialType, visibleTutorial> _visibleTutorials;
            std::set<TutorialType> _shownTutorials;

            Input::InputBindCode _clickBind = Input::InputBindCode_Invalid;

            Audio::SoundSet _popupSounds;
            Audio::SoundSet _closeSounds;

            bool _mouseOver = false;
            bool _visible = false;

            float _scale = 1.0f;
        };
    }

    template <>
    void EnumeratePreloads<HUD::TutorialDisplay>(PreloadSet& preloads);
}
