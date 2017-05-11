#include "Characters/Dwarves/BrewerDwarf.hpp"

namespace Dwarf
{
    namespace Character
    {
        static const std::string BrewerMatsetPath = "Skeletons/Characters/Dwarf2/brewer.polymatset";
        static const std::string BrewerIconMatsetPath = "HUD/hud.hudmatset";

        BrewerDwarf::BrewerDwarf(const CharacterParameters& parameters)
            : BrewerDwarf(parameters, CreateDefaultDwarfInfo(parameters))
        {
        }

        BrewerDwarf::BrewerDwarf(const CharacterParameters& parameters, const DwarfInfo& info)
            : Dwarf(parameters, info, BrewerMatsetPath)
        {
            AddTerrainAnimation(AnimationType_Move, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, NormalRunAnimation, 1.0f);
            SetIcon(BrewerIconMatsetPath, "class_brewer");
        }
    }

    template <>
    void EnumeratePreloads<Character::BrewerDwarf>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Dwarf>(preloads);
        preloads.insert(Character::BrewerMatsetPath);
        preloads.insert(Character::BrewerIconMatsetPath);
    }
}
