#pragma once

#include <map>
#include <vector>
#include <string>

namespace Dwarf
{
    namespace Character
    {
        std::string GenerateDwarfFirstName();
        std::string GenerateDwarfTitle(const std::string& type);
        std::string GenerateDwarfFullName(const std::string& type);
    }
}
