#include "Controllers/WildlifeController.hpp"

namespace Dwarf
{
    namespace Character
    {
        WildlifeController::WildlifeController(const ControllerParameters& parameters)
            : Controller(parameters)
        {
        }
    }

    template <>
    void EnumeratePreloads<Character::WildlifeController>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Controller>(preloads);
    }
}
