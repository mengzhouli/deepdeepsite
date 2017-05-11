#pragma once

#include "HUD/Debugger.hpp"
#include "Item/ItemTypes.hpp"
#include "Character/Character.hpp"
#include "Content/ContentManager.hpp"
#include "Level/LevelTypes.hpp"
#include "Ability/AbilityTypes.hpp"
#include "Content/Preload.hpp"

#include "Audio/ReverbEffect.hpp"

#include "RainEffect.hpp"

#include <functional>

namespace Dwarf
{
    namespace HUD
    {
        class FloatSliderDebuggerElement : public DebuggerElemement
        {
        public:
            using GetValueFunction = std::function<float()>;
            using OnValueChangedFunction = std::function<void(float)>;

            FloatSliderDebuggerElement(const std::string &title, float min, float max, GetValueFunction getValueFunc, OnValueChangedFunction onValueChangedFunc);

            void ReadFromXML(const XML::XMLNode& node) override;
            void WriteToXML(XML::XMLNode node) override;

            bool Update(double totalTime, float dt) override;

        private:
            std::string _title = "";
            float _value = 0.0f;
            float _min = 0.0f;
            float _max = 1.0f;
            GetValueFunction _getValue = nullptr;
            OnValueChangedFunction _onValueChanged = nullptr;
        };

        class CheckboxDebuggerElement : public DebuggerElemement
        {
        public:
            using GetValueFunction = std::function<bool()>;
            using OnValueChangedFunction = std::function<void(bool)>;

            CheckboxDebuggerElement(const std::string &title, GetValueFunction getValueFunc, OnValueChangedFunction onValueChangedFunc);

            void ReadFromXML(const XML::XMLNode& node) override;
            void WriteToXML(XML::XMLNode node) override;

            bool Update(double totalTime, float dt) override;

        private:
            std::string _title = "";
            bool _value = false;
            GetValueFunction _getValue = nullptr;
            OnValueChangedFunction _onValueChanged = nullptr;
        };

        class ComboboxDebuggerElement : public DebuggerElemement
        {
        public:
            using GetValuesFunction = std::function<std::pair<std::vector<std::string>, int32_t>()>;
            using OnValueChangedFunction = std::function<void(int32_t)>;

            ComboboxDebuggerElement(const std::string &title, bool saveValues, GetValuesFunction getValueFunc, OnValueChangedFunction onValueChangedFunc);

            void ReadFromXML(const XML::XMLNode& node) override;
            void WriteToXML(XML::XMLNode node) override;

            bool Update(double totalTime, float dt) override;

        private:
            std::string _title = "";
            bool _saveValues = true;
            int _curItem = 0;
            std::vector<std::string> _items;
            GetValuesFunction _getValue = nullptr;
            OnValueChangedFunction _onValueChanged = nullptr;
        };

        class ReverbEffectDebuggerElement : public DebuggerElemement
        {
        public:
            ReverbEffectDebuggerElement(const std::string& title, bool saveValues, Audio::ReverbEffect* effect);

            void ReadFromXML(const XML::XMLNode& node) override;
            void WriteToXML(XML::XMLNode node) override;

            bool Update(double totalTime, float dt) override;

        private:
            std::string _title = "";
            bool _saveValues = true;
            Audio::ReverbEffectParameters _originalParams = Audio::ReverbEffectParameters::Default_Generic;
            Audio::ReverbEffect* _effect = nullptr;
            std::vector<const char*> _presetNames;
        };

        class RainEffectDebuggerElement : public DebuggerElemement
        {
        public:
            RainEffectDebuggerElement(RainEffect* effect);

            void ReadFromXML(const XML::XMLNode& node) override;
            void WriteToXML(XML::XMLNode node) override;

            bool Update(double totalTime, float dt) override;

        private:
            RainEffect* _effect = nullptr;
        };

        using GetSpawnPointFunction = std::function<Vector2f()>;

        struct SpawnCharacterOption
        {
            std::pair<std::string, Character::CharacterConstructor<>> Constructor;
            std::vector<std::vector<std::pair<std::string, Item::ItemConstructor<Item::Weapon>>>> WeaponOptions;
            std::vector<std::pair<std::string, Item::ItemConstructor<Item::Armor>>> ArmorOptions;
            std::vector<std::pair<std::string, Item::ItemConstructor<Item::Trinket>>> TrinketOptions;
            std::vector<std::vector<std::pair<std::string, Ability::AbilityConstructor<>>>> AbilityOptions;
        };
        BindableDebuggerElemementFactory CreateCharacterSpawnerElement(
            Content::ContentManager* cm, Level::LevelLayerInstance* layer, GetSpawnPointFunction getSpwanPosFunc, std::vector<SpawnCharacterOption> options);

        BindableDebuggerElemementFactory CreateStandardCharacterSpawnerElement(
            Content::ContentManager* cm, Level::LevelLayerInstance* layer, GetSpawnPointFunction getSpwanPosFunc);

        BindableDebuggerElemementFactory CreateImpulseElement(Level::LevelLayerInstance* layer, GetSpawnPointFunction getSpwanPosFunc);

        BindableDebuggerElemementFactory CreatePlaySoundElement(Level::LevelLayerInstance* layer, GetSpawnPointFunction getSpwanPosFunc, Content::ContentManager* cm);
    }

    void EnumerateCreateStandardCharacterSpawnerElementPreloads(PreloadSet& preloads);
}
