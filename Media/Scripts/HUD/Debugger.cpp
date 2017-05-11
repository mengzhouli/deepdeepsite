#include "HUD/Debugger.hpp"

#include "HUD/Tooltip.hpp"

#include <imgui.h>

namespace Dwarf
{
    namespace HUD
    {
        DebuggerElemement::~DebuggerElemement()
        {
        }

        void DebuggerElemement::ReadFromXML(const XML::XMLNode& node)
        {
        }

        void DebuggerElemement::WriteToXML(XML::XMLNode node)
        {
        }

        bool DebuggerElemement::Update(double totalTime, float dt)
        {
            return false;
        }

        BindableDebuggerElemement::~BindableDebuggerElemement()
        {
        }

        void BindableDebuggerElemement::BeginExecute()
        {
        }

        void BindableDebuggerElemement::EndExecute()
        {
        }

        Debugger::Debugger(const Settings::Profile* profile, const Localization::StringTable* strings, const std::vector<Input::InputBindCode>& debugBinds)
        {
            for (uint32_t i = 0; i < debugBinds.size(); i++)
            {
                boundElement element;
                element.bind = debugBinds[i];

                element.name = Format("Debug %u", i + 1);

                const Input::InputBinding& bind = profile->GetBind(debugBinds[i]);
                if (bind.Type != Input::BindType_Unbound)
                {
                    std::string bindText = strings->GetString(Input::InputBinding::GetInputName(bind));
                    element.name += Format(" (%s)", bindText.c_str());
                }

                _boundElements.push_back(element);
            }

            _settingsFile = "DebuggerValues.xml";
        }

        Debugger::~Debugger()
        {
        }

        void Debugger::AddElement(const std::string& catagory, const std::string& title, std::shared_ptr<DebuggerElemement> element)
        {
            assert(element);
            _elements[catagory].push_back(std::make_pair(title, element));
        }

        void Debugger::AddBindableElement(const std::string& title, BindableDebuggerElemementFactory elementFactory)
        {
            _bindableElements.push_back(std::make_pair(title, elementFactory));
        }

        bool Debugger::IsMouseOver() const
        {
            return _mouseOver;
        }

        void Debugger::Update(double totalTime, float dt, const Input::FrameInput& input)
        {
            _mouseOver = false;

            if (!_visible)
            {
                return;
            }

            if (_firstUpdate)
            {
                load();

                _firstUpdate = false;
            }

            if (!ImGui::Begin("Debug"))
            {
                ImGui::End();
                return;
            }

            bool changed = false;

            for (auto& catagory : _elements)
            {
                if (ImGui::CollapsingHeader(catagory.first.c_str()))
                {
                    for (auto& element : catagory.second)
                    {
                        changed |= element.second->Update(totalTime, dt);
                    }
                }
            }

            std::vector<const char*> boundElementOptions(_bindableElements.size());
            for (uint32_t i = 0; i < _bindableElements.size(); i++)
            {
                boundElementOptions[i] = _bindableElements[i].first.c_str();
            }

            uint32_t boundItemIdx = 0;
            for (auto& element : _boundElements)
            {
                std::string title = element.name;
                if (element.element != nullptr)
                {
                    title += ": " + element.element->GetTitle();
                }
                title += Format("###boundItem%u", boundItemIdx++);

                if (ImGui::CollapsingHeader(title.c_str()))
                {
                    if (ImGui::Combo("Action", &element.selectedElement, boundElementOptions.data(), boundElementOptions.size()))
                    {
                        element.element = nullptr;
                        if (element.selectedElement >= 0)
                        {
                            if (element.executing)
                            {
                                element.element->EndExecute();
                                element.executing = false;
                            }
                            element.element.reset(_bindableElements[element.selectedElement].second());
                        }

                        changed = true;
                    }

                    if (element.element)
                    {
                        changed |= element.element->Update(totalTime, dt);
                    }
                }

                if (element.element)
                {
                    if (input.IsBindJustPressed(element.bind) && !element.executing)
                    {
                        element.element->BeginExecute();
                        element.executing = true;
                    }

                    if (input.IsBindJustReleased(element.bind) && element.executing)
                    {
                        element.element->EndExecute();
                        element.executing = false;
                    }
                }
            }

            ImGui::End();

            _mouseOver = ImGui::IsMouseHoveringWindow();

            if (changed)
            {
                save();
            }
        }

        void Debugger::SetVisible(bool visible)
        {
            _visible = visible;
        }

        bool Debugger::IsVisible() const
        {
            return _visible;
        }

        void Debugger::save()
        {
            XML::XMLDocument doc("Debug");

            XML::XMLNode root = doc.GetRoot();
            for (const auto& boundElement : _boundElements)
            {
                XML::XMLNode elementNode = root.AddChild("BoundElement");
                bool isBound = boundElement.selectedElement >= 0;
                elementNode.AddChild("Name").SetValue(isBound ? _bindableElements[boundElement.selectedElement].first : std::string());
                if (isBound)
                {
                    boundElement.element->WriteToXML(elementNode.AddChild("Value"));
                }
            }

            for (const auto& elementCatagory : _elements)
            {
                XML::XMLNode catagoryNode = root.AddChild("Catagory");
                catagoryNode.AddChild("Name").SetValue(elementCatagory.first);
                for (const auto& element : elementCatagory.second)
                {
                    XML::XMLNode elementNode = catagoryNode.AddChild("Element");
                    elementNode.AddChild("Name").SetValue(element.first);
                    element.second->WriteToXML(elementNode.AddChild("Value"));
                }
            }

            doc.WriteToFile(_settingsFile);
        }

        void Debugger::load()
        {
            try
            {
                XML::XMLDocument doc = XML::XMLDocument::ReadFromFile(_settingsFile);

                XML::XMLNode root = doc.GetRoot();

                uint32_t curBoundElement = 0;
                for (XML::XMLNode elementNode = root.GetChild("BoundElement");
                    elementNode && curBoundElement < _boundElements.size();
                    elementNode = elementNode.GetNextSibling("BoundElement"), curBoundElement++)
                {
                    std::string elementType = elementNode.GetChild("Name").GetValue<std::string>();
                    for (uint32_t i = 0; i < _bindableElements.size(); i++)
                    {
                        if (_bindableElements[i].first == elementType)
                        {
                            _boundElements[curBoundElement].selectedElement = i;
                            _boundElements[curBoundElement].element.reset(_bindableElements[i].second());
                            _boundElements[curBoundElement].element->ReadFromXML(elementNode.GetChild("Value"));
                            break;
                        }
                    }
                }

                for (XML::XMLNode catagoryNode = root.GetChild("Catagory"); catagoryNode; catagoryNode = catagoryNode.GetNextSibling("Catagory"))
                {
                    std::string catagoryName = catagoryNode.GetChild("Name").GetValue<std::string>();
                    for (auto& catagory : _elements)
                    {
                        if (catagory.first == catagoryName)
                        {
                            for (XML::XMLNode elementNode = catagoryNode.GetChild("Element"); elementNode; elementNode = elementNode.GetNextSibling("Element"))
                            {
                                std::string elementName = elementNode.GetChild("Name").GetValue<std::string>();
                                for (auto& element : catagory.second)
                                {
                                    if (element.first == elementName)
                                    {
                                        element.second->ReadFromXML(elementNode.GetChild("Value"));
                                        break;
                                    }
                                }
                            }

                            break;
                        }
                    }
                }
            }
            catch (const Exception& e)
            {
                LogError("Debugger", Format("Failed to load debugger settings: %s.", e.GetErrorMessage().c_str()));
            }
        }
    }
}
