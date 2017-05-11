#pragma once

#include "Character/Character.hpp"
#include "Animation/SkeletonInstance.hpp"
#include "Graphics/PolygonMaterial.hpp"
#include "Level/LevelInstance.hpp"

#include "MaterialSelector.hpp"
#include "SkeletonUtility.hpp"

#include "SoundSet.hpp"

namespace Dwarf
{
    namespace Character
    {
        class SkeletonCharacter : public Character
        {
        public:
            SkeletonCharacter(const CharacterParameters& parameters, const std::string& skeletonPath, const std::string& matsetPath);

            virtual const HUD::PanelDrawable* GetIcon() const override;
            virtual const HUD::Panel* GetTooltip() const override;

            const Rectanglef& GetBounds() const override;
            Physics::Collision* GetCollision() const override;

            virtual Rayf GetAttachPoint(const std::string& name) const override;
            virtual Rayf GetWeaponAttachPoint(const std::string& name, Item::WeaponType type) const override;

            bool IsAnimationFinished() const;
            bool HasAnimationTagPassed(const std::string& tag) const;
            bool HasAnimationTagJustPassed(const std::string& tag) const;
            float GetAnimationLength() const;
            float GetAnimationLength(const std::string& animationName) const;

            void ApplyMaterialSet(const std::string& group, const Graphics::PolygonMaterialSet* matset);
            void RemoveMaterialSet(const std::string& group);
            void AddMaterial(const std::string& group, const std::string& materialPath, float probability);
            void ClearMaterialGroup(const std::string& group);

            float PlayAnimation(const std::string& anim, bool loop, float transitionTime, float startOffset);
            float PlayAnimationSet(const Animation::AnimationSet& animSet, bool loop, float transitionTime, float startOffset);
            bool IsPlayingAnimation() const;
            bool IsPlayingAnimation(const std::string& anim) const;
            bool IsPlayingAnimation(const Animation::AnimationSet& animSet) const;
            const std::string& GetCurrentAnimation() const;

            void BreakMaterial(const std::string& material);

            virtual void AttachDrawable(const std::string& name, const Graphics::IDrawable* drawable) override;
            virtual void DetachDrawable(const std::string& name, const Graphics::IDrawable* drawable) override;

            virtual void OnLoadContent(Content::ContentManager* contentManager) override;
            virtual void OnUnloadContent() override;

            virtual void OnUpdate(double totalTime, float dt) override;

            virtual void OnDraw(Graphics::LevelRenderer* levelRenderer) const override;

            Animation::SkeletonInstance* GetSkeleton() const;

        protected:
            void SetIcon(const std::string& iconMatsetPath, const std::string& iconMaterialName);
            void SetDeadIcon(const std::string& iconMatsetPath, const std::string& iconMaterialName);
            void SetTooltip(const std::string codeName, ...);

            void SetDrawSkeleton(bool drawSkeleton);

            void SetSkeletonCastsShadows(bool castsShadows);
            bool GetSkeletonCastsShadows() const;

            void SetPlayFasterAnimationsForMoving(bool fastAnimations);
            void SetPlayFasterAnimationsForAttacking(bool fastAnimations);

            void SetSkeletonScale(float scale);
            void SetSkeletonColor(const Color& color);
            void SetSkeletonInvertedX(bool invertedX);
            void SetSkeletonInvertedY(bool invertedY);

            void SetSkeletonJointStrength(float strength);

            void SetSkeletonRotatesWithDirection(bool rotates);
            void SetSkeletonInvertsWithDirection(bool inverts);

            void SetMaterialCollisionSound(const Audio::SoundPathVector& sounds);

            void InsertBrokenMaterial(const Animation::BrokenMaterial material);

            void AddCustomAttachPoint(const std::string& name, const std::string& jointA, const std::string&jointB);

            virtual Physics::Collision* CreateCollision();

            virtual void OnSpawn() override;

            virtual void OnPositionChange(const Vector2f& oldPos, const Vector2f& newPos) override;
            virtual void OnScaleChange(float oldScale, float newScale) override;
            virtual void OnRotationChange(const Rotatorf& oldRot, const Rotatorf& newRot) override;
            virtual void OnColorChange(const Color& oldCol, const Color& newCol) override;

        private:
            std::string getAttachJoint(const std::string& attachName) const;

            bool _drawSkeleton;
            float _skeletonScale;
            Color _skeletonColor;
            bool _skeletonCastsShadows;
            bool _skeletonHiddenByFog;
            bool _skeletonRotates;
            bool _skeletonInverts;
            bool _skeletonInvertedX = false;
            bool _skeletonInvertedY = false;

            std::string _skeletonPath;
            std::string _matsetPath;
            Animation::SkeletonInstance* _skeleton;

            std::unordered_map<std::string, std::pair<std::string, std::string>> _customAttachPoints;

            float _skeletonJointStrength;
            Physics::Collision* _collision;

            typedef std::map<std::string, MaterialSelector> MaterialOptionsMap;
            MaterialOptionsMap _materialOptions;

            typedef std::map<std::string, const Graphics::PolygonMaterialSet*> ChosenMaterialMap;
            ChosenMaterialMap _chosenMaterials;

            bool _hasPendingAnimation;
            struct pendingAnimation
            {
                Animation::AnimationSet animationSet;
                bool loop;
                float transitionTime;
                float startOffset;
            };
            pendingAnimation _pendingAnimation;

            std::string _aliveIconMatsetPath;
            std::string _aliveIconMaterialName;
            const HUD::PanelDrawable* _aliveIcon;

            std::string _deadIconMatsetPath;
            std::string _deadIconMaterialName;
            const HUD::PanelDrawable* _deadIcon;

            std::string _description;
            const HUD::Panel* _tooltip;

            std::vector<Animation::BrokenMaterial> _brokenPieces;

            float _physicsSoundResetTimer = 0.0f;
            Audio::SoundSet _physicsMaterialSounds;

            bool _playFastAnimationsForMoving = true;
            bool _playFastAnimationsForAttacking = true;
        };
    }

    template <>
    void EnumeratePreloads<Character::SkeletonCharacter>(PreloadSet& preloads);
}
