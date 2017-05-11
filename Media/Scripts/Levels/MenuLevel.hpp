#pragma once

#include "Levels/BasicLevel.hpp"
#include "Levels/GameLevels.hpp"
#include "Levels/CampaignLevel.hpp"
#include "GameState/LevelGameState.hpp"

namespace Dwarf
{
    enum MenuState
    {
        Menu_Title,
        Menu_Options,
        Menu_Campaign,
        Menu_Challenges,
        Menu_Exit,
    };

    typedef std::function<GameState::LevelConstructor<>(const Level::CampaignLevelInfo& campaignLevelInfo, Level::CampaignLevelConstructor<> campaignLevel)> CampaignLoadoutConstructor;
    typedef std::function<GameState::LevelConstructor<>()> ChallengeLoadoutConstructor;

    namespace Level
    {
        class MenuLevel : public BasicLevel
        {
        public:
            MenuLevel(const LevelParameters& parameters, CampaignLoadoutConstructor campaignLoadoutConstructor, ChallengeLoadoutConstructor challengeLoadoutConstructor,
                      LevelChangeFunction levelChangeFunc, ExitGameFunction exitGameFunc, MenuState initialState);

            void PlayOpeningSwell();

        protected:
            virtual void OnCreate() override;
            virtual void OnUpdate(double totalTime, float dt) override;
            virtual void OnDraw(LevelLayerInstance* layer, Graphics::LevelRenderer* levelRenderer) const override;

        private:
            std::vector<Color> _lightColors;
            std::vector<Lights::SpotLight> _danceLights;

            bool _firstUpdate;
            std::vector<Character::Character*> _fighters;
            std::vector<float> _fighterActionTimers;

            std::vector<Character::Character*> _dummies;

            MenuState _initialState;
            LevelChangeFunction _levelChangeFunction;
            CampaignLoadoutConstructor _campaignLoadoutConstructor;
            ChallengeLoadoutConstructor _challengeLoadoutConstructor;

            ExitGameFunction _exitGameFunc;
        };
    }

    template <>
    void EnumeratePreloads<Level::MenuLevel>(PreloadSet& preloads);
}
