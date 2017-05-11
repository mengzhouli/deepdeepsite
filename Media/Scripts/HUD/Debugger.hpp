#pragma once

#include "Input/FrameInput.hpp"
#include "Input/InputBinding.hpp"
#include "Settings/Profile.hpp"
#include "Localization/StringTable.hpp"

#include "XML/XMLNode.hpp"

#include <vector>
#include <memory>
#include <functional>
#include <map>

namespace Dwarf
{
    namespace HUD
    {
        class DebuggerElemement
        {
        public:
            virtual ~DebuggerElemement();

            virtual void ReadFromXML(const XML::XMLNode& node);
            virtual void WriteToXML(XML::XMLNode node);

            virtual bool Update(double totalTime, float dt);
        };

        class BindableDebuggerElemement : public DebuggerElemement
        {
        public:
            virtual ~BindableDebuggerElemement();

            virtual void BeginExecute();
            virtual void EndExecute();

            virtual std::string GetTitle() const = 0;
        };

        using BindableDebuggerElemementFactory = std::function<BindableDebuggerElemement*()>;

        class Debugger final
        {
        public:
            Debugger(const Settings::Profile* profile, const Localization::StringTable* strings, const std::vector<Input::InputBindCode>& debugBinds);
            ~Debugger();

            void SetVisible(bool visible);
            bool IsVisible() const;

            void AddElement(const std::string& catagory, const std::string& title, std::shared_ptr<DebuggerElemement> element);
            void AddBindableElement(const std::string& title, BindableDebuggerElemementFactory elementFactory);

            bool IsMouseOver() const;

            void Update(double totalTime, float dt, const Input::FrameInput& input);

        private:
            bool _firstUpdate = true;
            bool _visible = false;
            bool _mouseOver = false;

            void save();
            void load();

            struct boundElement
            {
                Input::InputBindCode bind = Input::InputBindCode_Invalid;
                std::string name = "";
                int selectedElement = -1;
                bool executing = false;
                std::shared_ptr<BindableDebuggerElemement> element = nullptr;
            };
            std::vector<boundElement> _boundElements;

            std::vector<std::pair<std::string, std::function<BindableDebuggerElemement*()>>> _bindableElements;
            std::map<std::string, std::vector<std::pair<std::string, std::shared_ptr<DebuggerElemement>>>> _elements;

            std::string _settingsFile;
        };
    }
}
