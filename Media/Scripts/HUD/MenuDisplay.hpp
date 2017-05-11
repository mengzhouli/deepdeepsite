#pragma once

#include "Graphics/Text/Font.hpp"
#include "Localization/StringTable.hpp"
#include "Content/Preload.hpp"
#include "Content/ContentManager.hpp"
#include "Input/InputBinding.hpp"
#include "Input/FrameInput.hpp"
#include "Settings/Profile.hpp"
#include "Animation/SkeletonInstance.hpp"
#include "Audio/SoundManager.hpp"
#include "SoundSet.hpp"

#include "HUD/ButtonPanel.hpp"

#include <stdint.h>

namespace Dwarf
{
    namespace Level
    {
        class CampaignLevelCheckpoint;
    }

    namespace HUD
    {
        class MenuDisplay final
        {
        public:
            MenuDisplay(Content::ContentManager* contentManager, const Localization::StringTable* strings, const Settings::Profile* profile,
                        Audio::SoundManager* soundManager, Input::InputBindCode clickBind);
            ~MenuDisplay();

            void SetScale(float scale);

            void ShowVictoryMenu();
            void ShowDefeatMenu(const std::vector<std::shared_ptr<Level::CampaignLevelCheckpoint>>& checkpoints);
            void ShowPauseMenu(const std::vector<std::shared_ptr<Level::CampaignLevelCheckpoint>>& checkpoints);
            void Hide();

            bool IsMouseOver() const;
            bool IsBackButtonJustPressed() const;
            bool IsForwardButtonJustPressed() const;
            bool IsRestartAtLoadoutButtonPressed() const;
            bool IsCheckpointButtonPressed(uint32_t& outCheckpointIdx) const;

            const Panel* GetTooltip() const;

            void Update(double totalTime, float dt, const Input::FrameInput& input, const Camera& camera);
            void Draw(Graphics::SpriteRenderer* spriteRenderer) const;

        private:
            enum MenuType
            {
                MenuType_None,
                MenuType_Victory,
                MenuType_Defeat,
                MenuType_Paused,
            };

            void setState(MenuType state);
            void populateCheckpoints(const std::vector<std::shared_ptr<Level::CampaignLevelCheckpoint>>& checkpoints);
            void clearCheckpoints();

            const Panel* getForwardTooltip(MenuType state) const;
            const Panel* getBackTooltip(MenuType state) const;

            ResourcePointer<Content::ContentManager> _contentManager;
            ResourcePointer<const Localization::StringTable> _strings;
            Audio::SoundManager* _soundManager;

            Input::InputBindCode _clickBind;

            ResourcePointer<Animation::SkeletonInstance> _menuSkeleton;
            ResourcePointer<const Graphics::PolygonMaterialSet> _menuBaseMaterialSet;
            std::map<MenuType, ResourcePointer<const Graphics::PolygonMaterialSet>> _menuMaterials;

            Audio::SoundSet _chainSounds;
            std::vector<Audio::SoundSet> _thudSounds;

            ResourcePointer<const Graphics::PolygonMaterialSet> _forwardHightlight;
            ResourcePointer<const Graphics::PolygonMaterialSet> _backHightlight;

            ResourcePointer<const HUDMaterialPanelDrawable> _restartAtLoadoutIcon;
            ResourcePointer<const HUDMaterialPanelDrawable> _restartAtCheckpointIcon;
            ResourcePointer<const HUDMaterialPanelDrawable> _restartAtCheckpointClosedIcon;
            ResourcePointer<const HUDMaterialPanelDrawable> _disabledCheckpointIcon;

            ResourcePointer<const Panel> _restartToLoadoutTooltip;
            std::vector<ResourcePointer<const Panel>> _restartAtCheckpointTooltips;
            std::vector<ResourcePointer<const Panel>> _restartAtCheckpointClosedTooltips;
            ResourcePointer<const Panel> _disabledCheckpointTooltip;

            ResourcePointer<HUD::ButtonPanel> _buttons;

            struct checkpointInfo
            {
                bool isRestart = false;
                uint32_t checkpointIdx = 0;
                ResourcePointer<const Panel> tooltip = nullptr;
                ResourcePointer< const PanelDrawable> icon = nullptr;
                bool valid = false;
            };
            std::vector<checkpointInfo> _checkpointInfo;

            std::map<MenuType, ResourcePointer<const Panel>> _forwardButtonTooltips;
            std::map<MenuType, ResourcePointer<const Panel>> _backButtonTooltips;

            MenuType _currentState;
            bool _showButtons;
            ResourcePointer<const Panel> _selectedTooltip;

            bool _mouseOver = false;
            bool _forwardJustPressed;
            bool _backJustPressed;
            bool _restartJustPressed;
            bool _checkpointJustPressed;
            uint32_t _pressedCheckpoint;

            float _scale;
        };
    }

    template <>
    void EnumeratePreloads<HUD::MenuDisplay>(PreloadSet& preloads);
}
