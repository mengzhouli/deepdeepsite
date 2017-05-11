#pragma once

#include "Character/Buff.hpp"
#include "Content/Preload.hpp"

namespace Dwarf
{
    namespace HUD
    {
        class Icon;
    }

    namespace Character
    {
        struct BuffComponentParameters
        {
            Character* owner;
        };

        class BuffComponent
        {
        public:
            BuffComponent(const BuffComponentParameters& parameters);

            virtual void OnLoadContent(Content::ContentManager* contentManager);
            virtual void OnUnloadContent();

            virtual void OnApply();
            virtual void OnFinish();
            virtual bool CanTerminate() const;
            virtual void OnUpdate(double totalTime, float dt);
            virtual void OnDraw(Graphics::LevelRenderer* levelRenderer) const;

        protected:
            Character* GetOwner() const;

        private:
            Character* _owner;
        };

        class BasicBuff : public Buff
        {
        public:
            BasicBuff(const BuffParameters& parameters, const std::string& nameCode);

            float GetTotalDuration() const;
            float GetTimeRemaining() const;

            bool IsFinished() const override;

            // HUD
            const HUD::PanelDrawable* GetIcon() const override;
            const HUD::Panel* GetTooltip() const override;

        protected:
            void SetTotalDuration(float duration);

            void SetIcon(const std::string& iconMatsetPath, const std::string& iconMaterialName);
            void SetTooltipDescription(const std::string codeName, ...);
            void SetTooltipFlavor(const std::string codeName);

            template <typename T, typename... argsT>
            T* AddComponent(argsT... args);

            void OnApply() override;
            void OnFinish() override;

            void OnLoadContent(Content::ContentManager* contentManager) override;
            void OnUnloadContent() override;

            void OnUpdate(double totalTime, float dt) override;

            void OnDraw(Graphics::LevelRenderer* levelRenderer) const override;

        private:
            float _totalTime = 0.0f;
            float _timer = 0.0f;
            bool _finished = false;

            std::string _iconMatsetPath = "";
            std::string _iconMaterialName = "";

            HUD::Icon* _icon = nullptr;

            std::string _description = "";
            std::string _flavor = "";
            HUD::PanelDrawable* _iconDrawable = nullptr;
            HUD::Panel* _tooltip = nullptr;

            std::vector<BuffComponent*> _components;
        };

    }

    template <>
    void EnumeratePreloads<Character::BasicBuff>(PreloadSet& preloads);
}

#include "BasicBuff.inl"
