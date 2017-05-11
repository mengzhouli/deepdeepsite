#include "Abilities/ChannelingFoodHeal.hpp"

#include "Characters/CharacterTraits.hpp"
#include "Characters/BasicCharacter.hpp"
#include "Item/Item.hpp"
#include "Items/Weapons/WeaponTraits.hpp"
#include "AbilityUtility.hpp"
#include "NavigationUtility.hpp"
#include "ContentUtility.hpp"
#include "SkeletonUtility.hpp"
#include "Graphics/GraphicsUtility.hpp"
#include "Random.hpp"
#include "DamageTypes.hpp"

#include "Buffs/HealOverTime.hpp"
#include "Buffs/DamageOverTime.hpp"

namespace Dwarf
{
    static const std::string FoodSkeletonPath = "Skeletons/Items/Food/food.skel";
    static const std::vector<std::string> FoodMatsetPaths =
    {
        "Skeletons/Items/Food/beef.polymatset",
        "Skeletons/Items/Food/chicken.polymatset",
        "Skeletons/Items/Food/ham.polymatset",
    };

    static const std::string FoodIconMatsetPath = "HUD/abilities.hudmatset";

    namespace Character
    {
        struct ThrownFoodParams
        {
            CharacterID owner = 0;
            CharacterID target = 0;

            float healAmount = 0.0f;

            Damage damageAmount;

            Damage dotAmmount;
            float dotDuration = 0.0f;

            float hotAmmount = 0.0f;
            float hotDuration = 0.0f;

            float aoeRange = 0.0f;

            uint32_t matsetIdx = 0;
        };

        class ThrownFoodBuff : public BasicBuff
        {
        public:
            ThrownFoodBuff(const BuffParameters& parameters, const ThrownFoodParams& foodParams)
                : BasicBuff(parameters, "buff_thrown_food_name")
            {
                if (foodParams.dotAmmount.Amount > 0.0f)
                {
                    AddComponent<DamageOverTime>(foodParams.owner, foodParams.dotAmmount, 0.5f);
                    SetTotalDuration(foodParams.dotDuration);
                }
                else if (foodParams.hotAmmount > 0.0f)
                {
                    AddComponent<HealOverTime>(foodParams.owner, foodParams.hotAmmount / foodParams.hotDuration, 0.5f);
                    SetTotalDuration(foodParams.hotDuration);
                }

                SetIcon(FoodIconMatsetPath, "icon_buff_thrown_food");
                SetTooltipDescription("buff_thrown_description");
            }
        };

        class ThrownFood : public SkeletonCharacter
        {
        public:
            ThrownFood(const CharacterParameters& parameters, const ThrownFoodParams& foodParams)
                : SkeletonCharacter(parameters, FoodSkeletonPath, FoodMatsetPaths[foodParams.matsetIdx])
                , _params(foodParams)
                , _hitSomething(false)
                , _totalFadeTime(0.2f)
                , _totalTime(5.0f)
                , _timer(_totalTime)
            {
                SetEntityMask(CharacterMask_None);
                SetMoveType(MoveType_None);

                EnablePhysics(true);
                SetInvulnerable(true);
            }

        protected:
            virtual void OnUpdate(double totalTime, float dt) override
            {
                SkeletonCharacter::OnUpdate(totalTime, dt);

                if (!_hitSomething)
                {
                    const auto& layer = GetLevelLayer();
                    Character* target = layer->GetCharacter<>(_params.target);

                    Vector2f hitPoint;
                    if (target != nullptr && target->Intersects(GetCollision(), hitPoint))
                    {
                        Character* owner = layer->GetCharacter<>(_params.owner);

                        if (_params.damageAmount.Amount > 0.0f || _params.dotAmmount.Amount > 0.0f)
                        {
                            std::vector<Character*> targets;
                            if (_params.aoeRange > 0.0f)
                            {
                                targets = layer->GetCharacters<Character>([owner, target, this](const Character* character)
                                        {
                                            return character != target &&
                                                   Vector2f::DistanceSquared(GetPosition(), character->GetPosition()) < _params.aoeRange * _params.aoeRange &&
                                                   owner->IsCharacterAttackable(character, false);
                                        });
                            } 
                            targets.push_back(target);

                            for (auto& hitTarget : targets)
                            {
                                if (_params.damageAmount.Amount > 0.0f)
                                {
                                    hitTarget->ApplyDamage(owner, hitPoint, _params.damageAmount);
                                }
                                if (_params.dotAmmount.Amount > 0.0f)
                                {
                                    hitTarget->ApplyBuff(BindBuffConstructor<ThrownFoodBuff>(_params));
                                }
                            }
                        }

                        if (_params.healAmount > 0.0f || _params.hotAmmount > 0.0f)
                        {
                            std::vector<Character*> targets;
                            if (_params.aoeRange > 0.0f)
                            {
                                targets = layer->GetCharacters<Character>([owner, target, this](const Character* character)
                                        {
                                            return character != target &&
                                                   owner->GetController() == character->GetController() &&
                                                   character->IsAlive() &&
                                                   Vector2f::DistanceSquared(owner->GetPosition(), character->GetPosition()) <= _params.aoeRange * _params.aoeRange;
                                        });
                            } 
                            targets.push_back(target);

                            for (auto& hitTarget : targets)
                            {
                                if (_params.healAmount > 0.0f)
                                {
                                    hitTarget->Heal(owner, _params.healAmount);
                                }
                                if (_params.hotAmmount > 0.0f)
                                {
                                    hitTarget->ApplyBuff(BindBuffConstructor<ThrownFoodBuff>(_params));
                                }
                            }
                        }

                        _hitSomething = true;

                        // Start fading right away
                        _timer = _totalFadeTime;
                    }
                }

                _timer -= dt;
                if (_timer < _totalFadeTime)
                {
                    Color color = GetColor();
                    color.A = Lerp(0, 255, Clamp(_timer / _totalFadeTime, 0.0f, 1.0f));
                    SetColor(color);
                }

                if (_timer < 0.0f)
                {
                    Terminate();
                }
            }

        private:
            ThrownFoodParams _params;

            bool _hitSomething;

            const float _totalFadeTime;
            const float _totalTime;
            float _timer;
        };
    }

    template <>
    void EnumeratePreloads<Character::ThrownFoodBuff>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::BasicBuff>(preloads);

        EnumeratePreloads<Character::DamageOverTime>(preloads);
        EnumeratePreloads<Character::HealOverTime>(preloads);

        preloads.insert(FoodIconMatsetPath);
    }

    template <>
    void EnumeratePreloads<Character::ThrownFood>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::SkeletonCharacter>(preloads);
        preloads.insert(FoodSkeletonPath);
        for (const auto& foodMatset : FoodMatsetPaths)
        {
            preloads.insert(foodMatset);
        }
        EnumeratePreloads<Character::ThrownFoodBuff>(preloads);
    }

    namespace Ability
    {
        static const std::string FoodCursorPath = "HUD/abilities.cursorset";
        static const std::string FoodIconMatsetPath = "HUD/abilities.hudmatset";

        static const std::string CookPotSkeletonPath = "Skeletons/Items/Tools/Pot/Pot.skel";
        static const std::string CookPotMatsetPath = "Skeletons/Items/Tools/Pot/Pot_0.polymatset";

        static const Audio::SoundPathVector CookPotSoundPaths =
        {
            { "Audio/Abilities/Cook/Pot_Boiling.ogg", 1.0f },
        };
        static const float CookSoundVolume = 0.2f;

        static const float CookTime = 2.0f;

        static const float FoodRange = 1000.0f;
        static const float FoodHealing = 8.0f;
        static const float FoodHoTAmount = 4.0f;
        static const float FoodHoTDuration = 2.0f;

        static const Character::Damage FoodDamage(Character::DamageType_Type_Projectile | Character::DamageType_Element_Poison, 10.0f);

        ChannelingFoodHeal::ChannelingFoodHeal(const AbilityParameters& parameters)
            : BasicAbility(parameters, "ability_channeling_food_heal_name", AbilityType_OneTarget)
        {
            SetIcon(FoodIconMatsetPath, "icon_channeling_food_heal");
            SetCursor(FoodCursorPath, "cursor_cook_food");
            SetCooldown(5.0f);
            SetTooltipDescription("ability_channeling_food_heal_tooltip", CookTime, FoodHealing);

            _cookingSounds.AddSounds(CookPotSoundPaths);
        }

        void ChannelingFoodHeal::ClearState()
        {
            setDrawPot(false);
            setDrawFood(false);
            setDrawPlacementPot(false, false);

            _executing = false;
            
            _cooking = false;
            _throwing = false;

            _validPosition = false;
        }

        void ChannelingFoodHeal::SetPrimaryTarget(const Vector2f& pos)
        {
            _validPosition = VerifyPotPosition(pos, _placementPotPosition);
            setDrawPlacementPot(true, _validPosition);
        }

        bool ChannelingFoodHeal::IsInValidState() const
        {
            return _validPosition;
        }

        Character::Action ChannelingFoodHeal::GenerateAction() const
        {
            if (_validPosition)
            {
                return Character::CreateUseAbilityAction(GetID(), _placementPotPosition);
            }
            else
            {
                return Character::CreateEmptyAction();
            }
        }

        void ChannelingFoodHeal::ExecuteAction(const Character::Action& action)
        {
            Character::Character* owner = GetOwner();
            _potPosition = Vector2f(action.AbilityPosition.StartX, action.AbilityPosition.StartY);
            _placementPotPosition = _potPosition;
            _executing = owner->MoveTo(_potPosition);
            if (_executing)
            {
                setDrawPlacementPot(true, false);
                _validPosition = true;
            }
            _cooking = false;
            _throwing = false;
        }

        void ChannelingFoodHeal::CancelAction()
        {
            ClearState();
            StartCooldown();
        }

        bool ChannelingFoodHeal::IsExecutingAction() const
        {
            return _executing;
        }

        static float GetFoodRange(Character::Character* owner)
        {
            float modifier = 1.0f;
            for (auto modifierItem : owner->GetItems<Item::ChannelingFoodModifier>())
            {
                modifier *= modifierItem->GetCookRangeModifier();
            }

            return FoodRange * modifier;
        }

        static bool CookedFoodDoesAOE(Character::Character* owner)
        {
            for (auto modifierItem : owner->GetItems<Item::ChannelingFoodModifier>())
            {
                if (modifierItem->CookedFoodDoesAOE())
                {
                    return true;
                }
            }

            return false;
        }

        static bool CookedFoodTargetsEnemies(Character::Character* owner)
        {
            for (auto modifierItem : owner->GetItems<Item::ChannelingFoodModifier>())
            {
                if (modifierItem->CookedFoodTargetsEnemies())
                {
                    return true;
                }
            }

            return false;
        }

        static bool CookedFoodHealsOverTime(Character::Character* owner)
        {
            for (auto modifierItem : owner->GetItems<Item::ChannelingFoodModifier>())
            {
                if (modifierItem->CookedFoodHealsOverTime())
                {
                    return true;
                }
            }

            return false;
        }

        static const std::vector<Character::CharacterID> GetFoodTargets(Character::Character* owner)
        {
            uint32_t numTargets = 1;
            for (auto modifierItem : owner->GetItems<Item::ChannelingFoodModifier>())
            {
                numTargets += modifierItem->GetCookFoodAdditionalTargetCount();
            }

            float range = GetFoodRange(owner);
            std::vector<Character::Character*> characters;

            if (CookedFoodTargetsEnemies(owner))
            {
                characters = owner->GetLevelLayer()->GetCharacters<Character::Character>(
                    [owner, range](const Character::Character* target)
                    {
                        return target != owner &&
                            target->IsAlive() &&
                            Vector2f::DistanceSquared(owner->GetPosition(), target->GetPosition()) <= range * range &&
                            owner->IsCharacterAttackable(target, false);
                    });
            }
            else
            {
                characters = owner->GetLevelLayer()->GetCharacters<Character::Character>(
                    [owner, range](const Character::Character* target)
                    {
                        return target != owner &&
                            owner->GetController() == target->GetController() &&
                            ((target->GetEntityMask() & Character::CharacterMask_Constructed) == 0) &&
                            target->IsAlive() &&
                            Vector2f::DistanceSquared(owner->GetPosition(), target->GetPosition()) <= range * range &&
                            target->GetHealth().GetPercent() < 1.0f;
                    });
            }

            std::sort(characters.begin(), characters.end(),
                [owner](const Character::Character* a, const Character::Character* b)
            {
                //return Vector2f::DistanceSquared(owner->GetPosition(), a->GetPosition()) < 
                //       Vector2f::DistanceSquared(owner->GetPosition(), b->GetPosition());
                return std::make_tuple((a->GetEntityMask() & Character::CharacterMask_Constructed) != 0, a->GetHealth().GetCurrent()) <
                       std::make_tuple((b->GetEntityMask() & Character::CharacterMask_Constructed) != 0, b->GetHealth().GetCurrent());
            });

            std::vector<Character::CharacterID> result;
            for (uint32_t i = 0; i < Min(characters.size(), numTargets); i++)
            {
                result.push_back(characters[i]->GetID());
            }

            return result;
        }

        static Character::ThrownFoodParams GenerateThrownFoodParams(Character::Character* owner, Character::CharacterID target, uint32_t matsetIdx)
        {
            Character::ThrownFoodParams foodParams;

            foodParams.owner = owner->GetID();
            foodParams.target = target;
            foodParams.matsetIdx = matsetIdx;

            if (CookedFoodTargetsEnemies(owner))
            {
                foodParams.damageAmount = FoodDamage;
            }
            else
            {
                foodParams.healAmount = FoodHealing;
                if (CookedFoodHealsOverTime(owner))
                {
                    foodParams.hotAmmount = FoodHoTAmount;
                    foodParams.hotDuration = FoodHoTDuration;
                }
            }

            return foodParams;
        }

        void ChannelingFoodHeal::OnUpdate(double totalTime, float dt)
        {
            BasicAbility::OnUpdate(totalTime, dt);

            if (_executing)
            {
                Character::Character* owner = GetOwner();
                Character::Cooker* cooker = GetOwner<Character::Cooker>();
                assert(cooker != nullptr);

                if (_cooking)
                {
                    _targets = GetFoodTargets(owner);

                    _cookingTimer -= dt;
                    if (_targets.size() > 0 && _cookingTimer <= 0.0f)
                    {
                        _cooking = false;
                        _throwing = true;
                        setDrawFood(true);
                        cooker->PlayFoodThrowAnimation();
                    }
                }
                else if (_throwing)
                {
                    bool passedThrowPoint = cooker->HasFoodThrowReleasePassed();
                    bool animationFinished = cooker->HasFoodThrowAnimationFinished();

                    if (_drawFood && (passedThrowPoint || animationFinished))
                    {
                        setDrawFood(false);

                        // Throw
                        for (auto targetID : _targets)
                        {
                            auto target = owner->GetLevelLayer()->GetCharacter(targetID);
                            if (target == nullptr)
                            {
                                continue;
                            }

                            Character::ThrownFoodParams foodParams = GenerateThrownFoodParams(owner, targetID, _curFoodIdx);

                            Character::CharacterConstructor<Character::ThrownFood> constructor =
                                Character::BindCharacterConstructor<Character::ThrownFood>(foodParams);

                            const auto& food = _foods[_curFoodIdx];
                            Character::ThrownFood* projectile = owner->GetLevelLayer()->SpawnCharacter(food->GetPosition(), "thrown_food",
                                nullptr, constructor);

                            projectile->SetRotation(food->GetRotation());
                            projectile->SetScale(food->GetScale());
                            projectile->GetSkeleton()->SetInvertedX(food->IsInvertedX());
                            projectile->GetSkeleton()->SetInvertedY(food->IsInvertedY());

                            Physics::Collision *projectileCollision = projectile->GetCollision();

                            float m = projectileCollision->GetMass();
                            float g = projectileCollision->GetPhysicsWorld()->GetGravity().Y;

                            Vector2f dir = target->GetBounds().Middle() - projectile->GetPosition();

                            float rangePerc = Clamp(dir.Length() / GetFoodRange(owner), 0.0f, 1.0f);

                            // Adjust flight time depending on range perc
                            const float t = Lerp(0.05f, 0.6f, rangePerc);

                            //const Vector2f vi(dir.X / t, -0.5f * g * t * t);
                            const Vector2f vi(dir.X / t, ((2.0f * dir.Y) - (g * t * t)) / (2.0f * t));

                            const Vector2f impulse(vi * m);

                            projectile->ApplyLinearImpulse(impulse);

                            static const float minSpin = TwoPi * 50.0f;
                            static const float maxSpin = (TwoPi + Pi) * 50.0f;
                            float angularImpulse = m * Random::RandomBetween(minSpin, maxSpin);
                            if (dir.X > 0.0f)
                            {
                                angularImpulse = -angularImpulse;
                            }

                            projectile->ApplyAngularImpulse(angularImpulse);
                        }
                    }

                    if (animationFinished)
                    {
                        _cookingTimer = CookTime;
                        _cooking = true;
                        _throwing = false;
                        cooker->PlayCookAnimation();
                    }
                }
                else
                {
                    const float minDropDist = owner->GetBounds().W * 0.5f;
                    bool closeEnoughToDropPos = Vector2f::Distance(owner->GetPosition(), _potPosition) < minDropDist;

                    if (!owner->IsMoving() || closeEnoughToDropPos)
                    {
                        owner->StopMoving();
                        _cooking = true;
                        _throwing = false;
                        _cookingTimer = CookTime;
                        cooker->PlayCookAnimation();
                        setDrawPlacementPot(false, false);
                        setDrawPot(true);
                    }
                }

                if (_cooking || _throwing)
                {
                    if (_targets.size() > 0)
                    {
                        auto target = owner->GetLevelLayer()->GetCharacter(_targets[0]);
                        if (target != nullptr)
                        {
                            owner->SetRotation(Rotatorf(target->GetPosition() - owner->GetPosition()));

                            Character::BasicCharacter* basicOwner = GetOwner<Character::BasicCharacter>();
                            if (basicOwner != nullptr)
                            {
                                basicOwner->LookAt(target->GetPosition());
                            }
                        }
                    }
                }

                if (_drawPot)
                {
                    Rayf ownerAttachRay = owner->GetAttachPoint(cooker->GetCookPotAttachPointName());
                    Animation::AttachmentInfo attachInfo = Animation::GetSkeletonAttachmentInfo(
                        Animation::IsCharacterInvertedX(owner), Animation::IsCharacterInvertedY(owner), true, ownerAttachRay,
                        _cookPot, "attach_bottom", "attach_top");

                    _cookPot->SetInvertedX(attachInfo.InvertX);
                    _cookPot->SetInvertedY(attachInfo.InvertY);
                    _cookPot->SetPosition(attachInfo.Position);
                    _cookPot->SetScale(attachInfo.Scale);
                    _cookPot->SetRotation(attachInfo.Rotation);
                }

                if (_drawFood)
                {
                    auto& food = _foods[_curFoodIdx];

                    Rayf ownerAttachRay = owner->GetAttachPoint(cooker->GetFoodThrownItemAttachPointName());
                    Animation::AttachmentInfo attachInfo =
                        Animation::GetSkeletonAttachmentInfo(Animation::IsCharacterInvertedX(owner), Animation::IsCharacterInvertedY(owner), true, ownerAttachRay,
                            food, "origin", "b");

                    food->SetInvertedX(attachInfo.InvertX);
                    food->SetInvertedY(attachInfo.InvertY);
                    food->SetPosition(attachInfo.Position);
                    food->SetRotation(attachInfo.Rotation);
                    food->SetScale(attachInfo.Scale);
                }
            }

            _cookPot->Update(totalTime, dt);

            _placementPot->SetColor(GetAbilityUIColor(_validPosition));
            _placementPot->SetPosition(_placementPotPosition);
            _placementPot->Update(totalTime, dt);

            for (auto& food : _foods)
            {
                food->Update(totalTime, dt);
            }

            if (_curCookingSound)
            {
                const Vector2f& cookpotAudioPos = _cookPot->GetJointPosition("audio_source");
                float cookpotAudioSize = Vector2f::Distance(cookpotAudioPos, _cookPot->GetJointPosition("audio_size"));
                _curCookingSound->SetPosition(cookpotAudioPos);
                _curCookingSound->SetMinMaxDistance(cookpotAudioSize * 1.0f, cookpotAudioSize * 2.0f);
            }
        }

        void ChannelingFoodHeal::OnLoadContent(Content::ContentManager* contentManager)
        {
            BasicAbility::OnLoadContent(contentManager);

            _cookPot = Content::CreateSkeletonInstance(contentManager, CookPotSkeletonPath, CookPotMatsetPath);
            _cookPot->PlayAnimation("idle", true);

            _placementPot = Content::CreateSkeletonInstance(contentManager, CookPotSkeletonPath, CookPotMatsetPath);
            _placementPot->PlayAnimation("idle", true);

            for (const auto& foodMatset : FoodMatsetPaths)
            {
                auto food = Content::CreateSkeletonInstance(contentManager, FoodSkeletonPath, foodMatset);
                _foods.push_back(food);
            }

            _cookingSounds.LoadContent(contentManager);
        }

        void ChannelingFoodHeal::OnUnloadContent()
        {
            BasicAbility::OnUnloadContent();

            SafeRelease(_cookPot);
            SafeRelease(_placementPot);

            SafeReleaseAndClearContainer(_foods);

            _cookingSounds.UnloadContent();
            _curCookingSound = nullptr;
        }

        void ChannelingFoodHeal::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            BasicAbility::OnDraw(levelRenderer);
            
            if (_drawPlacementPot)
            {
                levelRenderer->AddDrawable(_placementPot, false, false);
            }
            // cookpot drawn as sub-drawable of owner
        }

        void ChannelingFoodHeal::OnDrawHUD(Graphics::SpriteRenderer* spriteRenderer) const
        {
            BasicAbility::OnDrawHUD(spriteRenderer);

            if (_drawPlacementPotRange)
            {
                Character::Character *owner = GetOwner();
                assert(owner);

                const Camera& worldCam = owner->GetLevel()->GetCameraController().GetCamera();
                const Camera& hudCam = owner->GetLevel()->GetHUDCamera();

                float range = GetFoodRange(owner);

                Vector2f hudCenter = Camera::Transpose(worldCam, _placementPotPosition, hudCam);
                float hudRadius = Vector2f::Distance(hudCenter, Camera::Transpose(worldCam, _placementPotPosition + Vector2f(range, 0.0f), hudCam));

                const uint32_t segmentCount = 128;

                Color innerColor = GetAbilityUIColor(_validPosition);
                innerColor.A = 63;
                Graphics::DrawFilledCircle(spriteRenderer, hudCenter, 0.0f, innerColor, hudRadius, innerColor, segmentCount);

                Color outterColor(innerColor.R, innerColor.G, innerColor.B, 127);
                Graphics::DrawCircle(spriteRenderer, hudCenter, hudRadius, outterColor, 2.0f, segmentCount);
            }
        }

        bool ChannelingFoodHeal::VerifyPotPosition(const Vector2f& pos, Vector2f& placePos)
        {
            Character::Character* owner = GetOwner();
            Level::LevelLayerInstance* layer = owner->GetLevelLayer();
            if (Level::GetPlaceTarget(layer, Pathfinding::EdgeType_Walk, pos, 400.0f, placePos))
            {
                return true;
            }
            else
            {
                placePos = pos + (_placementPot->GetPosition() - _placementPot->GetDrawBounds().Middle());
                return false;
            }
        }

        void ChannelingFoodHeal::setDrawPot(bool drawPot)
        {
            Character::Cooker* cookOwner = GetOwner<Character::Cooker>();
            if (cookOwner != nullptr)
            {
                if (_drawPot != drawPot)
                {
                    Character::Character* owner = GetOwner<Character::Character>();
                    std::string attachName = cookOwner->GetCookPotAttachPointName();
                    if (drawPot)
                    {
                        owner->AttachDrawable(attachName, _cookPot);
                    }
                    else
                    {
                        owner->DetachDrawable(attachName, _cookPot);
                    }

                    _drawPot = drawPot;
                }
            }
            else
            {
                _drawPot = false;
            }

            if (_drawPot)
            {
                auto soundManager = GetOwner()->GetLevel()->GetSoundManager();
                _curCookingSound = soundManager->PlayLoopingPositionalSound(_cookingSounds.GetNextSound(), Audio::SoundPriority::Medium, GetOwner()->GetPosition(), 0.0f, 1.0f, CookSoundVolume);
            }
            else
            {
                if (_curCookingSound)
                {
                    _curCookingSound->Stop(0.25f);
                    _curCookingSound = nullptr;
                }
            }
        }

        void ChannelingFoodHeal::setDrawFood(bool drawFood)
        {
            Character::Character* owner = GetOwner();
            Character::Cooker* cooker = GetOwner<Character::Cooker>();
            assert(cooker != nullptr);

            if (_drawFood != drawFood)
            {
                if (drawFood)
                {
                    // Select a new food
                    _curFoodIdx = Random::RandomBetween(0, _foods.size() - 1);
                }

                std::string attachName = cooker->GetFoodThrownItemAttachPointName();
                if (drawFood)
                {
                    owner->AttachDrawable(attachName, _foods[_curFoodIdx]);
                }
                else
                {
                    owner->DetachDrawable(attachName, _foods[_curFoodIdx]);
                }
                
                _drawFood = drawFood;
            }
        }

        void ChannelingFoodHeal::setDrawPlacementPot(bool drawPot, bool drawRange)
        {
            _drawPlacementPot = drawPot;
            _drawPlacementPotRange = drawRange;
            if (_drawPlacementPotRange)
            {
                assert(_drawPlacementPot);
            }
        }
    }

    template <>
    void EnumeratePreloads<Ability::ChannelingFoodHeal>(PreloadSet& preloads)
    {
        EnumeratePreloads<Ability::BasicAbility>(preloads);
        EnumeratePreloads<Character::ThrownFood>(preloads);
        preloads.insert(FoodSkeletonPath);
        for (const auto& foodMatset : FoodMatsetPaths)
        {
            preloads.insert(foodMatset);
        }
        preloads.insert(Ability::FoodCursorPath);
        preloads.insert(Ability::CookPotSkeletonPath);
        preloads.insert(Ability::CookPotMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Ability::CookPotSoundPaths);
    }
}
