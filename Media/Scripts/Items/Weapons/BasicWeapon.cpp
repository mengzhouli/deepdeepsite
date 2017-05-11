#include "Items/Weapons/BasicWeapon.hpp"
#include "ContentUtility.hpp"
#include "SkeletonUtility.hpp"

#include "HUD/Tooltip.hpp"

#include "Random.hpp"

namespace Dwarf
{
    namespace Item
    {
        static const std::pair<float, float> PhysicsCollisionSoundRadius = { 500, 3500 };
        static const float PhysicsCollisionSoundVolume = 0.5f;

        static std::string GetWeaponName(const ItemParameters& parameters, const std::string& nameCode)
        {
            assert(parameters.Layer);
            return parameters.Layer->GetLevel()->GetStringTable()->GetString(nameCode);
        }

        BasicWeapon::BasicWeapon(const ItemParameters& parameters, const std::string& nameCode, WeaponType type, const std::string& skeletonPath, const std::string& matsetPath)
            : Weapon(parameters, GetWeaponName(parameters, nameCode), type)
            , _iconMatsetPath()
            , _iconMaterialName()
            , _description()
            , _icon(nullptr)
            , _tooltip(nullptr)
            , _weaponAttachPointA()
            , _weaponAttachPointB()
            , _damageJoint()
            , _dps(Character::DamageType_Type_Melee | Character::DamageType_Element_Physical, 1.0f)
            , _armor(0.0f)
            , _blocks()
            , _skeletonPath(skeletonPath)
            , _matsetPath(matsetPath)
            , _skeleton(nullptr)
            , _collision(nullptr)
            , _pickupSoundRadius(2000.0f, 3000.0f)
            , _pickupSoundVolume(1.0f)
            , _brokenPieces()
        {
            SetDestructionFadeTimers(3.0f, 4.0f);
        }

        BasicWeapon::~BasicWeapon()
        {
        }

        void BasicWeapon::SetIcon(const std::string& iconMatsetPath, const std::string& iconMaterialName)
        {
            _iconMatsetPath = iconMatsetPath;
            _iconMaterialName = iconMaterialName;
        }

        void BasicWeapon::SetTooltipDescription(const std::string codeName, ...)
        {
            va_list args;
            va_start(args, codeName);
            _description = Format(GetLevel()->GetStringTable()->GetString(codeName).c_str(), args);
            va_end(args);
        }

        void BasicWeapon::SetTooltipFlavor(const std::string codeName)
        {
            _flavor = GetLevel()->GetStringTable()->GetString(codeName);
        }

        void BasicWeapon::SetAttachPoints(const std::string& weaponAttachA, const std::string& weaponAttachB)
        {
            _weaponAttachPointA = weaponAttachA;
            _weaponAttachPointB = weaponAttachB;
        }

        void BasicWeapon::SetDamageJoint(const std::string& damageJoint)
        {
            _damageJoint = damageJoint;
        }

        void BasicWeapon::SetDPS(const Character::Damage& dps)
        {
            _dps = dps;
        }

        void BasicWeapon::SetArmorAmmount(float armor)
        {
            _armor = armor;
        }

        void BasicWeapon::SetBlockChance(Character::DamageType type, float chance, float block)
        {
            _blocks[type] = BlockChance(chance, block);
        }

        void BasicWeapon::BreakMaterial(const std::string& material)
        {
            if (!_skeleton->HasJoint(material) || !_skeleton->GetMaterial(material))
            {
                return;
            }

            _brokenPieces.push_back(BreakSkeletonMaterial(_skeleton, GetLevelLayer()->GetPhysicsWorld(), material));
        }

        void BasicWeapon::SetMaterialCollisionSound(const Audio::SoundPathVector& sounds)
        {
            _physicsMaterialSounds.AddSounds(sounds);
        }

        void BasicWeapon::AddSelectionSounds(const Audio::SoundPathVector& sounds)
        {
            // Keep a consistent sound for each individual item type
            if (sounds.size() > 0)
            {
                _selectionSounds.Clear();
                _selectionSounds.AddSound(Random::SeededRandomItem(sounds, GetName()));
            }
        }

        void BasicWeapon::OnEquip(Character::Character* newOwner)
        {
            newOwner->AttachDrawable(GetAttachPoint(), _skeleton);
        }

        void BasicWeapon::OnUnEquip(Character::Character* oldOwner)
        {
            oldOwner->DetachDrawable(GetAttachPoint(), _skeleton);
        }

        void BasicWeapon::OnPositionChange(const Vector2f& oldPos, const Vector2f& newPos)
        {
            _skeleton->SetPosition(newPos);
        }

        void BasicWeapon::OnScaleChange(float oldScale, float newScale)
        {
            _skeleton->SetScale(newScale);
        }

        void BasicWeapon::OnRotationChange(const Rotatorf& oldRot, const Rotatorf& newRot)
        {
            _skeleton->SetRotation(newRot);
        }

        void BasicWeapon::OnColorChange(const Color& oldCol, const Color& newCol)
        {
            _skeleton->SetColor(newCol);
            for (auto& brokenMat : _brokenPieces)
            {
                brokenMat.skeleton->SetColor(newCol);
            }
        }

        float BasicWeapon::GetRange() const
        {
            const Rectanglef& bounds = GetBounds();
            return Max(bounds.W, bounds.H) * 0.3f;
        }

        const Character::Damage& BasicWeapon::GetDPS() const
        {
            return _dps;
        }

        const BlockMap& BasicWeapon::GetBlockChances() const
        {
            return _blocks;
        }

        const Rectanglef& BasicWeapon::GetBounds() const
        {
            if (_damageJoint.length() > 0)
            {
                return _skeleton->GetJointBounds(_damageJoint).Bounds();
            }
            else
            {
                return _skeleton->GetDrawBounds();
            }
        }

        Physics::Collision* BasicWeapon::GetCollision() const
        {
            return _collision;
        }

        float BasicWeapon::GetArmorValue() const
        {
            return _armor;
        }

        Character::Damage BasicWeapon::OnPreRecieveDamage(Character::Character* source, const Vector2f& position, const Character::Damage& inputDamage)
        {
            Character::Damage modifiedDamage = Weapon::OnPreRecieveDamage(source, position, inputDamage);

            for (const auto& block : _blocks)
            {
                if ((modifiedDamage.Type & block.first) != 0)
                {
                    if (Random::RandomBetween(0.0f, 1.0f) < block.second.Chance)
                    {
                        modifiedDamage.Amount = Max(modifiedDamage.Amount - block.second.Ammount, 0.0f);
                    }
                }
            }

            return modifiedDamage;
        }

        const HUD::PanelDrawable* BasicWeapon::GetIcon() const
        {
            return _icon;
        }

        const HUD::Panel* BasicWeapon::GetTooltip() const
        {
            return _tooltip;
        }

        const Audio::Sound* BasicWeapon::GetSelectionSound() const
        {
            return _selectionSounds.GetNextSound();
        }

        Animation::SkeletonInstance* BasicWeapon::GetSkeleton() const
        {
            return _skeleton;
        }

        void BasicWeapon::OnLoadContent(Content::ContentManager* contentManager)
        {
            auto skeleton = Content::CreateSkeletonInstance(contentManager, _skeletonPath, _matsetPath);
            SetSkeleton(skeleton);
            SafeRelease(skeleton);

            const Localization::StringTable* strings = GetLevel()->GetStringTable();

            _icon = HUD::CreateItemIcon(contentManager, _iconMatsetPath, _iconMaterialName, GetSkeleton());
            _tooltip = HUD::CreateWeaponTooltipPanel(contentManager, strings, this, _description, _flavor);

            _selectionSounds.LoadContent(contentManager);

            _physicsMaterialSounds.LoadContent(contentManager);
        }

        void BasicWeapon::OnUnloadContent()
        {
            SafeRelease(_icon);
            SafeRelease(_tooltip);
            SafeRelease(_skeleton);
            SafeRelease(_collision);

            _brokenPieces.clear();

            _selectionSounds.UnloadContent();

            _physicsMaterialSounds.UnloadContent();
        }

        void BasicWeapon::OnPostInteractedWith(Character::Character* interactor)
        {
            Weapon::OnPostInteractedWith(interactor);

            auto soundManager = GetLevel()->GetSoundManager();
            soundManager->PlaySinglePositionalSound(_selectionSounds.GetNextSound(), Audio::SoundPriority::High, GetPosition(), 
                                                    _pickupSoundRadius.first, _pickupSoundRadius.second, _pickupSoundVolume);
        }

        void BasicWeapon::OnUpdate(double totalTime, float dt)
        {
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
                    float velocityIntoSurface = Vector2f::Dot(contact.Velocity, -contact.Normal);
                    static const float VelocityThreshold = 200.0f;
                    if (velocityIntoSurface >= VelocityThreshold)
                    {
                        static const float MaxVolumeVelocity = 3000.0f;
                        float volume = Saturate((velocityIntoSurface - VelocityThreshold) / MaxVolumeVelocity);

                        auto soundManager = GetLevel()->GetSoundManager();
                        soundManager->PlaySinglePositionalSound(_physicsMaterialSounds.GetNextSound(), Audio::SoundPriority::Low,
                            contact.Position, PhysicsCollisionSoundRadius.first, PhysicsCollisionSoundRadius.second,
                            PhysicsCollisionSoundVolume * volume);

                        static const float ResetTime = 0.1f;
                        _physicsSoundResetTimer = ResetTime;
                        break;
                    }
                }
            };

            _skeleton->Update(totalTime, dt);
            checkForPhysicsMaterialSounds(_collision);
            for (auto& brokenMat : _brokenPieces)
            {
                brokenMat.skeleton->Update(totalTime, dt);
                brokenMat.collision->Update(totalTime, dt);
                checkForPhysicsMaterialSounds(brokenMat.collision);
            }

            Character::Character* owner = GetOwner();
            if (owner)
            {
                Rayf ownerAttachRay = owner->GetWeaponAttachPoint(GetAttachPoint(), GetWeaponType());
                Animation::AttachmentInfo attachInfo =
                    Animation::GetSkeletonAttachmentInfo(Animation::IsCharacterInvertedX(owner), Animation::IsCharacterInvertedY(owner), true, ownerAttachRay,
                                                         _skeleton, _weaponAttachPointA, _weaponAttachPointB);

                _skeleton->SetInvertedX(attachInfo.InvertX);
                _skeleton->SetInvertedY(attachInfo.InvertY);
                SetPosition(attachInfo.Position);
                SetScale(attachInfo.Scale);
                SetRotation(attachInfo.Rotation);
            }
            else
            {
                SetPosition(_collision->GetPosition());
                SetRotation(_collision->GetRotation());
            }

            _collision->Update(totalTime, dt);
        }

        void BasicWeapon::SetSkeleton(Animation::SkeletonInstance* skeleton)
        {
            assert(skeleton);

            Character::Character* owner = GetOwner();
            if (owner && _skeleton)
            {
                owner->DetachDrawable(GetAttachPoint(), _skeleton);
            }
            SafeRelease(_skeleton);
            SafeRelease(_collision);

            _skeleton = skeleton;
            SafeAddRef(_skeleton);

            _skeleton->SetPosition(GetPosition());
            _skeleton->SetScale(GetScale());
            _skeleton->SetRotation(GetRotation());
            _skeleton->SetColor(GetColor());
            if (owner)
            {
                owner->AttachDrawable(GetAttachPoint(), _skeleton);
            }

            _collision = new Physics::SkeletonCollision(GetLevelLayer()->GetPhysicsWorld(), _skeleton);
        }

        void BasicWeapon::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            Character::Character* owner = GetOwner();
            if (owner)
            {
                // skeleton is drawn by attaching it to its parent
            }
            else
            {
                levelRenderer->AddDrawable(_skeleton, false, true, GetMinimumColor(), GetEmissiveColor(), GetHighlightColor());
            }
        }

        void BasicWeapon::OnAttachPointChanged(const std::string& oldAttachPoint, const std::string& newAttachPoint)
        {
            Character::Character* owner = GetOwner();
            if (owner && _skeleton)
            {
                owner->DetachDrawable(oldAttachPoint, _skeleton);
                owner->AttachDrawable(newAttachPoint, _skeleton);
            }
        }
    }

    template <>
    void EnumeratePreloads<Item::BasicWeapon>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::Weapon>(preloads);
        EnumerateTooltipPreloads(preloads);
    }
}
