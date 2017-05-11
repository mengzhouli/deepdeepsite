#pragma once

#include "Level/Cutscene.hpp"
#include "Level/LevelTypes.hpp"
#include "Character/CharacterTypes.hpp"
#include "Geometry/Spline.hpp"

#include <functional>

namespace Dwarf
{
    namespace Character
    {
        class BasicCharacter;
    }

    namespace Level
    {
        class TimerEvent : public CutsceneEvent
        {
        public:
            TimerEvent(float timer);

            virtual bool IsFinished() const override;
            virtual void Update(double totalTime, float dt) override;

        protected:
            float GetPercentComplete() const;

        private:
            const float _duration;
            float _timer;
        };

        class FocusCameraEvent : public TimerEvent
        {
        public:
            FocusCameraEvent(LevelInstance* level, const Vector2f& target, float moveTime);
            FocusCameraEvent(const LevelLayerInstance* layer, const std::string& targetName, float moveTime);

            virtual void Start() override;

        private:
            LevelInstance* _level;
            const Vector2f _target;
            const float _moveTime;
        };

        class PanCameraEvent : public TimerEvent
        {
        public:
            PanCameraEvent(LevelInstance* level, const Splinef& spline, float moveTime);
            PanCameraEvent(const LevelLayerInstance* layer, const std::string& splineName, float moveTime);

            void Start() override;

        private:
            LevelInstance* _level;
            const Splinef _spline;
            const float _moveTime;
        };

        class ZoomCameraEvent : public TimerEvent
        {
        public:
            ZoomCameraEvent(LevelInstance* level, float zoom, float moveTime);
            ZoomCameraEvent(LevelInstance* level, float zoom, const Vector2f& zoomFocus, float moveTime);

            void Start() override;

        private:
            LevelInstance* _level;
            const float _zoom;
            const Vector2f _zoomFocus;
            bool _focus;
            const float _moveTime;
        };

        class MoveCharacterEvent : public CutsceneEvent
        {
        public:
            MoveCharacterEvent(const LevelLayerInstance* layer, Character::CharacterID character, const Vector2f& target);
            MoveCharacterEvent(const LevelLayerInstance* layer, Character::CharacterID character, const std::string& targetName);

            virtual void Start() override;
            virtual bool IsFinished() const override;

        private:
            const LevelLayerInstance* _layer;
            const Character::CharacterID _character;
            const Vector2f _target;
        };

        class InteractCharacterEvent : public CutsceneEvent
        {
        public:
            InteractCharacterEvent(const LevelLayerInstance* layer, Character::CharacterID character, Character::CharacterID target);

            virtual void Start() override;
            virtual bool IsFinished() const override;

        private:
            const LevelLayerInstance* _layer;
            const Character::CharacterID _character;
            const Character::CharacterID  _target;
        };

        class CharactersLookEvent : public TimerEvent
        {
        public:
            CharactersLookEvent(const LevelLayerInstance* layer, std::vector<Character::CharacterID> characters, const Vector2f& target, float duration);
            CharactersLookEvent(const LevelLayerInstance* layer, std::vector<Character::CharacterID> characters, const Splinef& target, float duration);
            CharactersLookEvent(const LevelLayerInstance* layer, std::vector<Character::CharacterID> characters, Character::CharacterID target, float duration);
            CharactersLookEvent(std::function<std::vector<Character::BasicCharacter*>()> getCharactersFunc, std::function<Vector2f()> getTargetFunc, float duration);

            virtual void Update(double totalTime, float dt) override;

        private:
            const std::function<std::vector<Character::BasicCharacter*>()> _getCharactersFunc;
            const std::function<Vector2f(float)> _getTargetFunc;
        };

        class FunctionalEvent : public CutsceneEvent
        {
        public:
            FunctionalEvent(std::function<void(void)> startFunc, std::function<bool(void)> finishedFunc, std::function<void(double, float)> updateFunc,
                            std::function<void(LevelLayerInstance*, Graphics::LevelRenderer*)> drawFunc);

            virtual void Start() override;
            virtual bool IsFinished() const override;
            virtual void Update(double totalTime, float dt) override;
            virtual void Draw(LevelLayerInstance* layer, Graphics::LevelRenderer* levelRenderer) const override;

        private:
            std::function<void(void)> _startFunc;
            std::function<bool(void)> _finishedFunc;
            std::function<void(double, float)> _updateFunc;
            std::function<void(LevelLayerInstance*, Graphics::LevelRenderer*)> _drawFunc;
        };

        class TimedFunctionalEvent : public TimerEvent
        {
        public:
            TimedFunctionalEvent(float duration, std::function<void(void)> startFunc, std::function<void(double, float)> updateFunc, std::function<void(LevelLayerInstance*, Graphics::LevelRenderer*)> drawFunc);

            virtual void Start() override;
            virtual void Update(double totalTime, float dt) override;
            virtual void Draw(LevelLayerInstance* layer, Graphics::LevelRenderer* levelRenderer) const override;

        private:
            std::function<void(void)> _startFunc;
            std::function<void(double, float)> _updateFunc;
            std::function<void(LevelLayerInstance*, Graphics::LevelRenderer*)> _drawFunc;
        };

        class SimultaneousEvents : public CutsceneEvent
        {
        public:
            SimultaneousEvents(const std::vector<std::shared_ptr<CutsceneEvent>>& events);

            virtual void Start() override;
            virtual bool IsFinished() const override;
            virtual void Update(double totalTime, float dt) override;
            virtual void Draw(LevelLayerInstance* layer, Graphics::LevelRenderer* levelRenderer) const override;

        private:
            std::vector<std::shared_ptr<CutsceneEvent>> _events;
        };

        class SeriesEvent : public CutsceneEvent
        {
        public:
            SeriesEvent(const std::vector<std::shared_ptr<CutsceneEvent>>& events);

            virtual void Start() override;
            virtual bool IsFinished() const override;
            virtual void Update(double totalTime, float dt) override;
            virtual void Draw(LevelLayerInstance* layer, Graphics::LevelRenderer* levelRenderer) const override;

        private:
            std::vector<std::shared_ptr<CutsceneEvent>> _events;
        };
    }
}
