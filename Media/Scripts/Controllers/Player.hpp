#pragma once

#include "Level/LevelInstance.hpp"
#include "Level/LevelLayerInstance.hpp"
#include "Character/Controller.hpp"
#include "Character/Character.hpp"
#include "Graphics/MaterialSet.hpp"
#include "Ability/Ability.hpp"
#include "Random.hpp"

#include "CharacterSet.hpp"
#include "Characters/Dwarves/Dwarf.hpp"
#include "DwarfNameGenerator.hpp"

namespace Dwarf
{
    namespace Settings
    {
        class TheDeepDeepProfile;
    }

    namespace HUD
    {
        class MenuDisplay;
        class ActionDisplay;
        class CharacterSelector;
        class SelectionArea;
        class Minimap;
        class PauseButton;
        class ResourceDisplay;
        class Debugger;
        class TutorialDisplay;
    }

    namespace Character
    {
        class Player : public Controller
        {
        public:
            Player(const ControllerParameters& parameters);

            void OnLevelSuccessfullyCompleted(Settings::TheDeepDeepProfile* profile) const;

            void OnLevelVictory();
            void OnLevelFailed();

        protected:
            virtual void OnCreate() override;
            virtual void OnDestroy() override;

            virtual void OnStartCutscene() override;
            virtual void OnEndCutscene() override;

            virtual void OnAddCharacter(Character* character) override;
            virtual void OnRemoveCharacter(Character* character) override;

            virtual void OnLoadContent(Content::ContentManager* contentManager) override;
            virtual void OnUnloadContent() override;

            virtual void OnUpdate(const Input::FrameInput& input, double totalTime, float dt) override;

            virtual void OnDraw(Level::LevelLayerInstance* layer, Graphics::LevelRenderer* levelRenderer) const override;
            virtual void OnDrawHUD(Graphics::SpriteRenderer* spriteRenderer) const override;

        private:
            enum PlayerContext
            {
                Context_None,
                Context_Attack,
                Context_Move,
                Context_Ability,
                Context_Selecting,
            };

            Rectanglef getSelectionRectangle() const;

            Character* getPrimaryCharacter() const;

            void setContext(PlayerContext newContext);

            void panCamera(const Vector2f& mousePos, const Rectanglef& viewport, const Input::FrameInput& input, float dt);

            void SelectedCharactersMove(const Vector2f& destination, bool queue);
            void SelectedCharactersAttack(const Vector2f& destination, bool forced, bool queue);
            void SelectedCharactersInteract(const Vector2f& destination, bool queue);
            void SelectedCharactersDoAction(const Vector2f& destination, bool queue);
            void PlaySelectedCharacterAffirmative();
            void PlaySelectedCharacterNegatory();

            bool isCharacterHighlightable(const Character* target) const;
            bool isItemHighlightable(const Item::Item* target) const;

            void PositionHUDButtons(const Rectangleu& viewport, float scale);

            void DrawSelectionRectangle(Graphics::SpriteRenderer* spriteRenderer, const Camera& worldCam, const Camera& hudCam, const Rectanglef& rect, float thickness,
                                        const Color& color) const;

            const App::Cursor* getCursor() const;

            void iterateSelectedCharacter();
            void clearSelection();

            void animateHUDIn(bool instant);
            void animateHUDOut(bool instant);

            void pause();
            void unpause();

            Rectangleu _prevViewport;
            float _uiScale;

            std::list<Input::InputBindCode> _availableCharacterBinds;

            Input::InputBindCode _hudSelectBind;

            Input::InputBindCode _moveUpBind;
            Input::InputBindCode _moveDownBind;
            Input::InputBindCode _moveLeftBind;
            Input::InputBindCode _moveRightBind;

            Input::InputBindCode _attackBind;
            Input::InputBindCode _moveBind;
            Input::InputBindCode _stopBind;
            Input::InputBindCode _holdBind;
            std::vector<Input::InputBindCode> _abilityBinds;

            Input::InputBindCode _pauseBind;

            Input::InputBindCode _showDebugUIBind;
            std::vector<Input::InputBindCode> _debugBinds;

            Input::InputBindCode _queueBind;
            Input::InputBindCode _cancelBind;
            Input::InputBindCode _holdPositionBind;

            Input::InputBindCode _selectAllBind;
            Input::InputBindCode _iterateSelectionBind;

            Input::InputBindCode _selectBind;
            Input::InputBindCode _actionBind;

            Vector2f _selectionOrigin;
            Vector2f _selectionExtent;

            PlayerContext _curContext;

            CharacterSet _allCharacters;

            uint32_t _selectedCharacterTypeIdx;
            uint32_t _selectedCharacterIdx;
            CharacterSet _selectedCharacters;

            Vector2f _mousePosScreen;
            Vector2f _mousePosWorld;

            Ability::Ability* _activeAbility;
            bool _setFirstAbilityPosition;

            float _minScrollDist;
            float _camMoveSpeed;

            float _targetZoom;
            float _minZoom;
            float _maxZoom;
            float _zoomSpeed;

            bool _mouseLightOn;

            HUD::Minimap* _minimap;
            HUD::SelectionArea* _hudSelectionArea;
            HUD::CharacterSelector* _hudCharacterSelector;
            HUD::ActionDisplay* _hudActionDisplay;
            HUD::MenuDisplay* _hudMenuDisplay;
            HUD::PauseButton* _hudPauseButton = nullptr;
            HUD::ResourceDisplay* _hudResourceDisplay = nullptr;
            HUD::TutorialDisplay* _tutorialDisplay = nullptr;

            HUD::Debugger* _debugger;

            const App::Cursor* _defaultCursor;
            std::map<PlayerContext, const App::Cursor*> _cursors;
            const App::Cursor* _defaultInteractCursor;

            std::unordered_map<CharacterID, Color> _highlightCharacters;
            std::unordered_map<Item::ItemID, Color> _highlightItems;
            const App::Cursor* _interactiveSelectionCursor;

            const HUD::Panel* _tooltip;

            bool _inCutscene;
            bool _paused;
            bool _hudVisible;

            bool _onVictoryScreen;
            bool _onDefeatScreen;

            bool _firstUpdate;

            float _affirmativeSoundResetTimer = 0.0f;
            float _negatorySoundResetTimer = 0.0f;

            float _timePlayingLevel = 0.0f;
            bool _givenActionCommand = false;
            bool _selectionChanged = false;
            bool _primarySelectionChanged = false;
            float _timeHavingAbilities = 0.0f;
            bool _abilityUsed = false;
        };
    }

    template <>
    void EnumeratePreloads<Character::Player>(PreloadSet& preloads);
}
