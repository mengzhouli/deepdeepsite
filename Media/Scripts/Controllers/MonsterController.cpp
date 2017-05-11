#include "Controllers/MonsterController.hpp"
#include "Characters/BasicCharacter.hpp"

#include "MathUtility.hpp"

namespace Dwarf
{
    namespace Character
    {
        MonsterController::MonsterController(const ControllerParameters& parameters)
            : Controller(parameters)
        {
        }

        void MonsterController::OnUpdate(const Input::FrameInput& input, double totalTime, float dt)
        {
        }

        void MonsterController::OnRemoveCharacter(Character* character)
        {
            _allCharacters.RemoveCharacter(character);
        }

        void MonsterController::OnAddCharacter(Character* character)
        {
            _allCharacters.AddCharacter(character);
        }
    }

    template <>
    void EnumeratePreloads<Character::MonsterController>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Controller>(preloads);
    }
}
