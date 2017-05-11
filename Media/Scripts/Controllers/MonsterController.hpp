#pragma once

#include "Character/Controller.hpp"
#include "Level/LevelInstance.hpp"
#include "Level/LevelLayerInstance.hpp"
#include "Content/Preload.hpp"

#include "CharacterSet.hpp"

namespace Dwarf
{
    namespace Character
    {
        class MonsterController : public Controller
        {
        public:
            MonsterController(const ControllerParameters& parameters);

            void OnAddCharacter(Character* character) override;
            void OnRemoveCharacter(Character* character) override;

            void OnUpdate(const Input::FrameInput& input, double totalTime, float dt) override;

        private:
            CharacterSet _allCharacters;
        };
    }

    template <>
    void EnumeratePreloads<Character::MonsterController>(PreloadSet& preloads);
}
