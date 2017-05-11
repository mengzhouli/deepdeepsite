#pragma once

#include "Random.hpp"

#include <vector>
#include <string>
#include <cstdint>

namespace Dwarf
{
    class MaterialSelector
    {
    public:
        void Add(const std::string& materialName, float weight)
        {
            if (weight < 0.0f)
            {
                weight = 0.0f;
            }

            // Scan the materials to make sure if this material exists already and update it if it does exist
            for (uint32_t i = 0; i < _weights.size(); i++)
            {
                if (_materials[i] == materialName)
                {
                    // Update the weight
                    float curWeight = _weights[i];
                    if (i > 0)
                    {
                        curWeight -= _weights[i - 1];
                    }

                    float weightDif = weight - curWeight;
                    _weights[i] = weight;

                    for (uint32_t j = i + 1; j < _weights.size(); j++)
                    {
                        _weights[j] += weightDif;
                    }

                    return;
                }
            }

            // Insert
            float finalWeight = (_weights.size() > 0) ? _weights[_weights.size() - 1] + weight : weight;
            _weights.push_back(finalWeight);
            _materials.push_back(materialName);
        }

        const std::string& GetMaterial() const
        {
            if (_weights.size() > 0)
            {
                float randVal = Random::RandomBetween(0.0f, _weights[_weights.size() - 1]);
                for (uint32_t i = 0; i < _weights.size(); i++)
                {
                    if (randVal < _weights[i])
                    {
                        return _materials[i];
                    }
                }
            }

            static const std::string emptyString;
            return emptyString;
        }

        void Clear()
        {
            _materials.clear();
            _weights.clear();
        }

    private:
        std::vector<std::string> _materials;
        std::vector<float> _weights;
    };
}
