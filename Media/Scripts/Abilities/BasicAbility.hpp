#pragma once

#include "Ability/Ability.hpp"
#include "Graphics/HUDMaterial.hpp"
#include "Graphics/MaterialSet.hpp"
#include "Level/LevelInstance.hpp"

namespace Dwarf
{
    namespace Ability
    {
        class BasicAbility : public Ability
        {
        public:
            BasicAbility(const AbilityParameters& parameters, const std::string& nameCode, AbilityType type);

            virtual float GetCooldown() const override;
            virtual float GetRemainingCooldown() const override;

            // Cost
            virtual Item::Resources GetResourceCost() const override;

            // HUD
            virtual const HUD::PanelDrawable* GetIcon() const override;
            virtual const HUD::Panel* GetTooltip() const override;

            virtual const App::Cursor* GetCursor() const override;

            virtual void OnLoadContent(Content::ContentManager* contentManager) override;
            virtual void OnUnloadContent() override;

            virtual void OnUpdate(double totalTime, float dt) override;

            virtual void OnDraw(Graphics::LevelRenderer* levelRenderer) const override;
            virtual void OnDrawHUD(Graphics::SpriteRenderer* spriteRenderer) const override;

        protected:
            virtual ~BasicAbility();

            void SetIcon(const std::string& iconMatsetPath, const std::string& iconMaterialName);
            void SetTooltipDescription(const std::string codeName, ...);
            void SetCursor(const std::string& cursorSetPath, const std::string& cursorName);

            void SetCooldown(float cooldown);
            void StartCooldown();

            void SetResourceCost(const Item::Resources& resourceCost);

        private:
            std::string _iconMatsetPath;
            std::string _iconMaterialName;

            HUD::Icon* _icon;

            std::string _description;
            HUD::PanelDrawable* _iconDrawable;
            HUD::Panel* _tooltip;

            std::string _cursorSetPath;
            std::string _cursorName;
            const App::Cursor* _cursor;

            Item::Resources _resourceCost;

            float _cooldown;
            float _remainingCooldown;
        };
    }

    template <>
    void EnumeratePreloads<Ability::BasicAbility>(PreloadSet& preloads);
}
