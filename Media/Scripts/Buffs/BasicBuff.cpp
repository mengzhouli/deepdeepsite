#include "Buffs/BasicBuff.hpp"
#include "HUD/Tooltip.hpp"

namespace Dwarf
{
    namespace Character
    {
        BuffComponent::BuffComponent(const BuffComponentParameters& parameters)
            : _owner(parameters.owner)
        {
        }

        void BuffComponent::OnLoadContent(Content::ContentManager* contentManager)
        {
        }

        void BuffComponent::OnUnloadContent()
        {
        }

        void BuffComponent::OnApply()
        {
        }

        void BuffComponent::OnFinish()
        {
        }

        bool BuffComponent::CanTerminate() const
        {
            return true;
        }

        void BuffComponent::OnUpdate(double totalTime, float dt)
        {
        }

        void BuffComponent::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
        }

        Character* BuffComponent::GetOwner() const
        {
            return _owner;
        }

        static std::string GetBuffName(Character* owner, const std::string& nameCode)
        {
            assert(owner);
            return owner->GetLevel()->GetStringTable()->GetString(nameCode);
        }

        BasicBuff::BasicBuff(const BuffParameters& parameters, const std::string& nameCode)
            : Buff(parameters, GetBuffName(parameters.owner, nameCode))
        {
        }

        float BasicBuff::GetTotalDuration() const
        {
            return _totalTime;
        }

        float BasicBuff::GetTimeRemaining() const
        {
            return _timer;
        }

        bool BasicBuff::IsFinished() const
        {
            return _finished && _components.empty();
        }

        const HUD::PanelDrawable* BasicBuff::GetIcon() const
        {
            return _iconDrawable;
        }

        const HUD::Panel* BasicBuff::GetTooltip() const
        {
            return _tooltip;
        }

        void BasicBuff::SetTotalDuration(float duration)
        {
            _totalTime = duration;
            _timer = duration;
        }

        void BasicBuff::SetIcon(const std::string& iconMatsetPath, const std::string& iconMaterialName)
        {
            _iconMatsetPath = iconMatsetPath;
            _iconMaterialName = iconMaterialName;
        }

        void BasicBuff::SetTooltipDescription(const std::string codeName, ...)
        {
            va_list args;
            va_start(args, codeName);
            _description = Format(GetOwner()->GetLevel()->GetStringTable()->GetString(codeName).c_str(), args);
            va_end(args);
        }

        void BasicBuff::SetTooltipFlavor(const std::string codeName)
        {
            _flavor = GetOwner()->GetLevel()->GetStringTable()->GetString(codeName);
        }

        void BasicBuff::OnApply()
        {
            for (auto& component : _components)
            {
                component->OnApply();
            }
        }

        void BasicBuff::OnFinish()
        {
            _finished = true;
            for (auto& component : _components)
            {
                component->OnFinish();
            }
        }

        void BasicBuff::OnLoadContent(Content::ContentManager* contentManager)
        {
            const Localization::StringTable* strings = GetOwner()->GetLevel()->GetStringTable();
            
            _iconDrawable = HUD::CreateBuffIcon(contentManager, _iconMatsetPath, _iconMaterialName, _icon);
            _tooltip = HUD::CreateBuffTooltip(contentManager, strings, GetName(), _iconDrawable, _description, _flavor);

            for (auto& component : _components)
            {
                component->OnLoadContent(contentManager);
            }
        }

        void BasicBuff::OnUnloadContent()
        {
            SafeRelease(_icon);
            SafeRelease(_iconDrawable);
            SafeRelease(_tooltip);

            for (auto& component : _components)
            {
                component->OnUnloadContent();
            }
        }

        void BasicBuff::OnUpdate(double totalTime, float dt)
        {
            for (auto& component : _components)
            {
                component->OnUpdate(totalTime, dt);
            }

            if (_totalTime > 0.0f && !_finished)
            {
                _timer -= dt;
                _icon->SetCooldown(Max(_timer, 0.0f), _totalTime);

                if (_timer <= 0.0f)
                {
                    Finish();
                }
            }

            if (_finished)
            {
                auto componentIter = _components.begin();
                while (componentIter != _components.end())
                {
                    if ((*componentIter)->CanTerminate())
                    {
                        componentIter = _components.erase(componentIter);
                    }
                    else
                    {
                        componentIter++;
                    }
                }
            }
        }

        void BasicBuff::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            for (const auto& component : _components)
            {
                component->OnDraw(levelRenderer);
            }
        }
    }

    template <>
    void EnumeratePreloads<Character::BasicBuff>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Buff>(preloads);
    }
}
