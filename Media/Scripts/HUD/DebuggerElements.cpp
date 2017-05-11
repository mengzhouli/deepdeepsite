#include "HUD/DebuggerElements.hpp"

#include "Level/LevelLayerInstance.hpp"
#include "Level/LevelInstance.hpp"
#include "Ability/Ability.hpp"
#include "Item/Item.hpp"
#include "Item/Weapon.hpp"
#include "Item/Armor.hpp"
#include "Item/Trinket.hpp"

#include "Characters/Dummy.hpp"
#include "Characters/NecroKnight.hpp"
#include "Characters/Critter.hpp"
#include "Characters/Ork.hpp"
#include "Items/Weapons/OrkWeapons.hpp"
#include "Characters/Gnome.hpp"
#include "Characters/Worm.hpp"
#include "Characters/Gobbo.hpp"

#include "Characters/Dwarves/BrewerDwarf.hpp"
#include "Items/Weapons/Gun.hpp"
#include "Items/Armors/BrewerArmor.hpp"
#include "Items/Trinkets/Pipe.hpp"
#include "Abilities/ThrowBomb.hpp"
#include "Abilities/PlantDynamite.hpp"
#include "Abilities/Brew.hpp"

#include "Characters/Dwarves/BuilderDwarf.hpp"
#include "Items/Weapons/Hammer.hpp"
#include "Items/Armors/BuilderArmor.hpp"
#include "Abilities/BuildBridge.hpp"
#include "Abilities/BuildBarricade.hpp"

#include "Characters/Dwarves/CookDwarf.hpp"
#include "Items/Weapons/Kitchenware.hpp"
#include "Items/Armors/CookArmor.hpp"
#include "Items/Trinkets/Cookbook.hpp"
#include "Abilities/CookFood.hpp"
#include "Abilities/ChannelingFoodHeal.hpp"

#include "Characters/Dwarves/FighterDwarf.hpp"
#include "Items/Weapons/Sword.hpp"
#include "Items/Weapons/Shield.hpp"
#include "Items/Armors/FighterArmor.hpp"
#include "Abilities/Leap.hpp"

#include "Characters/Dwarves/MinerDwarf.hpp"
#include "Items/Weapons/PickAxe.hpp"
#include "Items/Weapons/Sack.hpp"
#include "Items/Armors/MinerArmor.hpp"
#include "Abilities/Sprint.hpp"
#include "Abilities/SackSmash.hpp"

#include "Characters/Dwarves/NavigatorDwarf.hpp"
#include "Items/Weapons/Gun.hpp"
#include "Items/Armors/NavigatorArmor.hpp"
#include "Items/Trinkets/Headlamp.hpp"
#include "Abilities/ShootFlare.hpp"
#include "Abilities/Grapple.hpp"

#include "Characters/Gobbo.hpp"
#include "Items/Weapons/GobboWeapons.hpp"

#include "Characters/Ork.hpp"
#include "Items/Weapons/OrkWeapons.hpp"
#include "Abilities/GroundSlam.hpp"

#include "Characters/MrBones.hpp"
#include "Items/Weapons/SkeletonWeapons.hpp"

#include "Characters/Gnome.hpp"

#include "Characters/Bomb.hpp"
#include "Characters/Dynamite.hpp"

#include "Characters/ResourceNode.hpp"

#include <imgui.h>

namespace Dwarf
{
    namespace HUD
    {
        FloatSliderDebuggerElement::FloatSliderDebuggerElement(const std::string &title, float min, float max, GetValueFunction getValueFunc, OnValueChangedFunction onValueChangedFunc)
            : _title(title)
            , _min(min)
            , _max(max)
            , _getValue(getValueFunc)
            , _onValueChanged(onValueChangedFunc)
        {
            if (_getValue)
            {
                _value = _getValue();
            }
        }

        void FloatSliderDebuggerElement::ReadFromXML(const XML::XMLNode& node)
        {
            float prevValue = _value;
            _value = node.GetChild("Value").GetValue(_value);
            if (prevValue != _value)
            {
                if (_onValueChanged)
                {
                    _onValueChanged(_value);
                }
            }
        }

        void FloatSliderDebuggerElement::WriteToXML(XML::XMLNode node)
        {
            node.AddChild("Value").SetValue(_value);
        }

        bool FloatSliderDebuggerElement::Update(double totalTime, float dt)
        {
            bool changed = false;

            if (_getValue)
            {
                _value = _getValue();
            }

            if (ImGui::SliderFloat(_title.c_str(), &_value, _min, _max))
            {
                if (_onValueChanged)
                {
                    _onValueChanged(_value);
                }
                changed = true;
            }

            return changed;
        }

        CheckboxDebuggerElement::CheckboxDebuggerElement(const std::string &title, GetValueFunction getValueFunc, OnValueChangedFunction onValueChangedFunc)
            : _title(title)
            , _getValue(getValueFunc)
            , _onValueChanged(onValueChangedFunc)
        {
            if (_getValue)
            {
                _value = _getValue();
            }
        }

        void CheckboxDebuggerElement::ReadFromXML(const XML::XMLNode& node)
        {
            bool prevValue = _value;
            _value = node.GetChild("Value").GetValue(_value);
            if (prevValue != _value)
            {
                if (_onValueChanged)
                {
                    _onValueChanged(_value);
                }
            }
        }

        void CheckboxDebuggerElement::WriteToXML(XML::XMLNode node)
        {
            node.AddChild("Value").SetValue(_value);
        }

        bool CheckboxDebuggerElement::Update(double totalTime, float dt)
        {
            bool changed = false;

            if (_getValue)
            {
                _value = _getValue();
            }

            if (ImGui::Checkbox(_title.c_str(), &_value))
            {
                if (_onValueChanged)
                {
                    _onValueChanged(_value);
                }
                changed = true;
            }

            return changed;
        }

        ComboboxDebuggerElement::ComboboxDebuggerElement(const std::string &title, bool saveValues, GetValuesFunction getValueFunc, OnValueChangedFunction onValueChangedFunc)
            : _title(title)
            , _saveValues(saveValues)
            , _getValue(getValueFunc)
            , _onValueChanged(onValueChangedFunc)
        {
            assert(_getValue);
            std::tie(_items, _curItem) = _getValue();
        }

        void ComboboxDebuggerElement::ReadFromXML(const XML::XMLNode& node)
        {
            if (!_saveValues)
            {
                return;
            }

            std::string selectedItem = node.GetChild("SelectedItem").GetValue(std::string());
            for (uint32_t i = 0; i < _items.size(); i++)
            {
                if (selectedItem == _items[i])
                {
                    if (_onValueChanged && _curItem != static_cast<int>(i))
                    {
                        _curItem = i;
                        _onValueChanged(_curItem);
                    }
                    break;
                }
            }
        }

        void ComboboxDebuggerElement::WriteToXML(XML::XMLNode node)
        {
            if (!_saveValues)
            {
                return;
            }
            node.AddChild("SelectedItem").SetValue(_curItem >= 0 ? _items[_curItem] : std::string());
        }

        bool ComboboxDebuggerElement::Update(double totalTime, float dt)
        {
            bool changed = false;

            std::tie(_items, _curItem) = _getValue();
            _curItem = Clamp(_curItem, -1, int32_t(_items.size()) - 1);

            std::vector<const char*> items(_items.size());
            for (uint32_t i = 0; i < _items.size(); i++)
            {
                items[i] = _items[i].c_str();
            }

            if (ImGui::Combo(_title.c_str(), &_curItem, items.data(), items.size()))
            {
                if (_onValueChanged)
                {
                    _onValueChanged(_curItem);
                }
                changed = true;
            }

            return changed;
        }
        
        ReverbEffectDebuggerElement::ReverbEffectDebuggerElement(const std::string& title, bool saveValues, Audio::ReverbEffect* effect)
            : _title(title)
            , _saveValues(saveValues)
            , _originalParams(effect->GetEffectParameters())
            , _effect(effect)
        {
            const auto& presetNames = Audio::ReverbEffectParameters::GetPresetNames();
            for (const auto& presetName : presetNames)
            {
                _presetNames.push_back(presetName.c_str());
            }
        }
        
        void ReverbEffectDebuggerElement::ReadFromXML(const XML::XMLNode& node)
        {
            if (!_saveValues)
            {
                return;
            }

            Audio::ReverbEffectParameters params = node.GetChild("Parameters").GetValue(_effect->GetEffectParameters());
            _effect->SetEffectParameters(params);
        }
        
        void ReverbEffectDebuggerElement::WriteToXML(XML::XMLNode node)
        {
            if (!_saveValues)
            {
                return;
            }

            node.AddChild("Parameters").SetValue(_effect->GetEffectParameters());
        }
        
        bool ReverbEffectDebuggerElement::Update(double totalTime, float dt)
        {
            bool changed = false;
            
            if (!ImGui::TreeNode(_title.c_str()))
            {
                return false;
            }
            
            Audio::ReverbEffectParameters params = _effect->GetEffectParameters();
            
            if (ImGui::Button("Reset"))
            {
                params = _originalParams;
                changed = true;
            }
            
            int selectedPreset = -1;
            if (ImGui::Combo("Preset", &selectedPreset, _presetNames.data(), _presetNames.size()))
            {
                params = Audio::ReverbEffectParameters::GetPresetByName(_presetNames[selectedPreset]);
                changed = true;
            }
            
            changed |= ImGui::SliderFloat("Density", &params.Density, 0.0f, 1.0f);
            changed |= ImGui::SliderFloat("Diffusion", &params.Diffusion, 0.0f, 1.0f);
            changed |= ImGui::SliderFloat("Gain", &params.Gain, 0.0f, 1.0f);
            changed |= ImGui::SliderFloat("GainHF", &params.GainHF, 0.0f, 1.0f);
            changed |= ImGui::SliderFloat("GainLF", &params.GainLF, 0.0f, 1.0f);
            changed |= ImGui::SliderFloat("DecayTime", &params.DecayTime, 0.1f, 20.0f);
            changed |= ImGui::SliderFloat("DecayHFRatio", &params.DecayHFRatio, 0.1f, 2.0f);
            changed |= ImGui::SliderFloat("DecayLFRatio", &params.DecayLFRatio, 0.1f, 2.0f);
            changed |= ImGui::SliderFloat("ReflectionsGain", &params.ReflectionsGain, 0.0f, 3.16f);
            changed |= ImGui::SliderFloat("ReflectionsDelay", &params.ReflectionsDelay, 0.0f, 0.3f);
            changed |= ImGui::SliderFloat("LateReverbGain", &params.LateReverbGain, 0.0f, 10.0f);
            changed |= ImGui::SliderFloat("LateReverbDelay", &params.LateReverbDelay, 0.0f, 0.1f);
            changed |= ImGui::SliderFloat("EchoTime", &params.EchoTime, 0.075f, 0.25f);
            changed |= ImGui::SliderFloat("EchoDepth", &params.EchoDepth, 0.0f, 1.0f);
            changed |= ImGui::SliderFloat("ModulationTime", &params.ModulationTime, 0.04f, 4.0f);
            changed |= ImGui::SliderFloat("ModulationDepth", &params.ModulationDepth, 0.0f, 1.0f);
            changed |= ImGui::SliderFloat("AirAbsorptionGainHF", &params.AirAbsorptionGainHF, 0.892f, 1.0f);
            changed |= ImGui::SliderFloat("HFReference", &params.HFReference, 1000.0f, 20000.0f);
            changed |= ImGui::SliderFloat("LFReference", &params.LFReference, 20.0f, 1000.0f);
            changed |= ImGui::SliderFloat("RoomRolloffFactor", &params.RoomRolloffFactor, 0.0f, 10.0f);
            changed |= ImGui::Checkbox("DecayHFLimit", &params.DecayHFLimit);
            
            ImGui::TreePop();
            
            if (changed)
            {
                _effect->SetEffectParameters(params);
            }
            
            return changed;
        }

        RainEffectDebuggerElement::RainEffectDebuggerElement(RainEffect* effect)
            : _effect(effect)
        {
        }

        void RainEffectDebuggerElement::ReadFromXML(const XML::XMLNode& node)
        {

        }

        void RainEffectDebuggerElement::WriteToXML(XML::XMLNode node)
        {

        }

        bool RainEffectDebuggerElement::Update(double totalTime, float dt)
        {
            bool changed = false;

            if (ImGui::TreeNode("Rain"))
            {
                Rotatorf rainDir = _effect->GetRainDirection();
                if (ImGui::SliderAngle("Rain Direction", &rainDir.Angle, -180.0f, 180.0f))
                {
                    _effect->SetRainDirection(rainDir);
                    changed = true;
                }

                float rainMinSpeed = _effect->GetRainMinSpeed();
                float rainMaxSpeed = _effect->GetRainMaxSpeed();
                if (ImGui::DragFloatRange2("Rain Speed", &rainMinSpeed, &rainMaxSpeed, 10.0f, 0.0f, 10000.0f))
                {
                    _effect->SetRainSpeed(rainMinSpeed, rainMaxSpeed);
                    changed = true;
                }

                int dropCount = _effect->GetDropCount();
                if (ImGui::SliderInt("Drop Count", &dropCount, 0, 16384))
                {
                    _effect->SetDropCount(dropCount);
                    changed = true;
                }

                Color rainColor = _effect->GetDropColor();
                float rainRGB[3] = { rainColor.R / 255.0f, rainColor.G / 255.0f, rainColor.B / 255.f };
                if (ImGui::ColorEdit3("Rain Color", rainRGB))
                {
                    _effect->SetDropColor(Color::FromFloats(rainRGB[0], rainRGB[1], rainRGB[2], 1.0f));
                    changed = true;
                }

                float minTransparency = _effect->GetDropMinTransparency();
                float maxTransparency = _effect->GetDropMaxTransparency();
                if (ImGui::DragFloatRange2("Rain Transparency", &minTransparency, &maxTransparency, 0.01f, 0.0f, 1.0f))
                {
                    _effect->SetDropTransparency(minTransparency, maxTransparency);
                    changed = true;
                }

                float dropMinSize = _effect->GetDropMinSize();
                float dropMaxSize = _effect->GetDropMaxSize();
                if (ImGui::DragFloatRange2("Rain Drop Size", &dropMinSize, &dropMaxSize, 1.0f, 1.0f, 1024.0f))
                {
                    _effect->SetDropSize(dropMinSize, dropMaxSize);
                    changed = true;
                }

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Lightning"))
            {
                bool lightningEnabled = _effect->IsLightningEnabled();
                if (ImGui::Checkbox("Lightning Enabled", &lightningEnabled))
                {
                    _effect->SetLightningEnabled(lightningEnabled);
                    changed = true;
                }

                float lightningStrikeMinInterval = _effect->GetLightningStrikeMinInterval();
                float lightningStrikeMaxInterval = _effect->GetLightningStrikeMaxInterval();
                float lightningStrikeDoubleChance = _effect->GetLightningDoubleStrikeChance();
                if (ImGui::DragFloatRange2("Lightning Strike Interval", &lightningStrikeMinInterval, &lightningStrikeMaxInterval, 1.0f, 1.0f, 60.0f) ||
                    ImGui::SliderFloat("Lightning Double Strike Chance", &lightningStrikeDoubleChance, 0.0f, 1.0f))
                {
                    _effect->SetLightningStrikeInterval(lightningStrikeMinInterval, lightningStrikeMaxInterval, lightningStrikeDoubleChance);
                    changed = true;
                }

                Color lightningColor = _effect->GetLightningColor();
                float lightningRGB[3] = { lightningColor.R / 255.0f, lightningColor.G / 255.0f, lightningColor.B / 255.f };
                if (ImGui::ColorEdit3("Lightning Color", lightningRGB))
                {
                    _effect->SetLightningColor(Color::FromFloats(lightningRGB[0], lightningRGB[1], lightningRGB[2], 1.0f));
                    changed = true;
                }

                float lightingRampUpTime = _effect->GetLightningRampUpTime();
                if (ImGui::SliderFloat("Lightning Ramp Up Time", &lightingRampUpTime, 0.0f, 0.5f))
                {
                    _effect->SetLightingRampUpTime(lightingRampUpTime);
                    changed = true;
                }

                float lightingFadeTime = _effect->GetLightningFadeTime();
                if (ImGui::SliderFloat("Lightning Fade Time", &lightingFadeTime, 0.0f, 2.0f))
                {
                    _effect->SetLightingFadeTime(lightingFadeTime);
                    changed = true;
                }

                ImGui::TreePop();
            }

            return changed;
        }

        class SpawnCharacterBindableDebugElement : public BindableDebuggerElemement
        {
        public:
            SpawnCharacterBindableDebugElement(Content::ContentManager* cm, Level::LevelLayerInstance* layer, GetSpawnPointFunction getSpwanPosFunc,
                const std::vector<SpawnCharacterOption>& options)
                : _cm(cm)
                , _layer(layer)
                , _getSpwanPosFunc(getSpwanPosFunc)
                , _options(options)
            {
            }

            void updateSelectedConstructor(int32_t constructor)
            {
                _selectedConstructor = constructor;

                _selectedWeapons.clear();
                _selectedArmor = -1;
                _selectedTrinket = -1;
                _selectedAbilities.clear();

                if (_selectedConstructor >= 0)
                {
                    _selectedWeapons.resize(_options[_selectedConstructor].WeaponOptions.size(), -1);
                    _selectedAbilities.resize(_options[_selectedConstructor].AbilityOptions.size(), -1);
                }
            }

            bool Update(double totalTime, float dt) override
            {
                bool changed = false;

                std::vector<const char*> constructorOptions(_options.size());
                for (uint32_t i = 0; i < _options.size(); i++)
                {
                    constructorOptions[i] = _options[i].Constructor.first.c_str();
                }

                if (ImGui::Combo("Character", &_selectedConstructor, constructorOptions.data(), constructorOptions.size()))
                {
                    updateSelectedConstructor(_selectedConstructor);

                    changed = true;
                }

                if (_selectedConstructor >= 0)
                {
                    std::vector<const char*> controllerOptions;
                    controllerOptions.push_back("null");
                    for (uint32_t i = 0; i < _layer->GetLevel()->GetControllerCount(); i++)
                    {
                        auto controller = _layer->GetLevel()->GetController(i);
                        controllerOptions.push_back(controller->GetName().c_str());
                    }
                    if (ImGui::Combo("Controller", &_selectedController, controllerOptions.data(), controllerOptions.size()))
                    {
                        changed = true;
                    }

                    const SpawnCharacterOption& selectedOption = _options[_selectedConstructor];

                    for (uint32_t i = 0; i < selectedOption.WeaponOptions.size(); i++)
                    {
                        if (selectedOption.WeaponOptions[i].size() > 0)
                        {
                            std::vector<const char*> weaponOptions = getNameVector(selectedOption.WeaponOptions[i]);
                            std::string title = Format("Weapon %u", i);
                            if (ImGui::Combo(title.c_str(), &_selectedWeapons[i], weaponOptions.data(), weaponOptions.size()))
                            {
                                changed = true;
                            }
                        }
                    }

                    if (selectedOption.ArmorOptions.size() > 0)
                    {
                        std::vector<const char*> armorOptions = getNameVector(selectedOption.ArmorOptions);
                        if (ImGui::Combo("Armor", &_selectedArmor, armorOptions.data(), armorOptions.size()))
                        {
                            changed = true;
                        }
                    }

                    if (selectedOption.TrinketOptions.size() > 0)
                    {
                        std::vector<const char*> trinketOptions = getNameVector(selectedOption.TrinketOptions);
                        if (ImGui::Combo("Trinket", &_selectedTrinket, trinketOptions.data(), trinketOptions.size()))
                        {
                            changed = true;
                        }
                    }

                    for (uint32_t i = 0; i < selectedOption.AbilityOptions.size(); i++)
                    {
                        if (selectedOption.AbilityOptions[i].size() > 0)
                        {
                            std::vector<const char*> abilityOptions = getNameVector(selectedOption.AbilityOptions[i]);
                            std::string title = Format("Ability %u", i);
                            if (ImGui::Combo(title.c_str(), &_selectedAbilities[i], abilityOptions.data(), abilityOptions.size()))
                            {
                                changed = true;
                            }
                        }
                    }
                }

                return changed;
            }

            void ReadFromXML(const XML::XMLNode& node) override
            {
                std::string constructor = node.GetChild("Constructor").GetValue<std::string>();
                for (uint32_t i = 0; i < _options.size(); i++)
                {
                    const auto& option = _options[i];
                    if (option.Constructor.first == constructor)
                    {
                        updateSelectedConstructor(i);

                        std::string controllerName = node.GetChild("Controller").GetValue<std::string>();
                        for (uint32_t j = 0; j < _layer->GetLevel()->GetControllerCount(); j++)
                        {
                            auto controller = _layer->GetLevel()->GetController(j);
                            if (controller->GetName() == controllerName)
                            {
                                _selectedController = j + 1;
                                break;
                            }
                        }

                        uint32_t curWeapon = 0;
                        for (XML::XMLNode weaponNode = node.GetChild("Weapon");
                            weaponNode && curWeapon < _selectedWeapons.size();
                            weaponNode = weaponNode.GetNextSibling("Weapon"), curWeapon++)
                        {
                            _selectedWeapons[curWeapon] = findOption(option.WeaponOptions[curWeapon], weaponNode.GetValue<std::string>());
                        }

                        _selectedArmor = findOption(option.ArmorOptions, node.GetChild("Armor").GetValue<std::string>());
                        _selectedTrinket = findOption(option.TrinketOptions, node.GetChild("Trinket").GetValue<std::string>());

                        uint32_t curAbility = 0;
                        for (XML::XMLNode abilityNode = node.GetChild("Ability");
                            abilityNode && curAbility < _selectedAbilities.size();
                            abilityNode = abilityNode.GetNextSibling("Ability"), curAbility++)
                        {
                            _selectedAbilities[curAbility] = findOption(option.AbilityOptions[curAbility], abilityNode.GetValue<std::string>());
                        }

                        break;
                    }
                }
            }

            void WriteToXML(XML::XMLNode node) override
            {
                bool constructorSelected = _selectedConstructor >= 0;
                node.AddChild("Constructor").SetValue(constructorSelected ? _options[_selectedConstructor].Constructor.first : std::string());
                if (constructorSelected)
                {
                    node.AddChild("Controller").SetValue(_selectedController > 0 ? _layer->GetLevel()->GetController(_selectedController - 1)->GetName() : std::string());

                    const auto& option = _options[_selectedConstructor];
                    for (uint32_t i = 0; i < _selectedWeapons.size(); i++)
                    {
                        node.AddChild("Weapon").SetValue(_selectedWeapons[i] >= 0 ? option.WeaponOptions[i][_selectedWeapons[i]].first : std::string());
                    }
                    node.AddChild("Armor").SetValue(_selectedArmor >= 0 ? option.ArmorOptions[_selectedArmor].first : std::string());
                    node.AddChild("Trinket").SetValue(_selectedTrinket >= 0 ? option.TrinketOptions[_selectedTrinket].first : std::string());
                    for (uint32_t i = 0; i < _selectedAbilities.size(); i++)
                    {
                        node.AddChild("Ability").SetValue(_selectedAbilities[i] >= 0 ? option.AbilityOptions[i][_selectedAbilities[i]].first : std::string());
                    }
                }
            }

            void BeginExecute() override
            {
                if (_selectedConstructor >= 0)
                {
                    bool wasLocked = _cm->IsLocked();
                    _cm->SetLocked(false);

                    const SpawnCharacterOption& selectedOption = _options[_selectedConstructor];
                    PreloadSet preloads;

                    Character::Controller* controller = _selectedController > 0 ? _layer->GetLevel()->GetController(_selectedController - 1) : nullptr;

                    auto characterConstructor = selectedOption.Constructor.second;
                    Character::Character* character = _layer->SpawnCharacter(_getSpwanPosFunc(), "debug_character", controller, characterConstructor);
                    characterConstructor.EnumeratePreloads(preloads);

                    for (uint32_t i = 0; i < _selectedWeapons.size(); i++)
                    {
                        if (_selectedWeapons[i] >= 0)
                        {
                            auto constructor = selectedOption.WeaponOptions[i][_selectedWeapons[i]].second;
                            character->GiveItem(constructor);
                            constructor.EnumeratePreloads(preloads);
                        }
                    }

                    if (_selectedArmor >= 0)
                    {
                        auto constructor = selectedOption.ArmorOptions[_selectedArmor].second;
                        character->GiveItem(selectedOption.ArmorOptions[_selectedArmor].second);
                        constructor.EnumeratePreloads(preloads);
                    }

                    if (_selectedTrinket >= 0)
                    {
                        auto constructor = selectedOption.TrinketOptions[_selectedTrinket].second;
                        character->GiveItem(constructor);
                        constructor.EnumeratePreloads(preloads);
                    }

                    for (uint32_t i = 0; i < _selectedAbilities.size(); i++)
                    {
                        if (_selectedAbilities[i] >= 0)
                        {
                            auto constructor = selectedOption.AbilityOptions[i][_selectedAbilities[i]].second;
                            character->AddAbility(constructor);
                            constructor.EnumeratePreloads(preloads);
                        }
                    }

                    for (const auto& preload : preloads)
                    {
                        _cm->Preload(preload);
                    }

                    _cm->SetLocked(wasLocked);
                }
            }

            std::string GetTitle() const override
            {
                std::string title = "Spawn character";
                if (_selectedConstructor >= 0)
                {
                    title += std::string(" (") + _options[_selectedConstructor].Constructor.first + ")";
                }
                return title;
            }

        private:
            template <typename T>
            static std::vector<const char*> getNameVector(const std::vector<std::pair<std::string, T>>& items)
            {
                std::vector<const char*> result(items.size());
                for (uint32_t i = 0; i < items.size(); i++)
                {
                    result[i] = items[i].first.c_str();
                }
                return result;
            }

            template <typename T>
            static int32_t findOption(const std::vector<std::pair<std::string, T>>& items, const std::string& item)
            {
                for (uint32_t i = 0; i < items.size(); i++)
                {
                    if (items[i].first == item)
                    {
                        return i;
                    }
                }
                return -1;
            }

            Content::ContentManager* _cm;
            Level::LevelLayerInstance* _layer;
            GetSpawnPointFunction _getSpwanPosFunc;

            std::vector<SpawnCharacterOption> _options;

            int _selectedConstructor = -1;
            int _selectedController = 0;
            std::vector<int> _selectedWeapons;
            int _selectedArmor = -1;
            int _selectedTrinket = -1;
            std::vector<int> _selectedAbilities;
        };

        BindableDebuggerElemementFactory CreateCharacterSpawnerElement(
            Content::ContentManager* cm, Level::LevelLayerInstance* layer, GetSpawnPointFunction getSpwanPosFunc, std::vector<SpawnCharacterOption> options)
        {
            return [=]()
            {
                return new SpawnCharacterBindableDebugElement(cm, layer, getSpwanPosFunc, options);
            };
        }

        BindableDebuggerElemementFactory CreateStandardCharacterSpawnerElement(
            Content::ContentManager* cm, Level::LevelLayerInstance* layer, GetSpawnPointFunction getSpwanPosFunc)
        {
            std::vector<SpawnCharacterOption> spawnCharacterOptions;

            {
                SpawnCharacterOption option;
                option.Constructor = { "BrewerDwarf", Character::BindCharacterConstructor<Character::BrewerDwarf>() };
                option.WeaponOptions =
                {
                    {
                        { "Rifle", Item::BindItemConstructor<Item::BasicRifle>() },
                        { "HandCannon", Item::BindItemConstructor<Item::HandCannonRifle>() },
                        { "Blunderbuss", Item::BindItemConstructor<Item::BlunderbussRifle>() },
                        { "BlackQueen", Item::BindItemConstructor<Item::BlackQueenRifle>() },
                    }
                };
                option.ArmorOptions =
                {
                    { "BrewerTunic", Item::BindItemConstructor<Item::BrewerTunic>() },
                };
                option.TrinketOptions =
                {
                    { "BriarPipe", Item::BindItemConstructor<Item::BriarPipe>() },
                    { "MeerschaumPipe", Item::BindItemConstructor<Item::MeerschaumPipe>() },
                    { "SteinPipe", Item::BindItemConstructor<Item::SteinPipe>() },
                };

                std::vector<std::pair<std::string, Ability::AbilityConstructor<>>> abilties =
                {
                    { "ThrowBomb", Ability::BindAbilityConstructor<Ability::ThrowBomb>() },
                    { "PlantDynamite", Ability::BindAbilityConstructor<Ability::PlantDynamite>() },
                    { "BrewBeer", Ability::BindAbilityConstructor<Ability::BrewBeer>() },
                    { "BrewWine", Ability::BindAbilityConstructor<Ability::BrewWine>() },
                };
                option.AbilityOptions =
                {
                    abilties,
                    abilties,
                    abilties,
                    abilties,
                };

                spawnCharacterOptions.push_back(option);
            }

            {
                SpawnCharacterOption option;
                option.Constructor = { "BuilderDwarf", Character::BindCharacterConstructor<Character::BuilderDwarf>() };
                option.WeaponOptions =
                {
                    {
                        { "WoodHammer", Item::BindItemConstructor<Item::WoodHammer>() },
                        { "IronHammer", Item::BindItemConstructor<Item::IronHammer>() },
                        { "GoldHammer", Item::BindItemConstructor<Item::GoldHammer>() },
                    }
                };
                option.ArmorOptions =
                {
                    { "BuilderTunic", Item::BindItemConstructor<Item::BuilderTunic>() },
                };
                option.TrinketOptions =
                {
                };

                std::vector<std::pair<std::string, Ability::AbilityConstructor<>>> abilties =
                {
                    { "BuildBridge", Ability::BindAbilityConstructor<Ability::BuildBridge>() },
                    { "BuildBarricade", Ability::BindAbilityConstructor<Ability::BuildBarricade>() },
                };
                option.AbilityOptions =
                {
                    abilties,
                    abilties,
                    abilties,
                    abilties,
                };

                spawnCharacterOptions.push_back(option);
            }

            {
                SpawnCharacterOption option;
                option.Constructor = { "CookDwarf", Character::BindCharacterConstructor<Character::CookDwarf>() };
                option.WeaponOptions =
                {
                    {
                        { "Cutlery", Item::BindItemConstructor<Item::Cutlery>() },
                        { "ServingTools", Item::BindItemConstructor<Item::ServingTools>() },
                        { "PrepTools", Item::BindItemConstructor<Item::PrepTools>() },
                        { "ButcherTools", Item::BindItemConstructor<Item::ButcherTools>() },
                        { "Cookware", Item::BindItemConstructor<Item::Cookware>() },
                        { "AllKitchenware", Item::BindItemConstructor<Item::AllKitchenware>() },
                    }
                };
                option.ArmorOptions =
                {
                    { "Apron", Item::BindItemConstructor<Item::Apron>() },
                };
                option.TrinketOptions =
                {
                    { "DeVermisCoctioneCookbook", Item::BindItemConstructor<Item::DeVermisCoctioneCookbook>() },
                    { "TheChefInYellowCookbook", Item::BindItemConstructor<Item::TheChefInYellowCookbook>() },
                    { "CulinomiconCookbook", Item::BindItemConstructor<Item::CulinomiconCookbook>() },
                };

                std::vector<std::pair<std::string, Ability::AbilityConstructor<>>> abilties =
                {
                    { "CookBeef", Ability::BindAbilityConstructor<Ability::CookBeef>() },
                    { "CookChicken", Ability::BindAbilityConstructor<Ability::CookChicken>() },
                    { "CookHam", Ability::BindAbilityConstructor<Ability::CookHam>() },
                    { "CookCritFood", Ability::BindAbilityConstructor<Ability::CookCritFood>() },
                    { "CookAttackSpeedFood", Ability::BindAbilityConstructor<Ability::CookAttackSpeedFood>() },
                    { "ChannelingFoodHeal", Ability::BindAbilityConstructor<Ability::ChannelingFoodHeal>() }
                };
                option.AbilityOptions =
                {
                    abilties,
                    abilties,
                    abilties,
                    abilties,
                };

                spawnCharacterOptions.push_back(option);
            }

            {
                SpawnCharacterOption option;
                option.Constructor = { "FighterDwarf", Character::BindCharacterConstructor<Character::FighterDwarf>() };
                option.WeaponOptions =
                {
                    {
                        { "WoodenShortSword", Item::BindItemConstructor<Item::WoodenShortSword>() },
                        { "RustyIronShortSword", Item::BindItemConstructor<Item::RustyIronShortSword>() },
                        { "IronShortSword", Item::BindItemConstructor<Item::IronShortSword>() },
                        { "IgnitionPinSword", Item::BindItemConstructor<Item::IgnitionPinSword>() },
                        { "LettersbaneSword", Item::BindItemConstructor<Item::LettersbaneSword>() },
                        { "UncalculatedDivisionSword", Item::BindItemConstructor<Item::UncalculatedDivisionSword>() },
                    },
                    {
                        { "WoodenShortSword", Item::BindItemConstructor<Item::WoodenShortSword>() },
                        { "RustyIronShortSword", Item::BindItemConstructor<Item::RustyIronShortSword>() },
                        { "IronShortSword", Item::BindItemConstructor<Item::IronShortSword>() },
                        { "IgnitionPinSword", Item::BindItemConstructor<Item::IgnitionPinSword>() },
                        { "LettersbaneSword", Item::BindItemConstructor<Item::LettersbaneSword>() },
                        { "UncalculatedDivisionSword", Item::BindItemConstructor<Item::UncalculatedDivisionSword>() },
                        { "WoodenBasicShield", Item::BindItemConstructor<Item::WoodenBasicShield>() },
                        { "WoodenHeater", Item::BindItemConstructor<Item::WoodenHeater>() },
                        { "BracedWoodenHeater", Item::BindItemConstructor<Item::BracedWoodenHeater>() },
                        { "IronHeater", Item::BindItemConstructor<Item::IronHeater>() },
                        { "WoodenRoundShield", Item::BindItemConstructor<Item::WoodenRoundShield>() },
                        { "TowerShield", Item::BindItemConstructor<Item::TowerShield>() },
                    },
                };
                option.ArmorOptions =
                {
                    { "Chainmail", Item::BindItemConstructor<Item::Chainmail>() },
                    { "BristleNogginArmor", Item::BindItemConstructor<Item::BristleNogginArmor>() },
                    { "BronzeArmor", Item::BindItemConstructor<Item::BronzeArmor>() },
                    { "TwinFangArmor", Item::BindItemConstructor<Item::TwinFangArmor>() },
                    { "BladeArmor", Item::BindItemConstructor<Item::BladeArmor>() },
                    { "WokArmor", Item::BindItemConstructor<Item::WokArmor>() },
                    { "TeapotArmor", Item::BindItemConstructor<Item::TeapotArmor>() },
                };
                option.TrinketOptions =
                {
                };

                std::vector<std::pair<std::string, Ability::AbilityConstructor<>>> abilties =
                {
                    { "Leap", Ability::BindAbilityConstructor<Ability::Leap>() },
                };
                option.AbilityOptions =
                {
                    abilties,
                    abilties,
                    abilties,
                    abilties,
                };

                spawnCharacterOptions.push_back(option);
            }

            {
                SpawnCharacterOption option;
                option.Constructor = { "MinerDwarf", Character::BindCharacterConstructor<Character::MinerDwarf>() };
                option.WeaponOptions =
                {
                    {
                        { "WoodPickAxe", Item::BindItemConstructor<Item::WoodPickAxe>() },
                        { "IronPickAxe", Item::BindItemConstructor<Item::IronPickAxe>() },
                        { "GoldPickAxe", Item::BindItemConstructor<Item::GoldPickAxe>() },
                        { "DiamondPickAxe", Item::BindItemConstructor<Item::DiamondPickAxe>() },
                    },
                    {
                        { "Sack", Item::BindItemConstructor<Item::Sack>() },
                    },
                };
                option.ArmorOptions =
                {
                    { "MinerTunic", Item::BindItemConstructor<Item::MinerTunic>() },
                };
                option.TrinketOptions =
                {
                };

                std::vector<std::pair<std::string, Ability::AbilityConstructor<>>> abilties =
                {
                    { "Sprint", Ability::BindAbilityConstructor<Ability::Sprint>() },
                    { "SackSmash", Ability::BindAbilityConstructor<Ability::SackSmash>() },
                };
                option.AbilityOptions =
                {
                    abilties,
                    abilties,
                    abilties,
                    abilties,
                };

                spawnCharacterOptions.push_back(option);
            }

            {
                SpawnCharacterOption option;
                option.Constructor = { "NavigatorDwarf", Character::BindCharacterConstructor<Character::NavigatorDwarf>() };
                option.WeaponOptions =
                {
                    {
                        { "BasicPistol", Item::BindItemConstructor<Item::BasicPistol>() },
                        { "FingerCannonPistol", Item::BindItemConstructor<Item::FingerCannonPistol>() },
                        { "BlunderbussPistol", Item::BindItemConstructor<Item::BlunderbussPistol>() },
                        { "BlackPrincessPistol", Item::BindItemConstructor<Item::BlackPrincessPistol>() },
                    },
                };
                option.ArmorOptions =
                {
                    { "NavigatorTunic", Item::BindItemConstructor<Item::NavigatorTunic>() },
                };
                option.TrinketOptions =
                {
                    { "BasicHeadlamp", Item::BindItemConstructor<Item::BasicHeadlamp>() },
                    { "Flashlight", Item::BindItemConstructor<Item::Flashlight>() },
                    { "Lantern", Item::BindItemConstructor<Item::Lantern>() },
                };

                std::vector<std::pair<std::string, Ability::AbilityConstructor<>>> abilties =
                {
                    { "ShootFlare", Ability::BindAbilityConstructor<Ability::ShootFlare>() },
                    { "Grapple", Ability::BindAbilityConstructor<Ability::Grapple>() },
                };
                option.AbilityOptions =
                {
                    abilties,
                    abilties,
                    abilties,
                    abilties,
                };

                spawnCharacterOptions.push_back(option);
            }

            {
                SpawnCharacterOption option;
                option.Constructor = { "Gobbo", Character::BindCharacterConstructor<Character::Gobbo>() };
                option.WeaponOptions =
                {
                    {
                        { "GobBlade", Item::BindItemConstructor<Item::GobBlade>() },
                        { "GobDagger", Item::BindItemConstructor<Item::GobDagger>() },
                        { "GobFork", Item::BindItemConstructor<Item::GobFork>() },
                        { "GobSword", Item::BindItemConstructor<Item::GobSword>() },
                        { "GobTrident", Item::BindItemConstructor<Item::GobTrident>() },
                        { "GobJawBlade", Item::BindItemConstructor<Item::GobJawBlade>() },
                        { "GobSkullClub", Item::BindItemConstructor<Item::GobSkullClub>() },
                        { "GobOrkAxe", Item::BindItemConstructor<Item::GobOrkAxe>() },
                        { "GobOrkSickle", Item::BindItemConstructor<Item::GobOrkSickle>() },
                    },
                };

                spawnCharacterOptions.push_back(option);
            }

            {
                SpawnCharacterOption option;
                option.Constructor = { "GobboThrower", Character::BindCharacterConstructor<Character::GobboThrower>() };
                option.WeaponOptions =
                {
                    {
                        { "GobThrowingRocks", Item::BindItemConstructor<Item::GobThrowingRocks>() },
                    },
                };

                spawnCharacterOptions.push_back(option);
            }

            {
                SpawnCharacterOption option;
                option.Constructor = { "GobboBowman", Character::BindCharacterConstructor<Character::GobboBowman>() };
                option.WeaponOptions =
                {
                    {
                        { "GobBow", Item::BindItemConstructor<Item::GobBow>() },
                    },
                };

                spawnCharacterOptions.push_back(option);
            }

            {
                SpawnCharacterOption option;
                option.Constructor = { "GobboChief", Character::BindCharacterConstructor<Character::GobboChief>() };
                option.WeaponOptions =
                {
                    {
                        { "GobBlade", Item::BindItemConstructor<Item::GobBlade>() },
                        { "GobDagger", Item::BindItemConstructor<Item::GobDagger>() },
                        { "GobFork", Item::BindItemConstructor<Item::GobFork>() },
                        { "GobSword", Item::BindItemConstructor<Item::GobSword>() },
                        { "GobTrident", Item::BindItemConstructor<Item::GobTrident>() },
                        { "GobJawBlade", Item::BindItemConstructor<Item::GobJawBlade>() },
                        { "GobSkullClub", Item::BindItemConstructor<Item::GobSkullClub>() },
                        { "GobOrkAxe", Item::BindItemConstructor<Item::GobOrkAxe>() },
                        { "GobOrkSickle", Item::BindItemConstructor<Item::GobOrkSickle>() },
                    },
                };

                spawnCharacterOptions.push_back(option);
            }

            {
                SpawnCharacterOption option;
                option.Constructor = { "GobboTower", Character::BindCharacterConstructor<Character::GobboTower>() };

                spawnCharacterOptions.push_back(option);
            }

            {
                SpawnCharacterOption option;
                option.Constructor = { "GobboBramble", Character::BindCharacterConstructor<Character::GobboBramble>() };

                spawnCharacterOptions.push_back(option);
            }

            {
                SpawnCharacterOption option;
                option.Constructor = { "OrkWarrior", Character::BindCharacterConstructor<Character::OrkWarrior>() };
                option.WeaponOptions =
                {
                    {
                        { "OrkChopper", Item::BindItemConstructor<Item::OrkChopper>() },
                        { "OrkClub", Item::BindItemConstructor<Item::OrkClub>() },
                        { "OrkHook", Item::BindItemConstructor<Item::OrkHook>() },
                        { "OrkMace", Item::BindItemConstructor<Item::OrkMace>() },
                    },
                    {
                        { "OrkChopper", Item::BindItemConstructor<Item::OrkChopper>() },
                        { "OrkClub", Item::BindItemConstructor<Item::OrkClub>() },
                        { "OrkHook", Item::BindItemConstructor<Item::OrkHook>() },
                        { "OrkMace", Item::BindItemConstructor<Item::OrkMace>() },
                    },
                };

                std::vector<std::pair<std::string, Ability::AbilityConstructor<>>> abilties =
                {
                    { "GroundSlam", Ability::BindAbilityConstructor<Ability::GroundSlam>() },
                    { "Leap", Ability::BindAbilityConstructor<Ability::Leap>() },
                };
                option.AbilityOptions =
                {
                    abilties,
                    abilties,
                    abilties,
                    abilties,
                };

                spawnCharacterOptions.push_back(option);
            }

            {
                SpawnCharacterOption option;
                option.Constructor = { "OrkCannoneer", Character::BindCharacterConstructor<Character::OrkCannoneer>() };

                spawnCharacterOptions.push_back(option);
            }

            {
                SpawnCharacterOption option;
                option.Constructor = { "ShieldOrk", Character::BindCharacterConstructor<Character::ShieldOrk>() };

                spawnCharacterOptions.push_back(option);
            }

            {
                SpawnCharacterOption option;
                option.Constructor = { "OrkLord", Character::BindCharacterConstructor<Character::OrkLord>() };

                std::vector<std::pair<std::string, Ability::AbilityConstructor<>>> abilties =
                {
                    { "Leap", Ability::BindAbilityConstructor<Ability::Leap>(5000.0f, 15.0f) },
                };
                option.AbilityOptions =
                {
                    abilties,
                    abilties,
                    abilties,
                    abilties,
                };

                spawnCharacterOptions.push_back(option);
            }

            {
                SpawnCharacterOption option;
                option.Constructor = { "MrBonesMelee", Character::BindCharacterConstructor<Character::MrBonesMelee>() };
                option.WeaponOptions =
                {
                    {
                        { "SkelAxe", Item::BindItemConstructor<Item::SkelAxe>() },
                        { "SkelDagger", Item::BindItemConstructor<Item::SkelDagger>() },
                        { "SkelHalberd", Item::BindItemConstructor<Item::SkelHalberd>() },
                        { "SkelScimitar", Item::BindItemConstructor<Item::SkelScimitar>() },
                    },
                    {
                        { "SkelAxe", Item::BindItemConstructor<Item::SkelAxe>() },
                        { "SkelDagger", Item::BindItemConstructor<Item::SkelDagger>() },
                        { "SkelHalberd", Item::BindItemConstructor<Item::SkelHalberd>() },
                        { "SkelScimitar", Item::BindItemConstructor<Item::SkelScimitar>() },
                    },
                };

                spawnCharacterOptions.push_back(option);
            }

            {
                SpawnCharacterOption option;
                option.Constructor = { "MrBonesArcher", Character::BindCharacterConstructor<Character::MrBonesArcher>() };
                option.WeaponOptions =
                {
                    {
                        { "SkelBow", Item::BindItemConstructor<Item::SkelBow>() },
                    },
                };

                spawnCharacterOptions.push_back(option);
            }

            {
                SpawnCharacterOption option;
                option.Constructor = { "Gnome", Character::BindCharacterConstructor<Character::Gnome>() };
                option.WeaponOptions =
                {
                    {
                        { "GnomeWrench", Item::BindItemConstructor<Item::GnomeWrench>() },
                    },
                };

                spawnCharacterOptions.push_back(option);
            }

            {
                SpawnCharacterOption option;
                option.Constructor = { "Dummy", Character::BindCharacterConstructor<Character::Dummy>() };

                spawnCharacterOptions.push_back(option);
            }

            {
                SpawnCharacterOption option;
                option.Constructor = { "IronBomb", Character::BindCharacterConstructor<Character::IronBomb>(0, 3.0f, 1000.0f, 100.0f) };

                spawnCharacterOptions.push_back(option);
            }

            {
                SpawnCharacterOption option;
                option.Constructor = { "Dynamite", Character::BindCharacterConstructor<Character::Dynamite>(0, 3.0f, 1000.0f, 100.0f) };

                spawnCharacterOptions.push_back(option);
            }

            {
                SpawnCharacterOption option;
                option.Constructor = { "GoldNode", Character::BindCharacterConstructor<Character::GoldNode>(Item::Resources(100)) };

                spawnCharacterOptions.push_back(option);
            }

            return CreateCharacterSpawnerElement(cm, layer, getSpwanPosFunc, spawnCharacterOptions);
        }

        class CreateImpulseBindableDebugElement : public BindableDebuggerElemement
        {
        public:
            CreateImpulseBindableDebugElement(Level::LevelLayerInstance* layer, GetSpawnPointFunction getSpwanPosFunc)
                : _layer(layer)
                , _getSpwanPosFunc(getSpwanPosFunc)
            {
            }

            bool Update(double totalTime, float dt) override
            {
                bool changed = false;

                changed |= ImGui::SliderFloat("Radius", &_radius, 0.0f, 3000.0f);
                changed |= ImGui::SliderFloat("Velocity", &_velocity, 0.0f, 1500.0f);

                return changed;
            }

            void ReadFromXML(const XML::XMLNode& node) override
            {
                _radius = node.GetChild("Radius").GetValue<float>(_radius);
                _velocity = node.GetChild("Velocity").GetValue<float>(_velocity);
            }

            void WriteToXML(XML::XMLNode node) override
            {
                node.AddChild("Radius").SetValue(_radius);
                node.AddChild("Velocity").SetValue(_velocity);
            }

            void BeginExecute() override
            {
                Vector2f origin = _getSpwanPosFunc();
                auto characters =
                    _layer->GetCharacters<Character::Character>([&](const Character::Character* c) { return Vector2f::Distance(origin, c->GetPosition()) <= _radius; });
                for (auto character : characters)
                {
                    Vector2f direction = character->GetPosition() - origin;
                    float charDist = direction.Length();
                    auto collision = character->GetCollision();
                    float mass = collision ? collision->GetMass() : 1.0f;
                    float perc = _radius > 0.0f ? 1.0f - (charDist / _radius) : 0.0f;
                    character->ApplyLinearImpulse(Vector2f::Normalize(direction) * perc * mass * _velocity);
                }
            }

            std::string GetTitle() const override
            {
                return "Create impulse";
            }

        private:
            Level::LevelLayerInstance* _layer;
            GetSpawnPointFunction _getSpwanPosFunc;

            float _radius = 1500.0f;
            float _velocity = 750.0f;
        };

        BindableDebuggerElemementFactory CreateImpulseElement(Level::LevelLayerInstance* layer, GetSpawnPointFunction getSpwanPosFunc)
        {
            return [=]()
            {
                return new CreateImpulseBindableDebugElement(layer, getSpwanPosFunc);
            };
        }
        class PlaySoundBindableDebugElement : public BindableDebuggerElemement
        {
        public:
            PlaySoundBindableDebugElement(Level::LevelLayerInstance* layer, GetSpawnPointFunction getSpwanPosFunc, Content::ContentManager* cm)
                : _layer(layer)
                , _cm(cm)
                , _getSpwanPosFunc(getSpwanPosFunc)
            {
                bool wasFSLoadEnabled = _cm->IsFilesystemFallbackEnabled();
                _cm->SetFilesystemFallbackEnabled(true);
                _fullFiles = _cm->FindAllContentFiles<Audio::Sound>();
                _cm->SetFilesystemFallbackEnabled(wasFSLoadEnabled);

                _files.reserve(_fullFiles.size());
                for (const auto& file : _fullFiles)
                {
                    _files.push_back(FileSystem::GetFileName(file));
                }
            }

            bool Update(double totalTime, float dt) override
            {
                bool changed = false;

                std::vector<const char*> fileStrings(_files.size());
                for (uint32_t i = 0; i < _files.size(); i++)
                {
                    fileStrings[i] = _files[i].c_str();
                }

                changed |= ImGui::Combo("File", &_selectedFile, fileStrings.data(), fileStrings.size());
                changed |= ImGui::SliderFloat("Volume", &_volume, 0.0f, 1.0f);
                changed |= ImGui::SliderFloat("Pitch", &_pitch, 0.0f, 5.0f);
                changed |= ImGui::Checkbox("Loop", &_loop);
                if (_loop)
                {
                    changed |= ImGui::SliderFloat("Fade Time", &_fadeTime, 0.0f, 1.0f);
                }
                changed |= ImGui::Checkbox("Positional", &_positional);
                if (_positional)
                {
                    changed |= ImGui::SliderFloat("Min Radius", &_minRadius, 0.0f, 5000.0f);
                    _minRadius = Min(_minRadius, _maxRadius);
                    changed |= ImGui::SliderFloat("Max Radius", &_maxRadius, 0.0f, 5000.0f);
                    _maxRadius = Max(_minRadius, _maxRadius);
                }

                if (_curLoopingInstance)
                {
                    _curLoopingInstance->SetPosition(_getSpwanPosFunc());
                }

                return changed;
            }

            void ReadFromXML(const XML::XMLNode& node) override
            {
                std::string file = node.GetChild("File").GetValue<std::string>("");
                for (uint32_t i = 0; i < _files.size(); i++)
                {
                    if (_files[i] == file)
                    {
                        _selectedFile = i;
                        break;
                    }
                }

                _volume = node.GetChild("Volume").GetValue<float>(_volume);
                _pitch = node.GetChild("Pitch").GetValue<float>(_pitch);
                _loop = node.GetChild("Loop").GetValue<bool>(_loop);
                _fadeTime = node.GetChild("FadeTime").GetValue<float>(_loop);
                _positional = node.GetChild("Positional").GetValue<bool>(_positional);
                _minRadius = node.GetChild("MinRadius").GetValue<float>(_minRadius);
                _maxRadius = node.GetChild("MaxRadius").GetValue<float>(_maxRadius);
            }

            void WriteToXML(XML::XMLNode node) override
            {
                node.AddChild("File").SetValue(_selectedFile >= 0 ? _files[_selectedFile] : std::string());
                node.AddChild("Volume").SetValue(_volume);
                node.AddChild("Pitch").SetValue(_pitch);
                node.AddChild("Loop").SetValue(_loop);
                node.AddChild("FadeTime").SetValue(_fadeTime);
                node.AddChild("Positional").SetValue(_positional);
                node.AddChild("MinRadius").SetValue(_minRadius);
                node.AddChild("MaxRadius").SetValue(_maxRadius);
            }

            void BeginExecute() override
            {
                if (_selectedFile < 0)
                {
                    return;
                }

                const Audio::Sound* sound = nullptr;

                bool wasLocked = _cm->IsLocked();
                _cm->SetLocked(false);

                bool wasFSLoadEnabled = _cm->IsFilesystemFallbackEnabled();
                _cm->SetFilesystemFallbackEnabled(true);

                try
                {
                    sound = _cm->Load<Audio::Sound>(_fullFiles[_selectedFile]);
                    _cm->SetLocked(wasLocked);
                    _cm->SetFilesystemFallbackEnabled(wasFSLoadEnabled);
                }
                catch (const Exception& e)
                {
                    _cm->SetLocked(wasLocked);
                    _cm->SetFilesystemFallbackEnabled(wasFSLoadEnabled);
                    LogError("PlaySoundBindableDebugElement", Format("Failed to load sound file: %s.", e.GetErrorMessage().c_str()));
                    return;
                }

                Vector2f origin = _getSpwanPosFunc();
                auto soundManager = _layer->GetLevel()->GetSoundManager();

                std::shared_ptr<Audio::ManagedSoundInstance> soundInstance = nullptr;

                if (_loop)
                {
                    if (_positional)
                    {
                        soundInstance = soundManager->PlayLoopingPositionalSound(sound, Audio::SoundPriority::High, _getSpwanPosFunc(), _minRadius, _maxRadius, _volume);
                    }
                    else
                    {
                        soundInstance = soundManager->PlayLoopingGlobalSound(sound, Audio::SoundPriority::High, _volume);
                    }
                    _curLoopingInstance = soundInstance;
                }
                else
                {
                    if (_positional)
                    {
                        soundInstance = soundManager->PlaySinglePositionalSound(sound, Audio::SoundPriority::High, _getSpwanPosFunc(), _minRadius, _maxRadius, _volume);
                    }
                    else
                    {
                        soundInstance = soundManager->PlaySingleGlobalSound(sound, Audio::SoundPriority::High, _volume);
                    }
                }

                SafeRelease(sound);

                if (soundInstance)
                {
                    soundInstance->SetPitch(_pitch);
                }
            }

            void EndExecute() override
            {
                if (_curLoopingInstance)
                {
                    _curLoopingInstance->Stop(_fadeTime);
                    _curLoopingInstance = nullptr;
                }
            }

            std::string GetTitle() const override
            {
                std::string title = "Play sound";
                if (_selectedFile >= 0)
                {
                    title += std::string(" (") + _files[_selectedFile] + ")";
                }
                return title;
            }

        private:
            Level::LevelLayerInstance* _layer;
            Content::ContentManager* _cm;
            GetSpawnPointFunction _getSpwanPosFunc;

            std::vector<std::string> _fullFiles;
            std::vector<std::string> _files;
            int _selectedFile = -1;

            bool _positional = true;
            bool _loop = false;
            float _fadeTime = 0.0f;
            float _volume = 1.0f;
            float _minRadius = 1000.0f;
            float _maxRadius = 3000.0f;
            float _pitch = 1.0f;

            std::shared_ptr<Audio::ManagedSoundInstance> _curLoopingInstance;
        };

        BindableDebuggerElemementFactory CreatePlaySoundElement(Level::LevelLayerInstance* layer, GetSpawnPointFunction getSpwanPosFunc, Content::ContentManager* cm)
        {
            return [=]()
            {
                return new PlaySoundBindableDebugElement(layer, getSpwanPosFunc, cm);
            };
        }
    }

    void EnumerateCreateStandardCharacterSpawnerElementPreloads(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::BrewerDwarf>(preloads);
        EnumeratePreloads<Item::BasicRifle>(preloads);
        EnumeratePreloads<Item::BlunderbussRifle>(preloads);
        EnumeratePreloads<Item::BlackQueenRifle>(preloads);
        EnumeratePreloads<Item::BrewerTunic>(preloads);
        EnumeratePreloads<Item::BriarPipe>(preloads);
        EnumeratePreloads<Item::MeerschaumPipe>(preloads);
        EnumeratePreloads<Item::SteinPipe>(preloads);
        EnumeratePreloads<Ability::ThrowBomb>(preloads);
        EnumeratePreloads<Ability::PlantDynamite>(preloads);

        EnumeratePreloads<Character::BuilderDwarf>(preloads);
        EnumeratePreloads<Item::WoodHammer>(preloads);
        EnumeratePreloads<Item::IronHammer>(preloads);
        EnumeratePreloads<Item::GoldHammer>(preloads);
        EnumeratePreloads<Item::BuilderTunic>(preloads);
        EnumeratePreloads<Ability::BuildBridge>(preloads);
        EnumeratePreloads<Ability::BuildBarricade>(preloads);

        EnumeratePreloads<Character::CookDwarf>(preloads);
        EnumeratePreloads<Item::Cutlery>(preloads);
        EnumeratePreloads<Item::ServingTools>(preloads);
        EnumeratePreloads<Item::PrepTools>(preloads);
        EnumeratePreloads<Item::ButcherTools>(preloads);
        EnumeratePreloads<Item::Cookware>(preloads);
        EnumeratePreloads<Item::AllKitchenware>(preloads);
        EnumeratePreloads<Item::Apron>(preloads);
        EnumeratePreloads<Item::DeVermisCoctioneCookbook>(preloads);
        EnumeratePreloads<Item::TheChefInYellowCookbook>(preloads);
        EnumeratePreloads<Item::CulinomiconCookbook>(preloads);
        EnumeratePreloads<Ability::CookBeef>(preloads);
        EnumeratePreloads<Ability::CookChicken>(preloads);
        EnumeratePreloads<Ability::CookHam>(preloads);

        EnumeratePreloads<Character::FighterDwarf>(preloads);
        EnumeratePreloads<Item::WoodenShortSword>(preloads);
        EnumeratePreloads<Item::RustyIronShortSword>(preloads);
        EnumeratePreloads<Item::IronShortSword>(preloads);
        EnumeratePreloads<Item::WoodenBasicShield>(preloads);
        EnumeratePreloads<Item::WoodenHeater>(preloads);
        EnumeratePreloads<Item::BracedWoodenHeater>(preloads);
        EnumeratePreloads<Item::IronHeater>(preloads);
        EnumeratePreloads<Item::WoodenRoundShield>(preloads);
        EnumeratePreloads<Item::TowerShield>(preloads);
        EnumeratePreloads<Item::Chainmail>(preloads);
        EnumeratePreloads<Item::BladeArmor>(preloads);
        EnumeratePreloads<Item::WokArmor>(preloads);
        EnumeratePreloads<Item::TeapotArmor>(preloads);
        EnumeratePreloads<Ability::Leap>(preloads);

        EnumeratePreloads<Character::MinerDwarf>(preloads);
        EnumeratePreloads<Item::WoodPickAxe>(preloads);
        EnumeratePreloads<Item::IronPickAxe>(preloads);
        EnumeratePreloads<Item::GoldPickAxe>(preloads);
        EnumeratePreloads<Item::DiamondPickAxe>(preloads);
        EnumeratePreloads<Item::Sack>(preloads);
        EnumeratePreloads<Item::MinerTunic>(preloads);
        EnumeratePreloads<Ability::Sprint>(preloads);
        EnumeratePreloads<Ability::SackSmash>(preloads);

        EnumeratePreloads<Character::NavigatorDwarf>(preloads);
        EnumeratePreloads<Item::BasicPistol>(preloads);
        EnumeratePreloads<Item::FingerCannonPistol>(preloads);
        EnumeratePreloads<Item::BlunderbussPistol>(preloads);
        EnumeratePreloads<Item::BlackPrincessPistol>(preloads);
        EnumeratePreloads<Item::NavigatorTunic>(preloads);
        EnumeratePreloads<Ability::ShootFlare>(preloads);
        EnumeratePreloads<Ability::Grapple>(preloads);

        EnumeratePreloads<Character::Gobbo>(preloads);
        EnumeratePreloads<Item::GobBlade>(preloads);
        EnumeratePreloads<Item::GobDagger>(preloads);
        EnumeratePreloads<Item::GobFork>(preloads);
        EnumeratePreloads<Item::GobSword>(preloads);
        EnumeratePreloads<Item::GobTrident>(preloads);
        EnumeratePreloads<Item::GobJawBlade>(preloads);
        EnumeratePreloads<Item::GobSkullClub>(preloads);
        EnumeratePreloads<Item::GobOrkAxe>(preloads);
        EnumeratePreloads<Item::GobOrkSickle>(preloads);

        EnumeratePreloads<Character::GobboThrower>(preloads);
        EnumeratePreloads<Item::GobThrowingRocks>(preloads);

        EnumeratePreloads<Character::GobboBowman>(preloads);
        EnumeratePreloads<Item::GobBow>(preloads);

        EnumeratePreloads<Character::GobboChief>(preloads);

        EnumeratePreloads<Character::GobboTower>(preloads);

        EnumeratePreloads<Character::GobboBramble>(preloads);

        EnumeratePreloads<Character::OrkWarrior>(preloads);
        EnumeratePreloads<Item::OrkChopper>(preloads);
        EnumeratePreloads<Item::OrkClub>(preloads);
        EnumeratePreloads<Item::OrkHook>(preloads);
        EnumeratePreloads<Item::OrkMace>(preloads);

        EnumeratePreloads<Character::OrkCannoneer>(preloads);

        EnumeratePreloads<Character::ShieldOrk>(preloads);

        EnumeratePreloads<Character::OrkLord>(preloads);

        EnumeratePreloads<Character::MrBonesMelee>(preloads);
        EnumeratePreloads<Item::SkelAxe>(preloads);
        EnumeratePreloads<Item::SkelDagger>(preloads);
        EnumeratePreloads<Item::SkelHalberd>(preloads);
        EnumeratePreloads<Item::SkelScimitar>(preloads);

        EnumeratePreloads<Character::MrBonesArcher>(preloads);
        EnumeratePreloads<Item::SkelBow>(preloads);

        EnumeratePreloads<Character::Gnome>(preloads);
        EnumeratePreloads<Item::GnomeWrench>(preloads);

        EnumeratePreloads<Character::Dummy>(preloads);

        EnumeratePreloads<Character::IronBomb>(preloads);
        EnumeratePreloads<Character::Dynamite>(preloads);

        EnumeratePreloads<Character::GoldNode>(preloads);
    }
}
