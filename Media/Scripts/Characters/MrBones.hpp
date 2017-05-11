#pragma once

#include "Characters/BasicCharacter.hpp"

namespace Dwarf
{
    namespace Character
    {
        class MrBones : public BasicCharacter
        {
        public:
            MrBones(const CharacterParameters& parameters);

            virtual void OnStateChange(CharacterState newState) override;
        };

        class MrBonesMelee : public MrBones
        {
        public:
            MrBonesMelee(const CharacterParameters& parameters);
        };

        class MrBonesArcher : public MrBones
        {
        public:
            MrBonesArcher(const CharacterParameters& parameters);
        };
    }

    template <>
    void EnumeratePreloads<Character::MrBones>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Character::MrBonesMelee>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Character::MrBonesArcher>(PreloadSet& preloads);
}
