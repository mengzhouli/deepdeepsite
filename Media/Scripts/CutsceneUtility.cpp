#include "CutsceneUtility.hpp"

#include "Level/LevelInstance.hpp"
#include "Level/LevelLayerInstance.hpp"
#include "Character/Character.hpp"

#include "Characters/BasicCharacter.hpp"

namespace Dwarf
{
    namespace Level
    {
        TimerEvent::TimerEvent(float timer)
            : _timer(timer)
            , _duration(timer)
        {
        }

        bool TimerEvent::IsFinished() const
        {
            return _timer < 0.0f;
        }

        void TimerEvent::Update(double totalTime, float dt)
        {
            _timer -= dt;
        }

        float TimerEvent::GetPercentComplete() const
        {
            return 1.0f - Saturate(_timer / _duration);
        }

        void FunctionalEvent::Draw(LevelLayerInstance* layer, Graphics::LevelRenderer* levelRenderer) const
        {
            if (_drawFunc)
            {
                _drawFunc(layer, levelRenderer);
            }
        }

        FocusCameraEvent::FocusCameraEvent(LevelInstance* level, const Vector2f& target, float moveTime)
            : TimerEvent(moveTime)
            , _level(level)
            , _target(target)
            , _moveTime(moveTime)
        {

        }

        FocusCameraEvent::FocusCameraEvent(const LevelLayerInstance* layer, const std::string& targetName, float moveTime)
            : TimerEvent(moveTime)
            , _level(layer->GetLevel())
            , _target(layer->GetTriggerPosition(targetName))
            , _moveTime(moveTime)
        {
        }

        void FocusCameraEvent::Start()
        {
            _level->GetCameraController().Pan(_target, _moveTime);
        }

        PanCameraEvent::PanCameraEvent(LevelInstance* level, const Splinef& spline, float moveTime)
            : TimerEvent(moveTime)
            , _level(level)
            , _spline(spline)
            , _moveTime(moveTime)
        {
        }

        PanCameraEvent::PanCameraEvent(const LevelLayerInstance* layer, const std::string& splineName, float moveTime)
            : PanCameraEvent(layer->GetLevel(), layer->GetSpline(splineName), moveTime)
        {
        }

        void PanCameraEvent::Start()
        {
            _level->GetCameraController().FollowSpline(_spline, _moveTime);
        }

        ZoomCameraEvent::ZoomCameraEvent(LevelInstance* level, float zoom, float moveTime)
            : TimerEvent(moveTime)
            , _level(level)
            , _zoom(zoom)
            , _focus(false)
            , _moveTime(moveTime)
        {
        }

        ZoomCameraEvent::ZoomCameraEvent(LevelInstance* level, float zoom, const Vector2f& zoomFocus, float moveTime)
            : TimerEvent(moveTime)
            , _level(level)
            , _zoom(zoom)
            , _zoomFocus(zoomFocus)
            , _focus(false)
            , _moveTime(moveTime)
        {
        }

        void ZoomCameraEvent::Start()
        {
            if (_focus)
            {
                _level->GetCameraController().ZoomOnTarget(_zoom, _zoomFocus, _moveTime);
            }
            else
            {
                _level->GetCameraController().Zoom(_zoom, _moveTime);
            }
        }

        MoveCharacterEvent::MoveCharacterEvent(const LevelLayerInstance* layer, Character::CharacterID character, const Vector2f& target)
            : _layer(layer)
            , _character(character)
            , _target(target)
        {
        }

        MoveCharacterEvent::MoveCharacterEvent(const LevelLayerInstance* layer, Character::CharacterID character, const std::string& targetName)
            : _layer(layer)
            , _character(character)
            , _target(layer->GetTriggerPosition(targetName))
        {
        }

        void MoveCharacterEvent::Start()
        {
            Character::Character* character = _layer->GetCharacter<>(_character);
            character->PushAction(Character::CreateMoveAction(_target), false);
        }

        bool MoveCharacterEvent::IsFinished() const
        {
            Character::Character* character = _layer->GetCharacter<>(_character);
            return !character->IsMoving();
        }

        InteractCharacterEvent::InteractCharacterEvent(const LevelLayerInstance* layer, Character::CharacterID character, Character::CharacterID target)
            : _layer(layer)
            , _character(character)
            , _target(target)
        {
        }

        void InteractCharacterEvent::Start()
        {
            Character::Character* character = _layer->GetCharacter<>(_character);
            character->PushAction(Character::CreateInteractCharacterAction(_target), false);
        }

        bool InteractCharacterEvent::IsFinished() const
        {
            Character::Character* character = _layer->GetCharacter<>(_character);
            return character->GetCurrentAction().Type == Character::Action::ActionType_None;
        }

        FunctionalEvent::FunctionalEvent(std::function<void(void)> startFunc, std::function<bool(void)> finishedFunc, std::function<void(double, float)> updateFunc,
                                         std::function<void(LevelLayerInstance*, Graphics::LevelRenderer*)> drawFunc)
            : _startFunc(startFunc)
            , _finishedFunc(finishedFunc)
            , _updateFunc(updateFunc)
            , _drawFunc(drawFunc)
        {
        }

        void FunctionalEvent::Start()
        {
            if (_startFunc)
            {
                _startFunc();
            }
        }

        bool FunctionalEvent::IsFinished() const
        {
            if (_finishedFunc)
            {
                return _finishedFunc();
            }
            else
            {
                return true;
            }
        }

        void FunctionalEvent::Update(double totalTime, float dt)
        {
            if (_updateFunc)
            {
                _updateFunc(totalTime, dt);
            }
        }

        SimultaneousEvents::SimultaneousEvents(const std::vector<std::shared_ptr<CutsceneEvent>>& events)
            : _events(events)
        {
        }

        void SimultaneousEvents::Start()
        {
            for (auto& event : _events)
            {
                event->Start();
            }
        }

        bool SimultaneousEvents::IsFinished() const
        {
            for (auto& event : _events)
            {
                if (!event->IsFinished())
                {
                    return false;
                }
            }

            return true;
        }

        void SimultaneousEvents::Update(double totalTime, float dt)
        {
            for (auto& event : _events)
            {
                if (!event->IsFinished())
                {
                    event->Update(totalTime, dt);
                }
            }
        }

        void SimultaneousEvents::Draw(LevelLayerInstance* layer, Graphics::LevelRenderer* levelRenderer) const
        {
            for (const auto& event : _events)
            {
                if (!event->IsFinished())
                {
                    event->Draw(layer, levelRenderer);
                }
            }
        }

        CharactersLookEvent::CharactersLookEvent(const LevelLayerInstance* layer, std::vector<Character::CharacterID> characters, const Vector2f& target, float duration)
            : TimerEvent(duration)
            , _getCharactersFunc([=]()
                {
                    std::vector<Character::BasicCharacter*> result;
                    for (auto characterID : characters)
                    {
                        auto character = layer->GetCharacter<Character::BasicCharacter>(characterID);
                        if (character != nullptr)
                        {
                            result.push_back(character);
                        }
                    }
                    return result;
                })
            , _getTargetFunc([=](float){ return target; })
        {
        }

        CharactersLookEvent::CharactersLookEvent(const LevelLayerInstance* layer, std::vector<Character::CharacterID> characters, const Splinef& target, float duration)
            : TimerEvent(duration)
            , _getCharactersFunc([=]()
                {
                    std::vector<Character::BasicCharacter*> result;
                    for (auto characterID : characters)
                    {
                        auto character = layer->GetCharacter<Character::BasicCharacter>(characterID);
                        if (character != nullptr)
                        {
                            result.push_back(character);
                        }
                    }
                    return result;
                })
            , _getTargetFunc([=](float perc){ return target.Evalulate(perc); })
        {
        }

        CharactersLookEvent::CharactersLookEvent(const LevelLayerInstance* layer, std::vector<Character::CharacterID> characters, Character::CharacterID target, float duration)
            : TimerEvent(duration)
            , _getCharactersFunc([=]()
                {
                    std::vector<Character::BasicCharacter*> result;
                    for (auto characterID : characters)
                    {
                        auto character = layer->GetCharacter<Character::BasicCharacter>(characterID);
                        if (character != nullptr)
                        {
                            result.push_back(character);
                        }
                    }
                    return result;
                })
            , _getTargetFunc([=](float)
                {
                    auto character = layer->GetCharacter(target);
                    if (character != nullptr)
                    {
                        return character->GetBounds().Middle();
                    }
                    else
                    {
                        return Vector2f();
                    }
                })
        {
        }

        CharactersLookEvent::CharactersLookEvent(std::function<std::vector<Character::BasicCharacter*>()> getCharactersFunc, std::function<Vector2f()> getTargetFunc, float duration)
            : TimerEvent(duration)
            , _getCharactersFunc(getCharactersFunc)
            , _getTargetFunc([=](float) { return getTargetFunc(); })
        {
        }

        void CharactersLookEvent::Update(double totalTime, float dt)
        {
            TimerEvent::Update(totalTime, dt);

            Vector2f lookTarget = _getTargetFunc(GetPercentComplete());
            for (auto& character : _getCharactersFunc())
            {
                character->SetRotation(Rotatorf(lookTarget - character->GetPosition()));
                character->LookAt(lookTarget);
            }
        }

        TimedFunctionalEvent::TimedFunctionalEvent(float duration, std::function<void(void)> startFunc, std::function<void(double, float)> updateFunc, std::function<void(LevelLayerInstance*, Graphics::LevelRenderer*)> drawFunc)
            : TimerEvent(duration)
            , _startFunc(startFunc)
            , _updateFunc(updateFunc)
            , _drawFunc(drawFunc)
        {
        }

        void TimedFunctionalEvent::Start()
        {
            TimerEvent::Start();

            if (_startFunc != nullptr)
            {
                _startFunc();
            }
        }

        void TimedFunctionalEvent::Update(double totalTime, float dt)
        {
            TimerEvent::Update(totalTime, dt);

            if (_updateFunc != nullptr)
            {
                _updateFunc(totalTime, dt);
            }
        }

        void TimedFunctionalEvent::Draw(LevelLayerInstance* layer, Graphics::LevelRenderer* levelRenderer) const
        {
            TimerEvent::Draw(layer, levelRenderer);

            if (_drawFunc != nullptr)
            {
                _drawFunc(layer, levelRenderer);
            }
        }

        SeriesEvent::SeriesEvent(const std::vector<std::shared_ptr<CutsceneEvent>>& events)
            : _events(events)
        {
        }

        void SeriesEvent::Start()
        {
            if (!_events.empty())
            {
                _events.front()->Start();
            }
        }

        bool SeriesEvent::IsFinished() const
        {
            return _events.empty();
        }

        void SeriesEvent::Update(double totalTime, float dt)
        {
            if (!_events.empty())
            {
                _events.front()->Update(totalTime, dt);
                if (_events.front()->IsFinished())
                {
                    _events.erase(_events.begin());

                    if (!_events.empty())
                    {
                        _events.front()->Start();
                    }
                }
            }
        }

        void SeriesEvent::Draw(LevelLayerInstance* layer, Graphics::LevelRenderer* levelRenderer) const
        {
            if (!_events.empty())
            {
                _events.front()->Draw(layer, levelRenderer);
            }
        }
    }
}
