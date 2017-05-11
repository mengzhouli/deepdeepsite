#include "Characters/Dwarves/MinerDwarf.hpp"
#include "Character/Controller.hpp"
#include "ContentUtility.hpp"

#include "Characters/ResourceNode.hpp"
#include "Item/Item.hpp"
#include "Items/Weapons/WeaponTraits.hpp"

namespace Dwarf
{
    namespace Character
    {
        static const std::string MinerMatsetPath = "Skeletons/Characters/Dwarf2/miner.polymatset";
        static const std::string MinerIconMatsetPath = "HUD/hud.hudmatset";
        static const std::string GoldFontPath = "Fonts/numbers_gold.spritefont";
        static const std::string GoldMineParticlePath = "Particles/coin.partsys";

        static const std::string MinerSackSmashAnimation = "SACK_SLAM";

        static const std::map<std::string, std::string> MinerDwarfSacktargetJoints
        {
            { "upperarmb", "sack_target_upperarmb" },
            { "forearmb", "sack_target_forearmb" },
            { "handb", "sack_target_handb" },
        };

        MinerDwarf::MinerDwarf(const CharacterParameters& parameters)
            : MinerDwarf(parameters, CreateDefaultDwarfInfo(parameters))
        {
        }

        MinerDwarf::MinerDwarf(const CharacterParameters& parameters, const DwarfInfo& info)
            : Dwarf(parameters, info, MinerMatsetPath)
            , _nextMineAmmount(0)
            , _miningTarget(0)
            , _miningAnimationHitTag("fire")
            , _miningAnimations()
            , _goldHitParticleSystem(nullptr)
            , _goldFont(nullptr)
        {
            SetItemSlotCount(Item::ItemSlot::Weapon, 2);

            AddTerrainAnimation(AnimationType_Move, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, BackwardRunAnimation, 1.0f);
            SetIcon(MinerIconMatsetPath, "class_miner");

            _miningAnimations.AddAnimation("MINE_0", 1.0f);
            //_miningAnimations.AddAnimation("MINE_1", 1.0f);

            AddWeaponAttachment("off_hand", "weapon_b_0", "weapon_b_1", { { Item::WeaponType_Special, "handb" }, });

            SetPlayFasterAnimationsForMoving(false);

        }

        float MinerDwarf::OnPreInteractWithCharacter(Character* target)
        {
            if (IsA<ResourceNode>(target))
            {
                _miningTarget = target->GetID();
                PlayAnimationSet(_miningAnimations, true, 0.2f, 0.0f);
                _nextMineAmmount = 0;

                return std::numeric_limits<float>::max();
            }
            else
            {
                return Dwarf::OnPreInteractWithCharacter(target);
            }
        }

        void MinerDwarf::OnLoadContent(Content::ContentManager* contentManager)
        {
            Dwarf::OnLoadContent(contentManager);

            _goldHitParticleSystem = Content::CreateParticleSystemInstance(contentManager, GoldMineParticlePath);
            _goldHitParticleSystem->SetScale(2.0f);

            _goldFont = contentManager->Load<Graphics::Font>(GoldFontPath);
        }

        void MinerDwarf::OnUnloadContent()
        {
            Dwarf::OnUnloadContent();

            SafeRelease(_goldHitParticleSystem);
            SafeRelease(_goldFont);
        }

        void MinerDwarf::OnUpdate(double totalTime, float dt)
        {
            Dwarf::OnUpdate(totalTime, dt);

            Animation::SkeletonInstance* skel = GetSkeleton();
            if (IsMining())
            {
                ResourceNode* miningNode = GetLevelLayer()->GetCharacter<ResourceNode>(_miningTarget);

                std::vector<Item::Miner*> miningItems = GetWeapons<Item::Miner>();
                if (!miningNode || !miningNode->IsAlive() || miningItems.empty())
                {
                    CancelCurrentAction();
                }
                else
                {
                    if (skel->HasAnimationTagPassed(_miningAnimationHitTag))
                    {
                        if (_nextMineAmmount > 0)
                        {
                            Item::Resources minedAmmount = miningNode->ExtractResources(_nextMineAmmount);

                            Controller* controller = GetController();
                            if (controller)
                            {
                                controller->AddResources(minedAmmount);
                            }

                            if (minedAmmount.Gold > 0)
                            {
                                _goldHitParticleSystem->SetPointSpawner(miningItems[0]->GetTipPosition());
                                _goldHitParticleSystem->Burst();

                                AddOverheadText(Graphics::PreparedText(Format("%i", minedAmmount.Gold), _goldFont, 40, Color::White, false, false));
                            }

                            _nextMineAmmount = 0;
                        }
                    }
                    else
                    {
                        if (_nextMineAmmount == 0)
                        {
                            float miningModifier = 1.0f;
                            std::vector<Item::MineRateModifier*> miningModifiers = GetWeapons<Item::MineRateModifier>();
                            for (auto modifier : miningModifiers)
                            {
                                miningModifier *= modifier->GetMineRateMultiplier();
                            }

                            _nextMineAmmount = Item::Resources(miningItems[0]->GetMiningRate() * miningModifier * skel->GetAnimationLength());
                        }
                    }
                }
            }

            if (GetWeapons<Item::MinerSack>().size() > 0 && !IsPlayingAnimation(MinerSackSmashAnimation))
            {
                for (const auto& sackJointPositions : MinerDwarfSacktargetJoints)
                {
                    skel->PointJointAt(sackJointPositions.first, skel->GetJointPosition(sackJointPositions.second), PiOver2);
                }
            }
            else
            {
                for (const auto& sackJointPositions : MinerDwarfSacktargetJoints)
                {
                    skel->StopPointingJoint(sackJointPositions.first);
                }
            }

            bool shouldBeUsingSprintAnims = GetMoveSpeedMultiplier() > 1.0f;
            if (shouldBeUsingSprintAnims != _usingSprintAnimations)
            {
                ClearTerrainAnimations(AnimationType_Move, Pathfinding::EdgeType_Walk);
                if (shouldBeUsingSprintAnims)
                {
                    AddTerrainAnimation(AnimationType_Move, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "SPRINT", 1.0f);
                }
                else
                {
                    AddTerrainAnimation(AnimationType_Move, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, BackwardRunAnimation, 1.0f);
                }
                _usingSprintAnimations = shouldBeUsingSprintAnims;
            }

            _goldHitParticleSystem->Update(totalTime, dt);
        }

        bool MinerDwarf::IsMining() const
        {
            const Action& curAction = GetCurrentAction();
            return (curAction.Type == Action::ActionType_InteractCharacter && _miningTarget != 0 && curAction.InteractCharacter.Target == _miningTarget);
        }

        SackSmasher::Info MinerDwarf::PlaySackSmashAnimation()
        {
            auto skeleton = GetSkeleton();

            SackSmasher::Info info;
            info.TotalDuration = skeleton->GetAnimationLength(MinerSackSmashAnimation);
            info.WooshTime = skeleton->GetAnimationTagTime(MinerSackSmashAnimation, "woosh");
            info.SackHitTime = skeleton->GetAnimationTagTime(MinerSackSmashAnimation, "hit");

            PlayAnimation(MinerSackSmashAnimation, false, 0.1f, 0.0f);

            return info;
        }

        void MinerDwarf::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            Dwarf::OnDraw(levelRenderer);

            if (_goldHitParticleSystem->GetParticleCount() > 0)
            {
                levelRenderer->AddDrawable(_goldHitParticleSystem, false);
            }
        }
    }

    template <>
    void EnumeratePreloads<Character::MinerDwarf>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Dwarf>(preloads);
        preloads.insert(Character::MinerMatsetPath);
        preloads.insert(Character::GoldMineParticlePath);
        preloads.insert(Character::GoldFontPath);
        preloads.insert(Character::MinerIconMatsetPath);
    }
}
