#include "Items/Trinkets/BasicTrinket.hpp"

#include "HUD/Tooltip.hpp"

#include "ContentUtility.hpp"
#include "SkeletonUtility.hpp"

namespace Dwarf
{
    namespace Item
    {
        static std::string GetTrinketName(const ItemParameters& parameters, const std::string& nameCode)
        {
            assert(parameters.Layer);
            return parameters.Layer->GetLevel()->GetStringTable()->GetString(nameCode);
        }

        BasicTrinket::BasicTrinket(const ItemParameters& parameters, const std::string& nameCode, const std::string& skeletonPath, const std::string& skeletonMatsetPath)
            : Trinket(parameters, GetTrinketName(parameters, nameCode))

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
            , _ownerAttachPt()
            , _skeletonAttachA()
            , _skeletonAttachB()

            , _pickupSoundRadius(2000.0f, 3000.0f)
            , _pickupSoundVolume(1.0f)
        {
            SetDestructionFadeTimers(3.0f, 4.0f);
        }

        BasicTrinket::~BasicTrinket()
        {
        }

        const Rectanglef& BasicTrinket::GetBounds() const
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
                return Trinket::GetBounds();
            }
        }

        Physics::Collision* BasicTrinket::GetCollision() const
        {
            return _collision;
        }

        const HUD::PanelDrawable* BasicTrinket::GetIcon() const
        {
            return _icon;
        }

        const HUD::Panel* BasicTrinket::GetTooltip() const
        {
            return _tooltip;
        }

        const Audio::Sound* BasicTrinket::GetSelectionSound() const
        {
            return _selectionSounds.GetNextSound();
        }

        Animation::SkeletonInstance* BasicTrinket::GetSkeleton() const
        {
            return _skeleton;
        }

        void BasicTrinket::SetIcon(const std::string& iconMatsetPath, const std::string& iconMaterialName)
        {
            _iconMatsetPath = iconMatsetPath;
            _iconMaterialName = iconMaterialName;
        }

        void BasicTrinket::SetTooltipDescription(const std::string codeName, ...)
        {
            va_list args;
            va_start(args, codeName);
            _description = Format(GetLevel()->GetStringTable()->GetString(codeName).c_str(), args);
            va_end(args);
        }

        void BasicTrinket::SetTooltipFlavor(const std::string& codeName)
        {
            _flavor = GetLevel()->GetStringTable()->GetString(codeName);
        }

        void BasicTrinket::AddSelectionSounds(const Audio::SoundPathVector& sounds)
        {
            // Keep a consistent sound for each individual item type
            _selectionSounds.Clear();
            _selectionSounds.AddSound(Random::SeededRandomItem(sounds, GetName()));
        }

        void BasicTrinket::SetOwnerSkeletonAttachPoint(const std::string& ownerAttachPt, const std::string& localAttachA, const std::string& localAttachB)
        {
            _ownerAttachPt = ownerAttachPt;
            _skeletonAttachA = localAttachA;
            _skeletonAttachB = localAttachB;

            Character::Character* owner = GetOwner();
            if (owner != nullptr && _skeleton != nullptr && !_ownerAttachPt.empty())
            {
                owner->AttachDrawable(_ownerAttachPt, _skeleton);
            }
        }

        void BasicTrinket::OnEquip(Character::Character* newOwner)
        {
            if (_skeleton != nullptr && !_ownerAttachPt.empty())
            {
                newOwner->AttachDrawable(_ownerAttachPt, _skeleton);
            }
        }

        void BasicTrinket::OnUnEquip(Character::Character* oldOwner)
        {
            if (_skeleton != nullptr && !_ownerAttachPt.empty())
            {
                oldOwner->DetachDrawable(_ownerAttachPt, _skeleton);
            }
        }

        void BasicTrinket::OnPositionChange(const Vector2f& oldPos, const Vector2f& newPos)
        {
            if (_skeleton)
            {
                _skeleton->SetPosition(newPos);
            }
        }

        void BasicTrinket::OnScaleChange(float oldScale, float newScale)
        {
            if (_skeleton)
            {
                _skeleton->SetScale(newScale);
            }
        }

        void BasicTrinket::OnRotationChange(const Rotatorf& oldRot, const Rotatorf& newRot)
        {
            if (_skeleton)
            {
                _skeleton->SetRotation(newRot);
            }
        }

        void BasicTrinket::OnColorChange(const Color& oldCol, const Color& newCol)
        {
            if (_skeleton)
            {
                _skeleton->SetColor(newCol);
            }
        }

        void BasicTrinket::OnLoadContent(Content::ContentManager* contentManager)
        {
            Trinket::OnLoadContent(contentManager);

            if (!_skeletonPath.empty() && !_skeletonMatsetPath.empty())
            {
                _skeleton = Content::CreateSkeletonInstance(contentManager, _skeletonPath, _skeletonMatsetPath);
                _collision = new Physics::SkeletonCollision(GetLevelLayer()->GetPhysicsWorld(), _skeleton);
            }

            const Localization::StringTable* strings = GetLevel()->GetStringTable();
            _icon = HUD::CreateItemIcon(contentManager, _iconMatsetPath, _iconMaterialName, nullptr);
            _tooltip = HUD::CreateTrinketTooltipPanel(contentManager, strings, this, _description, _flavor);

            _selectionSounds.LoadContent(contentManager);
        }

        void BasicTrinket::OnUnloadContent()
        {
            Trinket::OnUnloadContent();

            SafeRelease(_icon);
            SafeRelease(_tooltip);
            SafeRelease(_skeleton);
            SafeRelease(_collision);

            _selectionSounds.UnloadContent();
        }

        void BasicTrinket::OnPostInteractedWith(Character::Character* interactor)
        {
            Trinket::OnPostInteractedWith(interactor);

            auto soundManager = GetLevel()->GetSoundManager();
            soundManager->PlaySinglePositionalSound(_selectionSounds.GetNextSound(), Audio::SoundPriority::High, GetPosition(), 
                                                    _pickupSoundRadius.first, _pickupSoundRadius.second, _pickupSoundVolume);
        }

        void BasicTrinket::OnUpdate(double totalTime, float dt)
        {
            Trinket::OnUpdate(totalTime, dt);

            if (_skeleton)
            {
                assert(_collision);
                Character::Character* owner = GetOwner();
                if (owner != nullptr)
                {
                    if (!_ownerAttachPt.empty())
                    {
                        Rayf ownerAttachRay = owner->GetAttachPoint(_ownerAttachPt);
                        Animation::AttachmentInfo attachInfo =
                            Animation::GetSkeletonAttachmentInfo(Animation::IsCharacterInvertedX(owner), Animation::IsCharacterInvertedY(owner), true, ownerAttachRay,
                                                                 _skeleton, _skeletonAttachA, _skeletonAttachB);

                        _skeleton->SetInvertedX(attachInfo.InvertX);
                        _skeleton->SetInvertedY(attachInfo.InvertY);
                        SetPosition(attachInfo.Position);
                        SetScale(attachInfo.Scale);
                        SetRotation(attachInfo.Rotation);
                    }
                    else
                    {
                        SetPosition(owner->GetPosition());
                    }
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

        void BasicTrinket::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            Trinket::OnDraw(levelRenderer);

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
    void EnumeratePreloads<Item::BasicTrinket>(PreloadSet& preloads)
    {
        EnumerateTooltipPreloads(preloads);
    }
}
