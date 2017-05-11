#include "Characters/Dwarves/CookDwarf.hpp"

namespace Dwarf
{
    namespace Character
    {
        static const std::string CookMatsetPath = "Skeletons/Characters/Dwarf2/cook.polymatset";
        static const std::string CookIconMatsetPath = "HUD/hud.hudmatset";

        CookDwarf::CookDwarf(const CharacterParameters& parameters)
            : CookDwarf(parameters, CreateDefaultDwarfInfo(parameters))
        {
        }

        CookDwarf::CookDwarf(const CharacterParameters& parameters, const DwarfInfo& info)
            : Dwarf(parameters, info, CookMatsetPath)
        {
            AddTerrainAnimation(AnimationType_Move, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, ForwardRunAnimation, 1.0f);
            SetIcon(CookIconMatsetPath, "class_cook");
        }
    }

    template <>
    void EnumeratePreloads<Character::CookDwarf>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Dwarf>(preloads);
        preloads.insert(Character::CookMatsetPath);
        preloads.insert(Character::CookIconMatsetPath);
    }
}
