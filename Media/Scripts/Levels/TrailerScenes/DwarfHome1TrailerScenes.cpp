#include "Levels/TrailerScenes/TrailerScenes.hpp"

#include "Levels/BasicLevel.hpp"
#include "CutsceneUtility.hpp"

#include "Characters/Dwarves/NavigatorDwarf.hpp"
#include "Items/Weapons/Gun.hpp"
#include "Items/Trinkets/Headlamp.hpp"

namespace Dwarf
{
    namespace Level
    {
        static const std::string LevelPath = "Levels/dwarfhomes_1.lvl";

        enum DwarfHome1TrailerScene
        {
            KitchenLookaround,
        };

        class DwarfHome1TrailerScenes : public BasicLevel
        {
        public:
            DwarfHome1TrailerScenes(const LevelParameters& parameters, DwarfHome1TrailerScene scene)
                : BasicLevel(parameters)
                , _scene(scene)
            {
            }

        protected:
            void OnCreate() override
            {
                BasicLevel::OnCreate();

                SetTargetCameraViewSize(2500.0f, 16.0f / 9.0f);

                switch (_scene)
                {
                case KitchenLookaround:
                    setupKitchenLookaroundScene();
                    break;
                }
            }

        private:
            void setupKitchenLookaroundScene()
            {
                auto primaryLayer = GetPrimaryLayer();

                auto navigator = primaryLayer->SpawnCharacter(primaryLayer->GetTriggerPosition("kitchen_trailer_navigator_spawn"), "nav", nullptr, Character::BindCharacterConstructor<Character::NavigatorDwarf>());
                navigator->GiveItem(Item::BindItemConstructor<Item::BasicPistol>());
                navigator->GiveItem(Item::BindItemConstructor<Item::BasicHeadlamp>());

                std::vector<std::shared_ptr<CutsceneEvent>> events
                {
                    std::make_shared<SimultaneousEvents>(std::vector<std::shared_ptr<CutsceneEvent>>
                    {
                        std::make_shared<PanCameraEvent>(primaryLayer, "kitchen_trailer_camera_spline", 10.0f),
                        std::make_shared<SeriesEvent>(std::vector<std::shared_ptr<CutsceneEvent>>
                        {
                            std::make_shared<CharactersLookEvent>(primaryLayer, std::vector<Character::CharacterID>{ navigator->GetID() }, primaryLayer->GetSpline("kitchen_trailer_navigator_look_spline").At(0), 2.0f),
                            std::make_shared<CharactersLookEvent>(primaryLayer, std::vector<Character::CharacterID>{ navigator->GetID() }, primaryLayer->GetSpline("kitchen_trailer_navigator_look_spline"), 4.0f),
                        }),
                    }),
                };
                PlayCutscene(std::make_shared<Cutscene>(events));
            }

            DwarfHome1TrailerScene _scene;
        };

        GameState::LevelConstructor<> CreateKitchenLookaroundScene()
        {
            return GameState::BindLevelConstructor<DwarfHome1TrailerScenes>(LevelPath, KitchenLookaround);
        }
    }

    template <>
    void EnumeratePreloads<Level::DwarfHome1TrailerScenes>(PreloadSet& preloads)
    {
        EnumeratePreloads<Level::BasicLevel>(preloads);
        preloads.insert(Level::LevelPath);

        EnumeratePreloads<Character::NavigatorDwarf>(preloads);
        EnumeratePreloads<Item::BasicHeadlamp>(preloads);
        EnumeratePreloads<Item::BasicPistol>(preloads);
    }
}
