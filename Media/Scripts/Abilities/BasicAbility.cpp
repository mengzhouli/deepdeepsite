#include "Abilities/BasicAbility.hpp"

#include "HUD/Tooltip.hpp"

namespace Dwarf
{
    namespace Ability
    {
        static std::string GetAbilityName(Character::Character* owner, const std::string& nameCode)
        {
            assert(owner);
            return owner->GetLevel()->GetStringTable()->GetString(nameCode);
        }

        BasicAbility::BasicAbility(const AbilityParameters& parameters, const std::string& nameCode, AbilityType type)
            : Ability(parameters, GetAbilityName(parameters.Owner, nameCode), type)
            , _iconMatsetPath()
            , _iconMaterialName()
            , _icon(nullptr)
            , _description()
            , _iconDrawable(nullptr)
            , _tooltip(nullptr)
            , _cursorSetPath()
            , _cursorName()
            , _cursor(nullptr)
            , _resourceCost(0)
            , _cooldown(0.0f)
            , _remainingCooldown(0.0f)
        {
        }

        BasicAbility::~BasicAbility()
        {
        }

        float BasicAbility::GetCooldown() const
        {
            return _cooldown;
        }

        float BasicAbility::GetRemainingCooldown() const
        {
            return (_remainingCooldown > 0.0f) ? _remainingCooldown : 0.0f;
        }

        Item::Resources BasicAbility::GetResourceCost() const
        {
            return _resourceCost;
        }

        const HUD::PanelDrawable* BasicAbility::GetIcon() const
        {
            return _iconDrawable;
        }

        const HUD::Panel* BasicAbility::GetTooltip() const
        {
            return _tooltip;
        }

        const App::Cursor* BasicAbility::GetCursor() const
        {
            return _cursor;
        }

        void BasicAbility::OnLoadContent(Content::ContentManager* contentManager)
        {
            const Localization::StringTable* strings = GetOwner()->GetLevel()->GetStringTable();

            _iconDrawable = HUD::CreateAbilityIcon(contentManager, _iconMatsetPath, _iconMaterialName, _icon);
            _tooltip = HUD::CreateAbilityTooltip(contentManager, strings, GetName(), _iconDrawable, GetAbilityType(), _resourceCost,
                                                 _cooldown, _description, "");
            _cursor = HUD::CreateCursor(contentManager, _cursorSetPath, _cursorName);
        }

        void BasicAbility::OnUnloadContent()
        {
            SafeRelease(_icon);
            SafeRelease(_iconDrawable);
            SafeRelease(_tooltip);
            SafeRelease(_cursor);
        }

        void BasicAbility::OnUpdate(double totalTime, float dt)
        {
            _remainingCooldown -= dt;
            if (_remainingCooldown > 0.0f && _cooldown > 0.0f)
            {
                _icon->SetCooldown(_remainingCooldown, _cooldown);
            }
            else
            {
                _icon->UnsetCooldown();
            }
        }

        void BasicAbility::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
        }

        void BasicAbility::OnDrawHUD(Graphics::SpriteRenderer* spriteRenderer) const
        {
        }

        void BasicAbility::SetIcon(const std::string& iconMatsetPath, const std::string& iconMaterialName)
        {
            _iconMatsetPath = iconMatsetPath;
            _iconMaterialName = iconMaterialName;
        }

        void BasicAbility::SetTooltipDescription(const std::string codeName, ...)
        {
            va_list args;
            va_start(args, codeName);
            _description = Format(GetOwner()->GetLevel()->GetStringTable()->GetString(codeName).c_str(), args);
            va_end(args);
        }

        void BasicAbility::SetCursor(const std::string& cursorSetPath, const std::string& cursorName)
        {
            _cursorSetPath = cursorSetPath;
            _cursorName = cursorName;
        }

        void BasicAbility::SetCooldown(float cooldown)
        {
            _cooldown = (cooldown > 0.0f) ? cooldown : 0.0f;
        }

        void BasicAbility::StartCooldown()
        {
            _remainingCooldown = _cooldown;
        }

        void BasicAbility::SetResourceCost(const Item::Resources& resourceCost)
        {
            _resourceCost = resourceCost;
        }
    }

    template <>
    void EnumeratePreloads<Ability::BasicAbility>(PreloadSet& preloads)
    {
        EnumeratePreloads<Ability::Ability>(preloads);
        EnumerateTooltipPreloads(preloads);
    }
}
