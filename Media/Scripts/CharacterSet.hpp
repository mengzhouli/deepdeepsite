#pragma once

#include "Character/Character.hpp"

#include <cstdint>
#include <map>
#include <vector>
#include <typeindex>

namespace Dwarf
{
    class CharacterSet
    {
    public:
        CharacterSet()
            : _totalCount(0)
        {
        }

        void AddCharacter(Character::Character* character)
        {
            if (!character)
            {
                return;
            }

            uint32_t index = 0;

            IndexMap::const_iterator iter = _indices.find(typeid(*character));
            if (iter != _indices.end())
            {
                index = iter->second;
            }
            else
            {
                index = _characters.size();
                _indices[typeid(*character)] = index;
                _characters.push_back(CharacterVector());
            }

            CharacterVector& charVector = _characters[index];
            if (std::find(charVector.begin(), charVector.end(), character) == charVector.end())
            {
                charVector.push_back(character);
                _totalCount++;
            }
        }

        void RemoveCharacter(Character::Character* character)
        {
            if (!character)
            {
                return;
            }

            IndexMap::const_iterator indexIter = _indices.find(typeid(*character));
            if (indexIter == _indices.end())
            {
                return;
            }

            CharacterVector& charVector = _characters[indexIter->second];
            CharacterVector::iterator charIter = std::find(charVector.begin(), charVector.end(), character);
            if (charIter != charVector.end())
            {
                charVector.erase(charIter);
                _totalCount--;
            }

            if (charVector.empty())
            {
                for (auto& index : _indices)
                {
                    if (index.second > indexIter->second)
                    {
                        index.second--;
                    }
                }

                _characters.erase(_characters.begin() + indexIter->second);
                _indices.erase(indexIter);
            }
        }

        void Clear()
        {
            _characters.clear();
            _indices.clear();
            _totalCount = 0;
        }

        bool ContainsCharacter(const Character::Character* character) const
        {
            if (!character)
            {
                return false;
            }

            IndexMap::const_iterator indexIter = _indices.find(typeid(*character));
            if (indexIter == _indices.end())
            {
                return false;
            }

            const CharacterVector& charVector = _characters[indexIter->second];
            return std::find(charVector.begin(), charVector.end(), character) != charVector.end();
        }

        uint32_t Count(const std::type_index& type) const
        {
            IndexMap::const_iterator indexIter = _indices.find(type);
            if (indexIter == _indices.end())
            {
                return 0;
            }

            return _characters[indexIter->second].size();
        }

        uint32_t Count(uint32_t typeIdx) const
        {
            if (typeIdx >= _characters.size())
            {
                return 0;
            }

            return _characters[typeIdx].size();
        }

        uint32_t Count() const
        {
            return _totalCount;
        }

        Character::Character* GetCharacter(uint32_t idx) const
        {
            if (idx >= _totalCount)
            {
                return NULL;
            }

            uint32_t count = 0;
            for (uint32_t i = 0; i < _characters.size(); i++)
            {
                if (idx - count < _characters[i].size())
                {
                    return _characters[i][idx - count];
                }
                else
                {
                    count += _characters[i].size();
                }
            }

            assert(0);
            return NULL;
        }

        Character::Character* GetCharacter(const std::type_index& type, uint32_t idx) const
        {
            IndexMap::const_iterator indexIter = _indices.find(type);
            if (indexIter == _indices.end())
            {
                return NULL;
            }

            if (idx < _characters[indexIter->second].size())
            {
                return NULL;
            }

            return _characters[indexIter->second][idx];
        }

        Character::Character* GetCharacter(uint32_t typeIdx, uint32_t idx) const
        {
            if (typeIdx >= _characters.size())
            {
                return NULL;
            }

            if (idx >= _characters[typeIdx].size())
            {
                return NULL;
            }

            return _characters[typeIdx][idx];
        }

        uint32_t GetTypeCount() const
        {
            return _characters.size();
        }

        const std::type_index& GetType(uint32_t typeIdx) const
        {
            if (typeIdx >= _characters.size())
            {
                throw Exception("Type index not in range.");
            }

            for (IndexMap::const_iterator i = _indices.begin(); i != _indices.end(); i++)
            {
                if (typeIdx == i->second)
                {
                    return i->first;
                }
            }

            assert(0);
            static const std::type_index defaultTypeIndex = typeid(int);
            return defaultTypeIndex;
        }

    private:
        uint32_t _totalCount;

        typedef std::vector<Character::Character*> CharacterVector;
        std::vector<CharacterVector> _characters;

        typedef std::map<std::type_index, uint32_t> IndexMap;
        IndexMap _indices;
    };
}
