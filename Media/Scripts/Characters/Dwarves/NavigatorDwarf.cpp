#include "Characters/Dwarves/NavigatorDwarf.hpp"

namespace Dwarf
{
    namespace Character
    {
        static const std::string NavigatorMatsetPath = "Skeletons/Characters/Dwarf2/navigator.polymatset";
        static const std::string NavigatorIconMatsetPath = "HUD/hud.hudmatset";

        NavigatorDwarf::NavigatorDwarf(const CharacterParameters& parameters)
            : NavigatorDwarf(parameters, CreateDefaultDwarfInfo(parameters))
        {
        }

        NavigatorDwarf::NavigatorDwarf(const CharacterParameters& parameters, const DwarfInfo& info)
            : Dwarf(parameters, info, NavigatorMatsetPath)
        {
            AddTerrainAnimation(AnimationType_Move, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, NormalRunAnimation, 1.0f);
            SetIcon(NavigatorIconMatsetPath, "class_navigator");
        }
    }

    template <>
    void EnumeratePreloads<Character::NavigatorDwarf>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Dwarf>(preloads);
        preloads.insert(Character::NavigatorMatsetPath);
        preloads.insert(Character::NavigatorIconMatsetPath);
    }
}
