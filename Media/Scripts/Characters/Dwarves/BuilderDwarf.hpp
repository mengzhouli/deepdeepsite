#pragma once

#include "Characters/Dwarves/Dwarf.hpp"

namespace Dwarf
{
    namespace Character
    {
        class BuilderDwarf : public Dwarf, public Builder
        {
        public:
            BuilderDwarf(const CharacterParameters& parameters);
            BuilderDwarf(const CharacterParameters& parameters, const DwarfInfo& info);

            // Builder interface
            virtual void PlayBuildAnimation() override;

        protected:
            float OnPreInteractWithCharacter(Character* target) override;

            void OnLoadContent(Content::ContentManager* contentManager) override;
            void OnUnloadContent() override;

            void OnUpdate(double totalTime, float dt) override;

            bool IsBuildingBridge() const;

        private:
            float getBuildRateMultplier() const;

            CharacterID _bridgeTarget = 0;

            ResourcePointer<const Graphics::Font> _goldFont;
        };
    }

    template <>
    void EnumeratePreloads<Character::BuilderDwarf>(PreloadSet& preloads);
}
