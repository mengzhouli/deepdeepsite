#include "Characters/Dwarves/BuilderDwarf.hpp"

#include "Item/Item.hpp"
#include "Items/Weapons/WeaponTraits.hpp"
#include "Characters/Bridge.hpp"

namespace Dwarf
{
    namespace Character
    {
        static const std::string BuilderMatsetPath = "Skeletons/Characters/Dwarf2/builder.polymatset";
        static const std::string BuilderIconMatsetPath = "HUD/hud.hudmatset";
        static const std::string GoldFontPath = "Fonts/numbers_gold.spritefont";

        static const std::string BuildAnimation = "BUILD";

        BuilderDwarf::BuilderDwarf(const CharacterParameters& parameters)
            : BuilderDwarf(parameters, CreateDefaultDwarfInfo(parameters))
        {
        }

        BuilderDwarf::BuilderDwarf(const CharacterParameters& parameters, const DwarfInfo& info)
            : Dwarf(parameters, info, BuilderMatsetPath)
        {
            AddTerrainAnimation(AnimationType_Move, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, BackwardRunAnimation, 1.0f);
            SetIcon(BuilderIconMatsetPath, "class_builder");
        }

        void BuilderDwarf::PlayBuildAnimation()
        {
            PlayAnimation(BuildAnimation, true, 0.1f, 0.0f);
        }

        float BuilderDwarf::OnPreInteractWithCharacter(Character* target)
        {
            if (IsA<Bridge2>(target))
            {
                PlayBuildAnimation();
                _bridgeTarget = target->GetID();

                Controller* controller = GetController();
                if (controller == nullptr || controller->GetResources().Gold > 0)
                {
                    return std::numeric_limits<float>::max();
                }
                else
                {
                    Speak(Speech_Negatory, false);
                    return -1.0f;
                }
            }
            else
            {
                return Dwarf::OnPreInteractWithCharacter(target);
            }
        }

        void BuilderDwarf::OnLoadContent(Content::ContentManager* contentManager)
        {
            Dwarf::OnLoadContent(contentManager);

            _goldFont = EmplaceResource(contentManager->Load<Graphics::Font>(GoldFontPath));
        }

        void BuilderDwarf::OnUnloadContent()
        {
            Dwarf::OnUnloadContent();

            _goldFont.Reset();
        }

        void BuilderDwarf::OnUpdate(double totalTime, float dt)
        {
            Dwarf::OnUpdate(totalTime, dt);

            if (IsBuildingBridge())
            {
                Bridge2* bridge = GetLevelLayer()->GetCharacter<Bridge2>(_bridgeTarget);
                if (!bridge || !bridge->IsAlive() || bridge->IsBuilt())
                {
                    CancelCurrentAction();
                }
                else
                {
                    if (HasAnimationTagJustPassed("hit"))
                    {
                        float multiplier = getBuildRateMultplier();
                        float goldCostPerSecond = bridge->GetGoldCostPerSecond(multiplier);
                        float animationLength = GetAnimationLength(BuildAnimation);

                        Item::Resources payment(Max(animationLength * goldCostPerSecond, 1));

                        Controller* controller = GetController();
                        if (controller)
                        {
                            payment.Gold = Min(payment.Gold, controller->GetResources().Gold);
                            controller->RemoveResources(payment);
                        }

                        if (payment.Gold > 0)
                        {
                            bridge->Build(payment, multiplier);
                            AddOverheadText(Graphics::PreparedText(Format("%i", payment.Gold), _goldFont, 40, Color::White, false, false));
                        }
                        else
                        {
                            CancelCurrentAction();
                        }
                    }
                }
            }
        }

        bool BuilderDwarf::IsBuildingBridge() const
        {
            const Action& curAction = GetCurrentAction();
            return (curAction.Type == Action::ActionType_InteractCharacter && _bridgeTarget != 0 && curAction.InteractCharacter.Target == _bridgeTarget);
        }

        float BuilderDwarf::getBuildRateMultplier() const
        {
            float modifier = 1.0f;

            for (auto modifierItem : GetItems<Item::BuildRateModifier>())
            {
                modifier *= modifierItem->GetBuildRateMultiplier();
            }

            return modifier;
        }
    }

    template <>
    void EnumeratePreloads<Character::BuilderDwarf>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Dwarf>(preloads);
        preloads.insert(Character::BuilderMatsetPath);
        preloads.insert(Character::BuilderIconMatsetPath);
        preloads.insert(Character::GoldFontPath);
    }
}
