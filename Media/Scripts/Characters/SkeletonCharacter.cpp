#include "Characters/SkeletonCharacter.hpp"

#include "Physics/SkeletonCollision.hpp"
#include "HUD/Tooltip.hpp"

namespace Dwarf
{
    namespace Character
    {
        static const std::pair<float, float> PhysicsCollisionSoundRadius = { 500, 3500 };
        static const float PhysicsCollisionSoundVolume = 0.5f;

        SkeletonCharacter::SkeletonCharacter(const CharacterParameters& parameters, const std::string& skeletonPath, const std::string& matsetPath)
            : Character(parameters)
            , _drawSkeleton(true)
            , _skeletonScale(1.0f)
            , _skeletonColor(Color::White)
            , _skeletonCastsShadows(false)
            , _skeletonHiddenByFog(false)
            , _skeletonRotates(false)
            , _skeletonInverts(true)

            , _skeletonPath(skeletonPath)
            , _matsetPath(matsetPath)
            , _skeleton(NULL)

            , _skeletonJointStrength(-1.0f)
            , _collision(NULL)

            , _materialOptions()

            , _hasPendingAnimation(false)
            , _pendingAnimation()

            , _aliveIconMatsetPath()
            , _aliveIconMaterialName()
            , _aliveIcon(nullptr)

            , _deadIconMatsetPath()
            , _deadIconMaterialName()
            , _deadIcon(nullptr)

            , _description()
            , _tooltip(nullptr)
        {
        }

        const HUD::PanelDrawable* SkeletonCharacter::GetIcon() const
        {
            if (!IsAlive() && _deadIcon)
            {
                return _deadIcon;
            }
            else
            {
                return _aliveIcon;
            }
        }

        const HUD::Panel* SkeletonCharacter::GetTooltip() const
        {
            return _tooltip;
        }

        const Rectanglef& SkeletonCharacter::GetBounds() const
        {
            return _skeleton->GetSkeletonBounds();
        }

        Physics::Collision* SkeletonCharacter::GetCollision() const
        {
            return _collision;
        }

        Rayf SkeletonCharacter::GetAttachPoint(const std::string& name) const
        {
            auto iter = _customAttachPoints.find(name);
            if (iter != _customAttachPoints.end())
            {
                Rayf result(Vector2f::Zero, Vector2f::Zero);

                const auto& jointA = iter->second.first;
                const auto& jointB = iter->second.second;
                bool hasA = _skeleton->HasJoint(jointA);
                bool hasB = _skeleton->HasJoint(jointB);

                if (hasA && hasB)
                {
                    result.Position = _skeleton->GetJointPosition(jointA);
                    result.Direction = _skeleton->GetJointPosition(jointB) - result.Position;
                }
                else if (hasA)
                {
                    result.Position = _skeleton->GetJointPosition(jointA);
                }
                else if (hasB)
                {
                    result.Direction = _skeleton->GetJointPosition(jointB);
                }

                return result;
            }
            else if (_skeleton->HasJoint(name))
            {
                return Rayf(_skeleton->GetJointPosition(name), Vector2f::Zero);
            }
            else
            {
                return Rayf(_skeleton->GetPosition(), Vector2f::Zero);
            }
        }

        Rayf SkeletonCharacter::GetWeaponAttachPoint(const std::string& name, Item::WeaponType type) const
        {
            return GetAttachPoint(name);
        }

        bool SkeletonCharacter::IsAnimationFinished() const
        {
            if (_hasPendingAnimation)
            {
                return false;
            }
            else
            {
                return _skeleton->IsAnimationFinished();
            }
        }

        bool SkeletonCharacter::HasAnimationTagPassed(const std::string& tag) const
        {
            return _skeleton->HasAnimationTagPassed(tag);
        }

        bool SkeletonCharacter::HasAnimationTagJustPassed(const std::string& tag) const
        {
            return _skeleton->HasAnimationTagJustPassed(tag);
        }

        float SkeletonCharacter::GetAnimationLength() const
        {
            return _skeleton->GetAnimationLength();
        }

        float SkeletonCharacter::GetAnimationLength(const std::string& animationName) const
        {
            return _skeleton->GetAnimationLength(animationName);
        }

        void SkeletonCharacter::AttachDrawable(const std::string& name, const Graphics::IDrawable* drawable)
        {
            std::string attachJoint = getAttachJoint(name);
            if (!attachJoint.empty())
            {
                _skeleton->AddChildDrawable(attachJoint, drawable);
            }
            else
            {
                return Character::AttachDrawable(name, drawable);
            }
        }

        void SkeletonCharacter::DetachDrawable(const std::string& name, const Graphics::IDrawable* drawable)
        {
            std::string attachJoint = getAttachJoint(name);
            if (!attachJoint.empty())
            {
                _skeleton->RemoveChildDrawable(attachJoint, drawable);
            }
            else
            {
                return Character::DetachDrawable(name, drawable);
            }
        }

        void SkeletonCharacter::OnLoadContent(Content::ContentManager* contentManager)
        {
            const Animation::Skeleton* skel = NULL;
            const Graphics::PolygonMaterialSet* matset = NULL;
            try
            {
                skel = contentManager->Load<Animation::Skeleton>(_skeletonPath);
                _skeleton = new Animation::SkeletonInstance(skel);
                SafeRelease(skel);

                matset = contentManager->Load<Graphics::PolygonMaterialSet>(_matsetPath);
                ApplyMaterialSet("", matset);
                SafeRelease(matset);

                for (MaterialOptionsMap::const_iterator i = _materialOptions.begin(); i != _materialOptions.end(); i++)
                {
                    const std::string& group = i->first;
                    const MaterialSelector& selector = i->second;
                    const std::string& materialPath = selector.GetMaterial();
                    if (materialPath.size() > 0)
                    {
                        matset = contentManager->Load<Graphics::PolygonMaterialSet>(materialPath);
                        ApplyMaterialSet(group, matset);
                        SafeRelease(matset);
                    }
                }

                _collision = CreateCollision();
            }
            catch (...)
            {
                SafeRelease(skel);
                SafeRelease(matset);
                SafeRelease(_skeleton);

                throw;
            }

            SetSkeletonColor(_skeletonColor);
            SetSkeletonScale(_skeletonScale);
            SetSkeletonJointStrength(_skeletonJointStrength);
            _skeleton->SetInvertedX(_skeletonInvertedX);
            _skeleton->SetInvertedY(_skeletonInvertedY);

            _aliveIcon = HUD::CreateItemIcon(contentManager, _aliveIconMatsetPath, _aliveIconMaterialName, GetSkeleton());

            if (!_deadIconMatsetPath.empty() && !_deadIconMaterialName.empty())
            {
                _deadIcon = HUD::CreateItemIcon(contentManager, _deadIconMatsetPath, _deadIconMaterialName, GetSkeleton());
            }

            if (!_description.empty())
            {
                _tooltip = HUD::CreateSimpleTextTooltip(contentManager, _description);
            }

            _physicsMaterialSounds.LoadContent(contentManager);
        }

        void SkeletonCharacter::OnUnloadContent()
        {
            SafeRelease(_skeleton);
            _brokenPieces.clear();

            for (ChosenMaterialMap::iterator iter = _chosenMaterials.begin(); iter != _chosenMaterials.end(); iter++)
            {
                SafeRelease(iter->second);
            }
            _chosenMaterials.clear();

            SafeRelease(_collision);

            SafeRelease(_aliveIcon);
            SafeRelease(_deadIcon);
            SafeRelease(_tooltip);

            _physicsMaterialSounds.UnloadContent();
        }

        void SkeletonCharacter::OnUpdate(double totalTime, float dt)
        {
            if (_hasPendingAnimation)
            {
                _hasPendingAnimation = false;
                if (!_pendingAnimation.loop ||
                    (_pendingAnimation.loop && (!_skeleton->IsLooping() || _skeleton->GetCurrentAnimationSet() != _pendingAnimation.animationSet)))
                {
                    _skeleton->PlayAnimationSet(_pendingAnimation.animationSet, _pendingAnimation.loop,
                                                _pendingAnimation.transitionTime, _pendingAnimation.startOffset);
                }
            }

            float animationDT = dt;
            if (_playFastAnimationsForMoving && IsMoving())
            {
                animationDT *= GetMoveSpeedMultiplier();
            }
            else if (_playFastAnimationsForAttacking && IsAttacking())
            {
                animationDT *= GetAttackSpeedMultiplier();
            }

            _physicsSoundResetTimer -= dt;
            auto checkForPhysicsMaterialSounds = [&](const Physics::Collision* collision)
            {
                if (collision->GetBehavior() != Physics::CollisionBehavior_Dynamic ||
                    _physicsMaterialSounds.Count() == 0 ||
                    _physicsSoundResetTimer > 0.0f)
                {
                    return;
                }

                for (const Physics::Contact& contact : collision->GetCurrentContacts())
                {
                    float velocity = Vector2f::Dot(contact.Velocity, -contact.Normal);
                    static const float VelocityThreshold = 200.0f;
                    if (velocity >= VelocityThreshold)
                    {
                        static const float MaxVolumeVelocity = 3000.0f;
                        float volume = Saturate((velocity - VelocityThreshold) / MaxVolumeVelocity);

                        auto soundManager = GetLevel()->GetSoundManager();
                        soundManager->PlaySinglePositionalSound(_physicsMaterialSounds.GetNextSound(), Audio::SoundPriority::High,
                                                                contact.Position, PhysicsCollisionSoundRadius.first, PhysicsCollisionSoundRadius.second,
                                                                PhysicsCollisionSoundVolume * volume);

                        static const float ResetTime = 0.1f;
                        _physicsSoundResetTimer = ResetTime;
                        break;
                    }
                }
            };

            _skeleton->Update(totalTime, animationDT);
            _collision->Update(totalTime, dt);
            checkForPhysicsMaterialSounds(_collision);

            for (auto& brokenMat : _brokenPieces)
            {
                brokenMat.skeleton->Update(totalTime, dt);
                brokenMat.collision->Update(totalTime, dt);
                checkForPhysicsMaterialSounds(brokenMat.collision);
            }
        }

        void SkeletonCharacter::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            if (_drawSkeleton)
            {
                levelRenderer->AddDrawable(_skeleton, _skeletonCastsShadows, true, GetMinimumColor(), GetEmissiveColor(), GetHighlightColor());
            }
        }

        Animation::SkeletonInstance* SkeletonCharacter::GetSkeleton() const
        {
            return _skeleton;
        }

        void SkeletonCharacter::SetIcon(const std::string& iconMatsetPath, const std::string& iconMaterialName)
        {
            _aliveIconMatsetPath = iconMatsetPath;
            _aliveIconMaterialName = iconMaterialName;
        }

        void SkeletonCharacter::SetDeadIcon(const std::string& iconMatsetPath, const std::string& iconMaterialName)
        {
            _deadIconMatsetPath = iconMatsetPath;
            _deadIconMaterialName = iconMaterialName;
        }

        void SkeletonCharacter::SetTooltip(const std::string codeName, ...)
        {
            va_list args;
            va_start(args, codeName);
            _description = Format(GetLevel()->GetStringTable()->GetString(codeName).c_str(), args);
            va_end(args);
        }

        void SkeletonCharacter::SetDrawSkeleton(bool drawSkeleton)
        {
            _drawSkeleton = drawSkeleton;
        }

        void SkeletonCharacter::SetSkeletonCastsShadows(bool castsShadows)
        {
            _skeletonCastsShadows = castsShadows;
        }

        bool SkeletonCharacter::GetSkeletonCastsShadows() const
        {
            return _skeletonCastsShadows;
        }

        void SkeletonCharacter::SetPlayFasterAnimationsForMoving(bool fastAnimations)
        {
            _playFastAnimationsForMoving = fastAnimations;
        }

        void SkeletonCharacter::SetPlayFasterAnimationsForAttacking(bool fastAnimations)
        {
            _playFastAnimationsForAttacking = fastAnimations;
        }

        void SkeletonCharacter::SetSkeletonScale(float scale)
        {
            _skeletonScale = scale;
            if (_skeleton)
            {
                _skeleton->SetScale(_skeletonScale);
            }
        }

        void SkeletonCharacter::SetSkeletonColor(const Color& color)
        {
            _skeletonColor = color;
            if (_skeleton)
            {
                _skeleton->SetColor(GetColor() * _skeletonColor);
            }
        }

        void SkeletonCharacter::SetSkeletonInvertedX(bool invertedX)
        {
            _skeletonInvertedX = invertedX;
        }

        void SkeletonCharacter::SetSkeletonInvertedY(bool invertedY)
        {
            _skeletonInvertedY = invertedY;
        }

        void SkeletonCharacter::SetSkeletonJointStrength(float strength)
        {
            _skeletonJointStrength = strength;
            if (_collision && IsA<Physics::SkeletonCollision>(_collision))
            {
                AsA<Physics::SkeletonCollision>(_collision)->SetJointStrength(strength);
            }
        }

        void SkeletonCharacter::SetSkeletonRotatesWithDirection(bool rotates)
        {
            _skeletonRotates = rotates;
        }

        void SkeletonCharacter::SetSkeletonInvertsWithDirection(bool inverts)
        {
            _skeletonInverts = inverts;
        }

        void SkeletonCharacter::SetMaterialCollisionSound(const Audio::SoundPathVector& sounds)
        {
            _physicsMaterialSounds.AddSounds(sounds);
        }

        void SkeletonCharacter::InsertBrokenMaterial(const Animation::BrokenMaterial material)
        {
            _brokenPieces.push_back(material);
        }

        void SkeletonCharacter::AddCustomAttachPoint(const std::string& name, const std::string& jointA, const std::string&jointB)
        {
            _customAttachPoints[name] = std::make_pair(jointA, jointB);
        }

        void SkeletonCharacter::ApplyMaterialSet(const std::string& group, const Graphics::PolygonMaterialSet* matset)
        {
            if (matset)
            {
                matset->AddRef();
                _chosenMaterials[group] = matset;

                if (_skeleton)
                {
                    _skeleton->MergeMaterialSet(matset);
                }
            }
        }

        void SkeletonCharacter::RemoveMaterialSet(const std::string& group)
        {
            ChosenMaterialMap::iterator iter = _chosenMaterials.find(group);
            if (iter != _chosenMaterials.end())
            {
                SafeRelease(iter->second);
                _chosenMaterials.erase(iter);

                _skeleton->ClearMaterials();
                for (iter = _chosenMaterials.begin(); iter != _chosenMaterials.end(); iter++)
                {
                    _skeleton->MergeMaterialSet(iter->second);
                }
            }
        }

        void SkeletonCharacter::AddMaterial(const std::string& group, const std::string& materialPath, float probability)
        {
            _materialOptions[group].Add(materialPath, probability);
        }

        void SkeletonCharacter::ClearMaterialGroup(const std::string& group)
        {
            _materialOptions[group].Clear();
        }

        float SkeletonCharacter::PlayAnimation(const std::string& anim, bool loop, float transitionTime, float startOffset)
        {
            _hasPendingAnimation = true;
            _pendingAnimation.animationSet.Clear();
            _pendingAnimation.animationSet.AddAnimation(anim, 1.0f);
            _pendingAnimation.loop = loop;
            _pendingAnimation.transitionTime = transitionTime;
            _pendingAnimation.startOffset = startOffset;
            return Max(GetAnimationLength(anim) - startOffset, 0.0f);
        }

        float SkeletonCharacter::PlayAnimationSet(const Animation::AnimationSet& animSet, bool loop, float transitionTime, float startOffset)
        {
            _hasPendingAnimation = true;
            _pendingAnimation.animationSet = animSet;
            _pendingAnimation.loop = loop;
            _pendingAnimation.transitionTime = transitionTime;
            _pendingAnimation.startOffset = startOffset;
            return Max(GetAnimationLength(_pendingAnimation.animationSet.PeekNextAnimation()) - startOffset, 0.0f);
        }

        bool SkeletonCharacter::IsPlayingAnimation() const
        {
            return _hasPendingAnimation || !_skeleton->IsAnimationFinished();
        }

        bool SkeletonCharacter::IsPlayingAnimation(const std::string& anim) const
        {
            return _skeleton->GetCurrentAnimation() == anim || (_hasPendingAnimation && _pendingAnimation.animationSet.PeekNextAnimation() == anim);
        }

        bool SkeletonCharacter::IsPlayingAnimation(const Animation::AnimationSet& animSet) const
        {
            return _skeleton->IsPlayingAnimationFromSet(animSet) || (_hasPendingAnimation && animSet.ContainsAnimation(_pendingAnimation.animationSet.PeekNextAnimation()));
        }

        const std::string& SkeletonCharacter::GetCurrentAnimation() const
        {
            return _skeleton->GetCurrentAnimation();
        }

        void SkeletonCharacter::BreakMaterial(const std::string& material)
        {
            if (!_skeleton->HasJoint(material) || !_skeleton->GetMaterial(material))
            {
                return;
            }

            InsertBrokenMaterial(BreakSkeletonMaterial(_skeleton, GetLevelLayer()->GetPhysicsWorld(), material));
        }

        Physics::Collision* SkeletonCharacter::CreateCollision()
        {
            return new Physics::SkeletonCollision(GetLevelLayer()->GetPhysicsWorld(), _skeleton, _skeletonJointStrength);
        }

        void SkeletonCharacter::OnPositionChange(const Vector2f& oldPos, const Vector2f& newPos)
        {
            _skeleton->SetPosition(newPos);
        }

        void SkeletonCharacter::OnScaleChange(float oldScale, float newScale)
        {
            _skeleton->SetScale(newScale * _skeletonScale);
        }

        void SkeletonCharacter::OnRotationChange(const Rotatorf& oldRot, const Rotatorf& newRot)
        {
            if (_skeletonInverts)
            {
                bool invert = _skeletonInvertedX ? newRot.ToVector().X <= 0.0f : newRot.ToVector().X > 0.0f;
                _skeleton->SetInvertedX(invert);
            }

            if (_skeletonRotates)
            {
                if (_skeleton->IsInvertedX())
                {
                    _skeleton->SetRotation(Rotatorf::Reflect(newRot, Rotatorf::PiOver2) + Rotatorf::Pi);
                }
                else
                {
                    _skeleton->SetRotation(newRot + Rotatorf::Pi);
                }
            }

        }

        void SkeletonCharacter::OnColorChange(const Color& oldCol, const Color& newCol)
        {
            Color modulatedColor = newCol * _skeletonColor;
            _skeleton->SetColor(modulatedColor);
            for (auto& brokenMat : _brokenPieces)
            {
                brokenMat.skeleton->SetColor(modulatedColor);
            }
        }

        void SkeletonCharacter::OnSpawn()
        {
            if (_collision)
            {
                _collision->SetPosition(GetPosition());
            }

            Character::OnSpawn();
        }

        std::string SkeletonCharacter::getAttachJoint(const std::string& attachName) const
        {
            auto iter = _customAttachPoints.find(attachName);
            if (_skeleton->HasJoint(attachName))
            {
                return attachName;
            }
            else if (iter != _customAttachPoints.end())
            {
                if (_skeleton->HasJoint(iter->second.first))
                {
                    return iter->second.first;
                }
                else if (_skeleton->HasJoint(iter->second.second))
                {
                    return iter->second.second;
                }
                else
                {
                    return "";
                }
            }
            else
            {
                return "";
            }
        }
    }

    template <>
    void EnumeratePreloads<Character::SkeletonCharacter>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Character>(preloads);

        EnumerateTooltipPreloads(preloads);
    }
}
