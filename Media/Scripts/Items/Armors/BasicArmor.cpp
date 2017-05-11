#include "Items/Armors/BasicArmor.hpp"

#include "Level/LevelInstance.hpp"

#include "Characters/SkeletonCharacter.hpp"
#include "HUD/Tooltip.hpp"

#include "ContentUtility.hpp"

namespace Dwarf
{
    namespace Item
    {
        static const std::string ArmorGroupName = "basic_armor";

        static std::string GetArmorName(const ItemParameters& parameters, const std::string& nameCode)
        {
            assert(parameters.Layer);
            return parameters.Layer->GetLevel()->GetStringTable()->GetString(nameCode);
        }

        BasicArmor::BasicArmor(const ItemParameters& parameters, const std::string& nameCode, const std::string& matsetPath,
                               const std::string& skeletonPath, const std::string& skeletonMatsetPath)
            : Armor(parameters, GetArmorName(parameters, nameCode))

            , _armorValue(0.0f)

            , _material(0)

            , _iconMatsetPath()
            , _iconMaterialName()

            , _description()
            , _flavor()
            , _icon(nullptr)
            , _tooltip(nullptr)

            , _skeletonPath(skeletonPath)
            , _skeletonMatsetPath(skeletonMatsetPath)
            , _skeleton(nullptr)
            , _collision(nullptr)

            , _pickupSoundRadius(2000.0f, 3000.0f)
            , _pickupSoundVolume(1.0f)

            , _matsetPath(matsetPath)
            , _matset(nullptr)
        {
            SetDestructionFadeTimers(3.0f, 4.0f);
        }

        BasicArmor::~BasicArmor()
        {
        }

        void BasicArmor::SetArmorValue(float armor)
        {
            _armorValue = armor;
        }

        void BasicArmor::SetMaterial(Character::MaterialType material)
        {
            _material = material;
        }

        float BasicArmor::GetArmorValue() const
        {
            return _armorValue;
        }

        Character::MaterialType BasicArmor::GetMaterial() const
        {
            return _material;
        }

        const Rectanglef& BasicArmor::GetBounds() const
        {
            if (HasOwner())
            {
                return GetOwner()->GetBounds();
            }
            else if (_skeleton)
            {
                return _skeleton->GetDrawBounds();
            }
            else
            {
                return Armor::GetBounds();
            }
        }

        Physics::Collision* BasicArmor::GetCollision() const
        {
            return _collision;
        }

        const HUD::PanelDrawable* BasicArmor::GetIcon() const
        {
            return _icon;
        }

        const HUD::Panel* BasicArmor::GetTooltip() const
        {
            return _tooltip;
        }

        const Dwarf::Audio::Sound* BasicArmor::GetSelectionSound() const
        {
            return _selectionSounds.GetNextSound();
        }

        Animation::SkeletonInstance* BasicArmor::GetSkeleton() const
        {
            return _skeleton;
        }

        void BasicArmor::SetIcon(const std::string& iconMatsetPath, const std::string& iconMaterialName)
        {
            _iconMatsetPath = iconMatsetPath;
            _iconMaterialName = iconMaterialName;
        }

        void BasicArmor::SetTooltipDescription(const std::string codeName, ...)
        {
            va_list args;
            va_start(args, codeName);
            _description = Format(GetLevel()->GetStringTable()->GetString(codeName).c_str(), args);
            va_end(args);
        }

        void BasicArmor::SetTooltipFlavor(const std::string& codeName)
        {
            _flavor = GetLevel()->GetStringTable()->GetString(codeName);
        }

        void BasicArmor::AddSelectionSounds(const Audio::SoundPathVector& sounds)
        {
            // Keep a consistant sound for each individual item type
            _selectionSounds.Clear();
            _selectionSounds.AddSound(Random::SeededRandomItem(sounds, GetName()));
        }

        void BasicArmor::OnEquip(Character::Character* newOwner)
        {
            if (newOwner && _matset && IsA<Character::SkeletonCharacter>(newOwner))
            {
                AsA<Character::SkeletonCharacter>(newOwner)->ApplyMaterialSet(ArmorGroupName, _matset);
            }
        }

        void BasicArmor::OnUnEquip(Character::Character* oldOwner)
        {
            if (oldOwner && IsA<Character::SkeletonCharacter>(oldOwner))
            {
                AsA<Character::SkeletonCharacter>(oldOwner)->RemoveMaterialSet(ArmorGroupName);
            }
        }

        void BasicArmor::OnPositionChange(const Vector2f& oldPos, const Vector2f& newPos)
        {
            if (_skeleton)
            {
                _skeleton->SetPosition(newPos);
            }
        }

        void BasicArmor::OnScaleChange(float oldScale, float newScale)
        {
            if (_skeleton)
            {
                _skeleton->SetScale(newScale);
            }
        }

        void BasicArmor::OnRotationChange(const Rotatorf& oldRot, const Rotatorf& newRot)
        {
            if (_skeleton)
            {
                _skeleton->SetRotation(newRot);
            }
        }

        void BasicArmor::OnColorChange(const Color& oldCol, const Color& newCol)
        {
            if (_skeleton)
            {
                _skeleton->SetColor(newCol);
            }
        }

        void BasicArmor::OnPostInteractedWith(Character::Character* interactor)
        {
            Armor::OnPostInteractedWith(interactor);

            auto soundManager = GetLevel()->GetSoundManager();
            soundManager->PlaySinglePositionalSound(_selectionSounds.GetNextSound(), Audio::SoundPriority::High, GetPosition(), 
                                                    _pickupSoundRadius.first, _pickupSoundRadius.second, _pickupSoundVolume);
        }

        void BasicArmor::OnLoadContent(Content::ContentManager* contentManager)
        {
            _matset = contentManager->Load<Graphics::PolygonMaterialSet>(_matsetPath);

            if (!_skeletonPath.empty() && !_skeletonMatsetPath.empty())
            {
                _skeleton = Content::CreateSkeletonInstance(contentManager, _skeletonPath, _skeletonMatsetPath);
                _collision = new Physics::SkeletonCollision(GetLevelLayer()->GetPhysicsWorld(), _skeleton);
            }

            OnEquip(GetOwner());

            const Localization::StringTable* strings = GetLevel()->GetStringTable();
            _icon = HUD::CreateItemIcon(contentManager, _iconMatsetPath, _iconMaterialName, nullptr);
            _tooltip = HUD::CreateArmorTooltipPanel(contentManager, strings, this, _description, _flavor);

            _selectionSounds.LoadContent(contentManager);
        }

        void BasicArmor::OnUnloadContent()
        {
            OnUnEquip(GetOwner());

            SafeRelease(_matset);
            SafeRelease(_skeleton);
            SafeRelease(_collision);
            SafeRelease(_icon);
            SafeRelease(_tooltip);

            _selectionSounds.UnloadContent();
        }

        void BasicArmor::OnUpdate(double totalTime, float dt)
        {
            if (_skeleton)
            {
                assert(_collision);
                if (HasOwner())
                {
                    SetPosition(GetOwner()->GetPosition());
                }
                else
                {
                    SetPosition(_collision->GetPosition());
                    SetRotation(_collision->GetRotation());
                }

                _skeleton->Update(totalTime, dt);
                _collision->Update(totalTime, dt);
            }
        }

        void BasicArmor::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            if (HasOwner())
            {
                // Don't draw the skeleton when attached to an owner
            }
            else
            {
                if (_skeleton)
                {
                    levelRenderer->AddDrawable(_skeleton, false, true, GetMinimumColor(), GetEmissiveColor(), GetHighlightColor());
                }
            }
        }
    }

    template <>
    void EnumeratePreloads<Item::BasicArmor>(PreloadSet& preloads)
    {
        EnumerateTooltipPreloads(preloads);
    }
}
