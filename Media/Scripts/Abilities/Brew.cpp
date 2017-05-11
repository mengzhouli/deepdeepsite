#include "Abilities/Brew.hpp"

#include "Characters/SkeletonCharacter.hpp"
#include "characters/CharacterTraits.hpp"

#include "AbilityUtility.hpp"
#include "NavigationUtility.hpp"
#include "ContentUtility.hpp"
#include "SkeletonUtility.hpp"

namespace Dwarf
{
    static const std::string BrewBeerKegSkeletonPath = "Skeletons/Characters/Beerkeg/beerkeg.skel";
    static const std::string BrewBeerKegMatsetPath = "Skeletons/Characters/Beerkeg/beerkeg.polymatset";

    static const std::string BrewWineKegSkeletonPath = "Skeletons/Characters/Winerack/winerack.skel";
    static const std::string BrewWineKegMatsetPath = "Skeletons/Characters/Winerack/winerack.polymatset";

    namespace Ability
    {
        static const std::string BrewCursorPath = "HUD/abilities.cursorset";
        static const std::string BrewIconMatsetPath = "HUD/abilities.hudmatset";

        static const Audio::SoundPathVector BrewKegSpawnSounds =
        {
        };

        Brew::Brew(const AbilityParameters& parameters, const std::string& nameCode, const std::string& skeletonPath, const std::string& matset)
            : BasicAbility(parameters, nameCode, AbilityType_OneTarget)
            , _skeletonPath(skeletonPath)
            , _matsetPath(matset)
        {
            SetCursor(BrewCursorPath, "cursor_brew");

            _spawnSounds.AddSounds(BrewKegSpawnSounds);
        }

        void Brew::ClearState()
        {
            _executing = false;
            _placementValid = false;
            setDrawPlacementKeg(false);
            setDrawActionKeg(false);
            _kegDropAnimationTimer = -1.0f;
            _kegSpawnTimer = -1.0f;
            _kegDropTimer = -1.0f;
        }

        void Brew::SetPrimaryTarget(const Vector2f& pos)
        {
            Vector2f validPos;
            _placementValid = VerifyKegPosition(pos, validPos);
            _placementKeg->SetPosition(validPos);
            setDrawPlacementKeg(true);
        }

        bool Brew::IsInValidState() const
        {
            return _placementValid;
        }

        Character::Action Brew::GenerateAction() const
        {
            if (_placementValid)
            {
                return Character::CreateUseAbilityAction(GetID(), _placementKeg->GetPosition());
            }
            else
            {
                return Character::CreateEmptyAction();
            }
        }

        void Brew::ExecuteAction(const Character::Action& action)
        {
            Character::Character* owner = GetOwner();
            _placementKeg->SetPosition(Vector2f(action.AbilityPosition.StartX, action.AbilityPosition.StartY));
            _executing = owner->MoveTo(_placementKeg->GetPosition());
        }

        void Brew::CancelAction()
        {
            ClearState();
        }

        bool Brew::IsExecutingAction() const
        {
            return _executing;
        }

        void Brew::OnUpdate(double totalTime, float dt)
        {
            BasicAbility::OnUpdate(totalTime, dt);

            Character::Character* owner = GetOwner();
            Character::Brewer* brewer = GetOwner<Character::Brewer>();
            assert(brewer);
            Rayf ownerAttachRay = owner->GetAttachPoint(brewer->GetKegAttachPointName());
            Animation::AttachmentInfo attachInfo = Animation::GetSkeletonAttachmentInfo(
                Animation::IsCharacterInvertedX(owner), Animation::IsCharacterInvertedY(owner), false, ownerAttachRay,
                _actionKeg, "attach_bottom", "attach_top");

            _actionKeg->SetInvertedX(attachInfo.InvertX);
            _actionKeg->SetInvertedY(attachInfo.InvertY);
            _actionKeg->SetPosition(attachInfo.Position);
            _actionKeg->SetScale(attachInfo.Scale);
            _actionKeg->SetRotation(attachInfo.Rotation);
            _actionKeg->Update(totalTime, dt);

            _placementKeg->SetColor(GetAbilityUIColor(_placementValid));
            _placementKeg->Update(totalTime, dt);

            if (_executing)
            {
                if (_kegDropAnimationTimer >= 0.0f)
                {
                    _kegDropAnimationTimer -= dt;
                    _kegSpawnTimer -= dt;
                    _kegDropTimer -= dt;

                    if (_kegDropAnimationTimer < 0.0f)
                    {
                        _executing = false;
                    }
                    else if (_kegDropTimer < 0.0f)
                    {
                        if (_drawActionKeg)
                        {
                            setDrawActionKeg(false);

                            Vector2f spawnPos;
                            VerifyKegPosition(_actionKeg->GetPosition(), spawnPos);

                            // spawn keg
                            Level::LevelLayerInstance* layer = owner->GetLevelLayer();
                            Character::Character* keg = layer->SpawnCharacter(spawnPos, "keg", owner->GetController(), GetKegConstructor());
                            keg->SetRotation(_actionKeg->GetRotation());
                            keg->SetScale(_actionKeg->GetScale());

                            Character::Character* prevKeg = layer->GetCharacter(_prevKeg);
                            if (prevKeg)
                            {
                                prevKeg->Kill(nullptr);
                            }
                            _prevKeg = keg->GetID();

                            StartCooldown();
                        }
                    }
                    else if (_kegSpawnTimer < 0.0f)
                    {
                        if (!_drawActionKeg)
                        {
                            setDrawActionKeg(true);
                        }
                    }
                }
                else
                {
                    const float minDropDist = owner->GetBounds().W * 0.5f;
                    bool closeEnoughToDropPos = Vector2f::Distance(owner->GetPosition(), _placementKeg->GetPosition()) < minDropDist;

                    if (!owner->IsMoving() || closeEnoughToDropPos)
                    {
                        owner->StopMoving();

                        auto info = brewer->PlayKegThrowdownAnimation();
                        _kegDropAnimationTimer = info.TotalDuration;
                        _kegSpawnTimer = info.KegSpawnTime;
                        _kegDropTimer = info.KegDropTime;
                    }
                }
            }
        }

        void Brew::OnLoadContent(Content::ContentManager* contentManager)
        {
            BasicAbility::OnLoadContent(contentManager);

            _placementKeg = Content::CreateSkeletonInstance(contentManager, _skeletonPath, _matsetPath);
            _actionKeg = Content::CreateSkeletonInstance(contentManager, _skeletonPath, _matsetPath);

            _spawnSounds.LoadContent(contentManager);
        }

        void Brew::OnUnloadContent()
        {
            BasicAbility::OnUnloadContent();

            SafeRelease(_placementKeg);
            SafeRelease(_actionKeg);

            _spawnSounds.UnloadContent();
        }

        void Brew::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            BasicAbility::OnDraw(levelRenderer);
            if (_drawPlacementKeg)
            {
                levelRenderer->AddDrawable(_placementKeg, false, false);
            }
            // action keg drawn as sub-drawable of owner
        }

        bool Brew::VerifyKegPosition(const Vector2f& pos, Vector2f& placePos)
        {
            Character::Character* owner = GetOwner();
            Level::LevelLayerInstance* layer = owner->GetLevelLayer();
            if (Level::GetPlaceTarget(layer, Pathfinding::EdgeType_Walk, pos, 400.0f, placePos))
            {
                return true;
            }
            else
            {
                placePos = pos + (_placementKeg->GetPosition() - _placementKeg->GetDrawBounds().Middle());
                return false;
            }
        }

        void Brew::setDrawPlacementKeg(bool drawPlacement)
        {
            _drawPlacementKeg = drawPlacement;
        }

        void Brew::setDrawActionKeg(bool drawActionKeg)
        {
            Character::Character* owner = GetOwner<Character::Character>();
            Character::Brewer* brewer = GetOwner<Character::Brewer>();
            assert(brewer != nullptr);

            if (_drawActionKeg != drawActionKeg)
            {
                std::string attachName = brewer->GetKegAttachPointName();
                if (drawActionKeg)
                {
                    owner->AttachDrawable(attachName, _actionKeg);
                }
                else
                {
                    owner->DetachDrawable(attachName, _actionKeg);
                }

                _drawActionKeg = drawActionKeg;
            }
        }
    }

    namespace Character
    {
        const float KegDeathFadeTime = 1.0f;

        class Keg : public SkeletonCharacter, public Drinkable
        {
        public:
            Keg(const CharacterParameters& parameters, const std::string& skeletonPath, const std::string& matset)
                : SkeletonCharacter(parameters, skeletonPath, matset)
            {
                SetEntityMask(CharacterMask_Usable | CharacterMask_Constructed);
                SetMoveType(MoveType_None);

                DisablePhysics();
                SetInvulnerable(true);
                SetSkeletonRotatesWithDirection(true);
            }

        protected:
            ~Keg() override
            {
            }

            void OnUpdate(double totalTime, float dt) override
            {
                if (!IsAlive())
                {
                    _fadeTimer -= dt;

                    Color c = GetColor();
                    c.A = Lerp(0, 255, Saturate(_fadeTimer / KegDeathFadeTime));
                    SetColor(c);

                    if (_fadeTimer < 0.0f)
                    {
                        Terminate();
                    }
                }
            }

            void OnPostInteractedWith(Character* interactor) override
            {
                OnDrank(interactor);
                if (IsA<Drinker>(interactor))
                {
                    AsA<Drinker>(interactor)->OnPostDrink(this);
                }
            }

        private:
            virtual void OnDrank(Character* drinker) = 0;

            float _fadeTimer = KegDeathFadeTime;
        };

        class BeerKeg : public Keg
        {
        public:
            BeerKeg(const CharacterParameters& parameters)
                : Keg(parameters, BrewBeerKegSkeletonPath, BrewBeerKegMatsetPath)
            {
                SetTooltip("character_beer_keg_name");
            }

            DrinkableType GetDrinkableType() const override
            {
                return DrinkableType::Beer;
            }

        private:
            void OnDrank(Character* drinker) override
            {
            }
        };

        class WineKeg : public Keg
        {
        public:
            WineKeg(const CharacterParameters& parameters)
                : Keg(parameters, BrewWineKegSkeletonPath, BrewWineKegMatsetPath)
            {
                SetTooltip("character_wine_keg_name");
            }

            DrinkableType GetDrinkableType() const override
            {
                return DrinkableType::Wine;
            }

        private:
            void OnDrank(Character* drinker) override
            {
            }
        };
    }

    template <>
    void EnumeratePreloads<Character::Keg>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::SkeletonCharacter>(preloads);
    }

    template <>
    void EnumeratePreloads<Character::BeerKeg>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Keg>(preloads);
        preloads.insert(BrewBeerKegSkeletonPath);
        preloads.insert(BrewBeerKegMatsetPath);
    }

    template <>
    void EnumeratePreloads<Character::WineKeg>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Keg>(preloads);
        preloads.insert(BrewWineKegSkeletonPath);
        preloads.insert(BrewWineKegMatsetPath);
    }

    namespace Ability
    {
        BrewBeer::BrewBeer(const AbilityParameters& parameters)
            : Brew(parameters, "ability_brew_beer_name", BrewBeerKegSkeletonPath, BrewBeerKegMatsetPath)
            , _damageReduction(0.1f)
        {
            SetIcon(BrewIconMatsetPath, "icon_brew_beer");
            SetCooldown(5.0f);
            SetTooltipDescription("ability_brew_beer_tooltip", _damageReduction * 100.0f);
        }

        Character::CharacterConstructor<Character::Character> BrewBeer::GetKegConstructor() const
        {
            return Character::BindCharacterConstructor<Character::BeerKeg>();
        }

        BrewWine::BrewWine(const AbilityParameters& parameters)
            : Brew(parameters, "ability_brew_wine_name", BrewWineKegSkeletonPath, BrewWineKegMatsetPath)
            , _regen(2.0f)
        {
            SetIcon(BrewIconMatsetPath, "icon_brew_wine");
            SetCooldown(5.0f);
            SetTooltipDescription("ability_brew_wine_tooltip", _regen);
        }

        Character::CharacterConstructor<Character::Character> BrewWine::GetKegConstructor() const
        {
            return Character::BindCharacterConstructor<Character::WineKeg>();
        }
    }

    template <>
    void EnumeratePreloads<Ability::Brew>(PreloadSet& preloads)
    {
        EnumeratePreloads<Ability::BasicAbility>(preloads);

        preloads.insert(Ability::BrewCursorPath);
        preloads.insert(Ability::BrewIconMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Ability::BrewKegSpawnSounds);
    }

    template <>
    void EnumeratePreloads<Ability::BrewBeer>(PreloadSet& preloads)
    {
        EnumeratePreloads<Ability::Brew>(preloads);

        EnumeratePreloads<Character::BeerKeg>(preloads);
        preloads.insert(BrewBeerKegSkeletonPath);
        preloads.insert(BrewBeerKegMatsetPath);
    }

    template <>
    void EnumeratePreloads<Ability::BrewWine>(PreloadSet& preloads)
    {
        EnumeratePreloads<Ability::Brew>(preloads);

        EnumeratePreloads<Character::WineKeg>(preloads);
        preloads.insert(BrewWineKegSkeletonPath);
        preloads.insert(BrewWineKegMatsetPath);
    }
}
