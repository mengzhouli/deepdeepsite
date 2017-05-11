#pragma once

#include "Levels/BasicLevel.hpp"

namespace Dwarf
{
    namespace Level
    {
        class VideoTunnelLevel : public BasicLevel
        {
        public:
            VideoTunnelLevel(const LevelParameters& parameters);

        protected:
            void OnCreate() override;
            void OnUpdate(double totalTime, float dt) override;
            void OnDraw(LevelLayerInstance* layer, Graphics::LevelRenderer* levelRenderer) const override;

        private:
            Vector2f _pathStart;
            Vector2f _pathEnd;
            std::vector<Character::CharacterID> _dwarves;

            uint32_t _loopCounter = 0;
            float _loopTime = 0;

            uint32_t _frame = 0;
        };
    }

    template <>
    void EnumeratePreloads<Level::VideoTunnelLevel>(PreloadSet& preloads);
}
