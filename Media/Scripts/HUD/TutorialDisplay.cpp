#include "HUD/TutorialDisplay.hpp"

#include "HUD/Tooltip.hpp"

namespace Dwarf
{
    namespace HUD
    {
        static const float TutorialFadeTime = 10.0f;

        static const Audio::SoundPathVector TutorialPopupSounds =
        {
            { "Audio/Items/UI/item_cloth_1.wav", 1.0f },
        };

        static const Audio::SoundPathVector TutorialCloseSounds =
        {
            { "Audio/Items/UI/item_cloth_2.wav", 1.0f },
        };

        TutorialDisplay::TutorialDisplay(Content::ContentManager* contentManager, const Localization::StringTable* strings, Audio::SoundManager* soundManager,
            Input::InputBindCode clickBind, Settings::TheDeepDeepProfile* profile)
            : _contentManager(contentManager)
            , _strings(strings)
            , _soundManager(soundManager)
            , _profile(profile)
            , _clickBind(clickBind)
            , _popupSounds(TutorialPopupSounds)
            , _closeSounds(TutorialCloseSounds)
        {
            _popupSounds.LoadContent(contentManager);
            _closeSounds.LoadContent(contentManager);
        }

        TutorialDisplay::~TutorialDisplay()
        {
            _popupSounds.UnloadContent();
            _closeSounds.UnloadContent();
        }

        void TutorialDisplay::AnimateIn(bool instant)
        {
            _visible = true;
            if (instant)
            {
                for (auto& tutorial : _visibleTutorials)
                {
                    tutorial.second.alpha = 1.0f;
                }
            }
        }

        void TutorialDisplay::AnimateOut(bool instant)
        {
            _visible = false;
            if (instant)
            {
                for (auto& tutorial : _visibleTutorials)
                {
                    tutorial.second.alpha = 0.0f;
                }
            }
        }

        void TutorialDisplay::ShowTutorial(TutorialType type, const Vector2f& hudPos)
        {
            if (_visibleTutorials.find(type) != _visibleTutorials.end())
            {
                return;
            }

            visibleTutorial tutorial;
            tutorial.panel = EmplaceResource(CreateTutorialTooltip(_contentManager, _strings, type, _profile));
            tutorial.position = hudPos;
            _visibleTutorials[type] = tutorial;

            _shownTutorials.insert(type);

            _soundManager->PlaySingleGlobalSound(_popupSounds.GetNextSound(), Audio::SoundPriority::High, 1.0f);
        }

        void TutorialDisplay::StopShowingTutorial(TutorialType type)
        {
            auto iter = _visibleTutorials.find(type);
            if (iter != _visibleTutorials.end())
            {
                iter->second.clicked = true;
                _soundManager->PlaySingleGlobalSound(_closeSounds.GetNextSound(), Audio::SoundPriority::High, 1.0f);
            }
            _shownTutorials.insert(type);
        }

        bool TutorialDisplay::HasShownTutorial(TutorialType type)
        {
            return _profile->HasTutorialBeenShown(type) || _shownTutorials.find(type) != _shownTutorials.end();
        }

        const std::set<TutorialType>& TutorialDisplay::GetShownTutorials() const
        {
            return _shownTutorials;
        }

        void TutorialDisplay::SetScale(float scale)
        {
            _scale = scale;
        }

        bool TutorialDisplay::IsMouseOver() const
        {
            return _mouseOver;
        }

        static Rectanglef GetTooltipBounds(ResourcePointer<const Panel> panel, const Vector2f& position, float scale, TutorialType type)
        {
            Vector2f minSize = panel->GetMinimumSize(scale);
            switch (GetTutorialArrowCorner(type))
            {
            case TutorialArrowCorner::None:
                return Rectanglef(position - (minSize * 0.5f), minSize);

            case TutorialArrowCorner::TopRight:
                return Rectanglef(position - Vector2f(minSize.X, 0.0f), minSize);

            case TutorialArrowCorner::BottomLeft:
                return Rectanglef(position + Vector2f(0.0f, minSize.Y), minSize);

            case TutorialArrowCorner::BottomRight:
                return Rectanglef(position - minSize, minSize);

            case TutorialArrowCorner::TopLeft:
            default:
                return Rectanglef(position, minSize);
            }
        }

        void TutorialDisplay::Update(double totalTime, float dt, const Input::FrameInput& input, const Camera& camera)
        {
            _mouseOver = false;

            Vector2f mousePos = input.UnProjectMousePosition(camera);

            auto iter = _visibleTutorials.begin();
            while (iter != _visibleTutorials.end())
            {
                visibleTutorial& tutorial = iter->second;

                if (tutorial.alpha > 0.0f &&
                    Rectanglef::Contains(GetTooltipBounds(tutorial.panel, tutorial.position, _scale, iter->first), mousePos))
                {
                    _mouseOver = true;
                    if (!tutorial.clicked && input.IsBindJustPressed(_clickBind))
                    {
                        tutorial.clicked = true;
                        _soundManager->PlaySingleGlobalSound(_closeSounds.GetNextSound(), Audio::SoundPriority::High, 1.0f);
                    }
                }

                float deltaAlpha = (_visible && !tutorial.clicked) ? TutorialFadeTime * dt : -TutorialFadeTime * dt;
                tutorial.alpha = Saturate(tutorial.alpha + deltaAlpha);

                if (tutorial.clicked && tutorial.alpha <= 0.0f)
                {
                    iter = _visibleTutorials.erase(iter);
                }
                else
                {
                    iter++;
                }
            }
        }

        void TutorialDisplay::Draw(Graphics::SpriteRenderer* spriteRenderer) const
        {
            for (const auto& tutorialIter : _visibleTutorials)
            {
                const visibleTutorial& tutorial = tutorialIter.second;
                tutorial.panel->Draw(spriteRenderer, Color::FromFloats(1.0f, 1.0f, 1.0f, tutorial.alpha),
                                     GetTooltipBounds(tutorial.panel, tutorial.position, _scale, tutorialIter.first), _scale);
            }
        }
    }

    template <>
    void EnumeratePreloads<HUD::TutorialDisplay>(PreloadSet& preloads)
    {
        EnumerateTooltipPreloads(preloads);
        EnumerateSoundVectorPreloads(preloads, HUD::TutorialPopupSounds);
        EnumerateSoundVectorPreloads(preloads, HUD::TutorialCloseSounds);
    }
}

