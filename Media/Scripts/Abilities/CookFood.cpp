#include "Abilities/CookFood.hpp"

#include "Buffs/HealOverTime.hpp"
#include "Buffs/MaxHealthIncrease.hpp"
#include "Buffs/AttackSpeedBonus.hpp"
#include "Buffs/CritChanceBonus.hpp"

#include "Characters/SkeletonCharacter.hpp"
#include "Characters/CharacterTraits.hpp"
#include "AbilityUtility.hpp"
#include "NavigationUtility.hpp"
#include "ContentUtility.hpp"
#include "SkeletonUtility.hpp"

namespace Dwarf
{
    namespace Ability
    {
        static const std::string FoodSkeletonPath = "Skeletons/Items/Food/food.skel";
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

        CookFood::CookFood(const AbilityParameters& parameters, const std::string& nameCode, const std::string& foodMatSet)
            : BasicAbility(parameters, nameCode, AbilityType_OneTarget)
            , _executing(false)

            , _validPosition(false)
            , _drawFood(false)
            , _drawCookpot(false)
            , _foodPosition(Vector2f::Zero)

            , _cooking(false)
            , _cookingTimer(0.0f)

            , _foodMatsetPath(foodMatSet)
            , _food(nullptr)

            , _cookPot(nullptr)
        {
            SetCursor(FoodCursorPath, "cursor_cook_food");

            _cookingSounds.AddSounds(CookPotSoundPaths);
        }

        void CookFood::ClearState()
        {
            _validPosition = false;
            _drawFood = false;
        }

        void CookFood::SetPrimaryTarget(const Vector2f& pos)
        {
            _validPosition = VerifyFoodPosition(pos, _foodPosition);
            _drawFood = true;
        }

        bool CookFood::IsInValidState() const
        {
            return _validPosition;
        }

        Character::Action CookFood::GenerateAction() const
        {
            if (_validPosition)
            {
                return Character::CreateUseAbilityAction(GetID(), _foodPosition);
            }
            else
            {
                return Character::CreateEmptyAction();
            }
        }

        void CookFood::ExecuteAction(const Character::Action& action)
        {
            Character::Character* owner = GetOwner();
            _foodPosition = Vector2f(action.AbilityPosition.StartX, action.AbilityPosition.StartY);
            _executing = owner->MoveTo(_foodPosition);
        }

        void CookFood::CancelAction()
        {
            _executing = false;
            _drawFood = false;
            setDrawCookpot(false);
            _cooking = false;
        }

        bool CookFood::IsExecutingAction() const
        {
            return _executing;
        }

        void CookFood::OnUpdate(double totalTime, float dt)
        {
            BasicAbility::OnUpdate(totalTime, dt);

            if (_executing)
            {
                Character::Character* owner = GetOwner();
                if (_cooking)
                {
                    _cookingTimer -= dt;
                    if (_cookingTimer <= 0.0f)
                    {
                        Level::LevelLayerInstance* layer = owner->GetLevelLayer();
                        Character::Character* food = layer->SpawnCharacter(_foodPosition, "food", owner->GetController(), GetFoodConstructor());
                        StartCooldown();

                        _cooking = false;
                        setDrawCookpot(false);
                        _executing = false;

                        Character::Character* prevFood = layer->GetCharacter(_prevFood);
                        if (prevFood)
                        {
                            prevFood->Kill(nullptr);
                        }
                        _prevFood = food->GetID();
                    }

                    if (_drawCookpot)
                    {
                        Character::Cooker* cookOwner = GetOwner<Character::Cooker>();
                        assert(cookOwner);
                        Rayf ownerAttachRay = owner->GetAttachPoint(cookOwner->GetCookPotAttachPointName());
                        Animation::AttachmentInfo attachInfo = Animation::GetSkeletonAttachmentInfo(
                            Animation::IsCharacterInvertedX(owner), Animation::IsCharacterInvertedY(owner), true, ownerAttachRay,
                            _cookPot, "attach_bottom", "attach_top");

                        _cookPot->SetInvertedX(attachInfo.InvertX);
                        _cookPot->SetInvertedY(attachInfo.InvertY);
                        _cookPot->SetPosition(attachInfo.Position);
                        _cookPot->SetScale(attachInfo.Scale);
                        _cookPot->SetRotation(attachInfo.Rotation);

                        float alpha = 1.0f;
                        const float CookPotFadeTime = 0.1f;
                        if (_cookingTimer < CookPotFadeTime)
                        {
                            alpha = Max(_cookingTimer, 0.0f) / CookPotFadeTime;
                        }
                        else if (CookTime - _cookingTimer < CookPotFadeTime)
                        {
                            alpha = (CookTime - _cookingTimer) / CookPotFadeTime;
                        }
                        Color color = _cookPot->GetColor();
                        color.A = Clamp(alpha * 255.0f, 0, 255);
                        _cookPot->SetColor(color);
                    }
                }
                else
                {
                    const float minDropDist = owner->GetBounds().W * 0.5f;
                    bool closeEnoughToDropPos = Vector2f::Distance(owner->GetPosition(), _foodPosition) < minDropDist;

                    if (!owner->IsMoving() || closeEnoughToDropPos)
                    {
                        owner->StopMoving();
                        _cooking = true;
                        _cookingTimer = CookTime;

                        Character::Cooker* cookOwner = GetOwner<Character::Cooker>();
                        if (cookOwner != nullptr)
                        {
                            cookOwner->PlayCookAnimation();
                        }
                        _drawFood = false;
                        setDrawCookpot(true);
                    }
                }
            }

            _food->SetColor(GetAbilityUIColor(_validPosition));
            _food->SetPosition(_foodPosition);
            _food->Update(totalTime, dt);

            _cookPot->Update(totalTime, dt);

            if (_curCookingSound)
            {
                const Vector2f& cookpotAudioPos = _cookPot->GetJointPosition("audio_source");
                float cookpotAudioSize = Vector2f::Distance(cookpotAudioPos, _cookPot->GetJointPosition("audio_size"));
                _curCookingSound->SetPosition(cookpotAudioPos);
                _curCookingSound->SetMinMaxDistance(cookpotAudioSize * 1.0f, cookpotAudioSize * 2.0f);
            }
        }

        void CookFood::OnLoadContent(Content::ContentManager* contentManager)
        {
            BasicAbility::OnLoadContent(contentManager);

            _food = Content::CreateSkeletonInstance(contentManager, FoodSkeletonPath, _foodMatsetPath);

            _cookPot = Content::CreateSkeletonInstance(contentManager, CookPotSkeletonPath, CookPotMatsetPath);
            _cookPot->PlayAnimation("idle", true);

            _cookingSounds.LoadContent(contentManager);
        }

        void CookFood::OnUnloadContent()
        {
            BasicAbility::OnUnloadContent();

            SafeRelease(_food);
            SafeRelease(_cookPot);

            _cookingSounds.UnloadContent();
            _curCookingSound = nullptr;
        }

        void CookFood::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            BasicAbility::OnDraw(levelRenderer);

            if (_drawFood)
            {
                levelRenderer->AddDrawable(_food, false, false);
            }
            // cookpot drawn as sub-drawable of owner
        }

        bool CookFood::VerifyFoodPosition(const Vector2f& pos, Vector2f& placePos)
        {
            Character::Character* owner = GetOwner();
            Level::LevelLayerInstance* layer = owner->GetLevelLayer();
            if (Level::GetPlaceTarget(layer, Pathfinding::EdgeType_Walk, pos, 400.0f, placePos))
            {
                return true;
            }
            else
            {
                placePos = pos + (_food->GetPosition() - _food->GetDrawBounds().Middle());
                return false;
            }
        }

        void CookFood::setDrawCookpot(bool drawCookpot)
        {
            Character::Cooker* cookOwner = GetOwner<Character::Cooker>();
            if (cookOwner != nullptr)
            {
                if (_drawCookpot != drawCookpot)
                {
                    Character::Character* owner = GetOwner<Character::Character>();
                    std::string attachName = cookOwner->GetCookPotAttachPointName();
                    if (drawCookpot)
                    {
                        owner->AttachDrawable(attachName, _cookPot);
                    }
                    else
                    {
                        owner->DetachDrawable(attachName, _cookPot);
                    }

                    _drawCookpot = drawCookpot;
                }
            }
            else
            {
                _drawCookpot = false;
            }

            if (_drawCookpot)
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
    }

    namespace Character
    {
        static const float FoodFadeTime = 1.5f;
        static const std::string FoodSkeletonPath = "Skeletons/Items/Food/food.skel";
        static const std::string FoodParticlesPath = "Particles/food_steam.partsys";

        class Food : public SkeletonCharacter, public Edible
        {
        public:
            Food(const CharacterParameters& parameters, const std::string& matset)
                : SkeletonCharacter(parameters, FoodSkeletonPath, matset)
                , _steamName("origin")
                , _steam(nullptr)

                , _fadeDuration(0.25f)
                , _fadeTimer(0.0f)
            {
                SetEntityMask(CharacterMask_Usable | CharacterMask_Constructed);
                SetMoveType(MoveType_None);

                SetSkeletonCastsShadows(false);
                SetSkeletonColor(Color::White);
                EnablePhysics(true);
                SetMaxHealth(1.0f);
            }

        protected:
            ~Food() override
            {
            }

            void SetFadeTime(float fadeTime)
            {
                _fadeDuration = fadeTime;
            }

            void OnLoadContent(Content::ContentManager* contentManager) override
            {
                SkeletonCharacter::OnLoadContent(contentManager);

                _steam = Content::CreateParticleSystemInstance(contentManager, FoodParticlesPath);

                AttachDrawable(_steamName, _steam);
                _steam->SetScale(1.0f);
                _steam->Start();
            }

            void OnUnloadContent() override
            {
                SkeletonCharacter::OnUnloadContent();

                SafeRelease(_steam);
            }

            void OnPositionChange(const Vector2f& oldPos, const Vector2f& newPos) override
            {
                SkeletonCharacter::OnPositionChange(oldPos, newPos);
                updateParticleSpawner();
            }

            void OnScaleChange(float oldScale, float newScale) override
            {
                SkeletonCharacter::OnScaleChange(oldScale, newScale);
                updateParticleSpawner();
            }

            void OnRotationChange(const Rotatorf& oldRot, const Rotatorf& newRot) override
            {
                SkeletonCharacter::OnRotationChange(oldRot, newRot);
                updateParticleSpawner();
            }

            void OnUpdate(double totalTime, float dt) override
            {
                SkeletonCharacter::OnUpdate(totalTime, dt);

                Rayf steamAttachPt = GetAttachPoint(_steamName);
                _steam->Update(totalTime, dt);

                if (!IsAlive())
                {
                    _fadeTimer += dt;

                    Color color = GetSkeleton()->GetColor();
                    color.A = Lerp(255, 0, Saturate(_fadeTimer / _fadeDuration));
                    SetSkeletonColor(color);

                    if (_fadeTimer >= _fadeDuration)
                    {
                        Terminate();
                    }
                }
            }

            void OnDraw(Graphics::LevelRenderer* levelRenderer) const override
            {
                SkeletonCharacter::OnDraw(levelRenderer);

                levelRenderer->AddDrawable(_steam, false);
            }

            void OnPostInteractedWith(Character* interactor) override
            {
                _steam->Stop();
                OnEaten(interactor);
                if (IsA<Eater>(interactor))
                {
                    AsA<Eater>(interactor)->OnPostEat(this);
                }
            }

        private:
            void updateParticleSpawner()
            {
                _steam->SetPolygonSpawner(GetSkeleton()->GetJointBounds(_steamName));
            }

            virtual void OnEaten(Character* eater) = 0;

            std::string _steamName;
            Particles::ParticleSystemInstance* _steam;

            float _fadeDuration;
            float _fadeTimer;
        };

        static const std::string FoodBuffIconMatsetPath = "HUD/abilities.hudmatset";

        class CritFoodBuff : public BasicBuff
        {
        public:
            CritFoodBuff(const BuffParameters& parameters, float crit)
                : BasicBuff(parameters, "buff_crit_food_name")
            {
                AddComponent<CritChanceBonus>(crit, 0.0f);

                SetIcon(FoodBuffIconMatsetPath, "icon_buff_crit_food");
                SetTooltipDescription("buff_crit_food_description", crit);
            }
        };

        class AttackSpeedFoodBuff : public BasicBuff
        {
        public:
            AttackSpeedFoodBuff(const BuffParameters& parameters, float attackSpeed)
                : BasicBuff(parameters, "buff_crit_food_name")
            {
                AddComponent<AttackSpeedBonus>(attackSpeed);

                SetIcon(FoodBuffIconMatsetPath, "icon_buff_attack_speed_food");
                SetTooltipDescription("buff_attack_speed_food_description", attackSpeed);
            }
        };

        class BeefBuff : public BasicBuff
        {
        public:
            BeefBuff(const BuffParameters& parameters, CharacterID healer, float healthBonus, float duration)
                : BasicBuff(parameters, "buff_beef_name")
            {
                SetTotalDuration(duration);
                AddComponent<MaxHealthIncrease>(healer, healthBonus);

                SetIcon(FoodBuffIconMatsetPath, "icon_buff_beef");
                SetTooltipDescription("buff_beef_description", healthBonus, duration);
            }
        };

        const float CritAttackSpeedFoodFadeTime = 1.0f;

        static const std::string CritFoodMatsetPath = "Skeletons/Items/Food/crit_food.polymatset";

        class CritFood : public Food
        {
        public:
            CritFood(const CharacterParameters& parameters, float crit)
                : Food(parameters, CritFoodMatsetPath)
                , _crit(crit)
            {
                SetTooltip("character_crit_food_name");
                SetFadeTime(CritAttackSpeedFoodFadeTime);
            }

        private:
            void OnEaten(Character* eater) override
            {
                eater->ApplyBuff(BindBuffConstructor<CritFoodBuff>(_crit));
            }

            const float _crit;
        };

        static const std::string AttackSpeedFoodMatsetPath = "Skeletons/Items/Food/attack_speed_food.polymatset";

        class AttackSpeedFood : public Food
        {
        public:
            AttackSpeedFood(const CharacterParameters& parameters, float attackSpeed)
                : Food(parameters, AttackSpeedFoodMatsetPath)
                , _attackSpeed(attackSpeed)
            {
                SetTooltip("character_attack_speed_food_name");
                SetFadeTime(CritAttackSpeedFoodFadeTime);
            }

        private:
            void OnEaten(Character* eater) override
            {
                eater->ApplyBuff(BindBuffConstructor<AttackSpeedFoodBuff>(_attackSpeed));
            }

            const float _attackSpeed;
        };

        static const std::string BeefMatsetPath = "Skeletons/Items/Food/beef.polymatset";

        class Beef : public Food
        {
        public:
            Beef(const CharacterParameters& parameters, CharacterID owner, float healthBonus, float duration)
                : Food(parameters, BeefMatsetPath)
                , _owner(owner)
                , _healthBonus(healthBonus)
                , _bonusDuration(duration)
            {
                SetTooltip("character_beef_name");
            }

        private:
            void OnEaten(Character* eater) override
            {
                eater->ApplyBuff(BindBuffConstructor<BeefBuff>(_owner, _bonusDuration, _healthBonus));
                Kill(nullptr);
            }

            const CharacterID _owner;
            const float _healthBonus;
            const float _bonusDuration;
        };

        class ChickenBuff : public BasicBuff
        {
        public:
            ChickenBuff(const BuffParameters& parameters, CharacterID healer, float regen, float duration, float tickRate)
                : BasicBuff(parameters, "buff_chicken_name")
            {
                SetTotalDuration(duration);
                AddComponent<HealOverTime>(healer, regen, tickRate);

                SetIcon(FoodBuffIconMatsetPath, "icon_buff_chicken");
                SetTooltipDescription("buff_chicken_description", regen, duration);
            }
        };

        static const std::string ChickenMatsetPath = "Skeletons/Items/Food/chicken.polymatset";

        class Chicken : public Food
        {
        public:
            Chicken(const CharacterParameters& parameters, CharacterID owner, float regen, float duration)
                : Food(parameters, ChickenMatsetPath)
                , _owner(owner)
                , _tickRate(1.0f)
                , _regenBonus(regen)
                , _regenDuration(duration)
            {
                SetTooltip("character_chicken_name");
            }

        private:
            void OnEaten(Character* eater) override
            {
                eater->ApplyBuff(BindBuffConstructor<ChickenBuff>(_owner, _regenDuration, _regenBonus, _tickRate));
                Kill(nullptr);
            }

            const CharacterID _owner;
            const float _tickRate;
            const float _regenBonus;
            const float _regenDuration;
        };

        class HamBuff : public BasicBuff
        {
        public:
            HamBuff(const BuffParameters& parameters)
                : BasicBuff(parameters, "buff_ham_name")
            {
                SetIcon(FoodBuffIconMatsetPath, "icon_buff_ham");
                SetTooltipDescription("buff_ham_description");
            }
        };

        static const std::string HamMatsetPath = "Skeletons/Items/Food/ham.polymatset";

        class Ham : public Food
        {
        public:
            Ham(const CharacterParameters& parameters, CharacterID owner, float healAmmount)
                : Food(parameters, HamMatsetPath)
                , _owner(owner)
                , _healAmmount(healAmmount)
            {
                SetTooltip("character_ham_name");
            }

        private:
            void OnEaten(Character* eater) override
            {
                Character* owner = GetLevelLayer()->GetCharacter(_owner);
                eater->Heal(owner, _healAmmount);

                //eater->ApplyBuff(BindBuffConstructor<HamBuff>());

                Kill(nullptr);
            }

            const CharacterID _owner;
            const float _healAmmount;
        };
    }

    template <>
    void EnumeratePreloads<Character::Food>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::SkeletonCharacter>(preloads);
        preloads.insert(Character::FoodSkeletonPath);
        preloads.insert(Character::FoodParticlesPath);
    }

    template <>
    void EnumeratePreloads<Character::CritFoodBuff>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::BasicBuff>(preloads);
        EnumeratePreloads<Character::CritChanceBonus>(preloads);
        preloads.insert(Character::FoodBuffIconMatsetPath);
    }

    template <>
    void EnumeratePreloads<Character::CritFood>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Food>(preloads);
        EnumeratePreloads<Character::CritFoodBuff>(preloads);
        preloads.insert(Character::CritFoodMatsetPath);
    }

    template <>
    void EnumeratePreloads<Character::AttackSpeedFoodBuff>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::BasicBuff>(preloads);
        EnumeratePreloads<Character::AttackSpeedBonus>(preloads);
        preloads.insert(Character::FoodBuffIconMatsetPath);
    }

    template <>
    void EnumeratePreloads<Character::AttackSpeedFood>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Food>(preloads);
        EnumeratePreloads<Character::AttackSpeedFoodBuff>(preloads);
        preloads.insert(Character::AttackSpeedFoodMatsetPath);
    }

    template <>
    void EnumeratePreloads<Character::BeefBuff>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::BasicBuff>(preloads);
        EnumeratePreloads<Character::MaxHealthIncrease>(preloads);
        preloads.insert(Character::FoodBuffIconMatsetPath);
    }

    template <>
    void EnumeratePreloads<Character::Beef>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Food>(preloads);
        EnumeratePreloads<Character::BeefBuff>(preloads);
        preloads.insert(Character::BeefMatsetPath);
    }

    template <>
    void EnumeratePreloads<Character::ChickenBuff>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::BasicBuff>(preloads);
        EnumeratePreloads<Character::HealOverTime>(preloads);
        preloads.insert(Character::FoodBuffIconMatsetPath);
    }

    template <>
    void EnumeratePreloads<Character::Chicken>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Food>(preloads);
        EnumeratePreloads<Character::ChickenBuff>(preloads);
        preloads.insert(Character::ChickenMatsetPath);
    }

    template <>
    void EnumeratePreloads<Character::HamBuff>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::BasicBuff>(preloads);
        preloads.insert(Character::FoodBuffIconMatsetPath);
    }

    template <>
    void EnumeratePreloads<Character::Ham>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Food>(preloads);
        preloads.insert(Character::HamMatsetPath);
    }

    namespace Ability
    {
        CookCritFood::CookCritFood(const AbilityParameters& parameters)
            : CookFood(parameters, "ability_cook_crit_food_name", Character::CritFoodMatsetPath)
            , _critChance(0.1f)
        {
            SetIcon(FoodIconMatsetPath, "icon_cook_crit_food");
            SetCooldown(5.0f);
            SetTooltipDescription("ability_cook_crit_food_tooltip", _critChance * 100.0f);
        }

        Character::CharacterConstructor<Character::Character> CookCritFood::GetFoodConstructor() const
        {
            return Character::BindCharacterConstructor<Character::CritFood>(_critChance);
        }

        CookAttackSpeedFood::CookAttackSpeedFood(const AbilityParameters& parameters)
            : CookFood(parameters, "ability_cook_attack_speed_food_name", Character::AttackSpeedFoodMatsetPath)
            , _attackSpeedBonus(1.1f)
        {
            SetIcon(FoodIconMatsetPath, "icon_cook_attack_speed_food");
            SetCooldown(5.0f);
            SetTooltipDescription("ability_cook_attack_speed_food_tooltip", (_attackSpeedBonus - 1.0f) * 100.0f);
        }

        Character::CharacterConstructor<Character::Character> CookAttackSpeedFood::GetFoodConstructor() const
        {
            return Character::BindCharacterConstructor<Character::AttackSpeedFood>(_attackSpeedBonus);
        }

        CookBeef::CookBeef(const AbilityParameters& parameters)
            : CookFood(parameters, "ability_cook_beef_name", "Skeletons/Items/Food/beef.polymatset")
            , _beefHealthBonus(20.0f)
            , _beefBonusDuration(20.0f)
        {
            SetIcon(FoodIconMatsetPath, "icon_cook_beef");
            SetCooldown(20.0f);
            SetResourceCost(Item::Resources(0));
            SetTooltipDescription("ability_cook_beef_tooltip", _beefHealthBonus, _beefBonusDuration);
        }

        Character::CharacterConstructor<Character::Character> CookBeef::GetFoodConstructor() const
        {
            return Character::BindCharacterConstructor<Character::Beef>(GetOwner()->GetID(), _beefHealthBonus, _beefBonusDuration);
        }

        CookChicken::CookChicken(const AbilityParameters& parameters)
            : CookFood(parameters, "ability_cook_chicken_name", "Skeletons/Items/Food/chicken.polymatset")
            , _chickenRegen(3.0f)
            , _chickenDuration(15.0f)
        {
            SetIcon(FoodIconMatsetPath, "icon_cook_chicken");
            SetCooldown(20.0f);
            SetResourceCost(Item::Resources(0));
            SetTooltipDescription("ability_cook_chicken_tooltip", _chickenRegen, _chickenDuration);
        }

        Character::CharacterConstructor<Character::Character> CookChicken::GetFoodConstructor() const
        {
            return Character::BindCharacterConstructor<Character::Chicken>(GetOwner()->GetID(), _chickenRegen, _chickenDuration);
        }

        CookHam::CookHam(const AbilityParameters& parameters)
            : CookFood(parameters, "ability_cook_ham_name", "Skeletons/Items/Food/ham.polymatset")
            , _hamHealAmmount(30.0f)
        {
            SetIcon(FoodIconMatsetPath, "icon_cook_ham");
            SetCooldown(20.0f);
            SetResourceCost(Item::Resources(0));
            SetTooltipDescription("ability_cook_ham_tooltip", _hamHealAmmount);
        }

        Character::CharacterConstructor<Character::Character> CookHam::GetFoodConstructor() const
        {
            return Character::BindCharacterConstructor<Character::Ham>(GetOwner()->GetID(), _hamHealAmmount);
        }
    }

    template <>
    void EnumeratePreloads<Ability::CookFood>(PreloadSet& preloads)
    {
        EnumeratePreloads<Ability::BasicAbility>(preloads);
        preloads.insert(Ability::FoodSkeletonPath);
        preloads.insert(Ability::FoodCursorPath);
        preloads.insert(Ability::CookPotSkeletonPath);
        preloads.insert(Ability::CookPotMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Ability::CookPotSoundPaths);
    }

    template <>
    void EnumeratePreloads<Ability::CookCritFood>(PreloadSet& preloads)
    {
        EnumeratePreloads<Ability::CookFood>(preloads);
        EnumeratePreloads<Character::CritFood>(preloads);
        preloads.insert(Ability::FoodIconMatsetPath);
        preloads.insert(Character::CritFoodMatsetPath);
    }

    template <>
    void EnumeratePreloads<Ability::CookAttackSpeedFood>(PreloadSet& preloads)
    {
        EnumeratePreloads<Ability::CookFood>(preloads);
        EnumeratePreloads<Character::AttackSpeedFood>(preloads);
        preloads.insert(Ability::FoodIconMatsetPath);
        preloads.insert(Character::AttackSpeedFoodMatsetPath);
    }

    template <>
    void EnumeratePreloads<Ability::CookBeef>(PreloadSet& preloads)
    {
        EnumeratePreloads<Ability::CookFood>(preloads);
        EnumeratePreloads<Character::Beef>(preloads);
        preloads.insert(Ability::FoodIconMatsetPath);
    }

    template <>
    void EnumeratePreloads<Ability::CookChicken>(PreloadSet& preloads)
    {
        EnumeratePreloads<Ability::CookFood>(preloads);
        EnumeratePreloads<Character::Chicken>(preloads);
        preloads.insert(Ability::FoodIconMatsetPath);
    }

    template <>
    void EnumeratePreloads<Ability::CookHam>(PreloadSet& preloads)
    {
        EnumeratePreloads<Ability::CookFood>(preloads);
        EnumeratePreloads<Character::Ham>(preloads);
        preloads.insert(Ability::FoodIconMatsetPath);
    }
}
