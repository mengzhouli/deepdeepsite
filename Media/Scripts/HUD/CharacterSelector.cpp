#include "HUD/CharacterSelector.hpp"

#include "Level/LevelInstance.hpp"

#include "HUD/Tooltip.hpp"

#include "MathUtility.hpp"

namespace Dwarf
{
    namespace HUD
    {
        static std::string getCharacterButtonKeybindText(const Character::Character* character)
        {
            if (character != nullptr && character->GetBind() != Input::InputBindCode_Invalid)
            {
                const Localization::StringTable* strings = character->GetLevel()->GetStringTable();
                const Settings::Profile* profile = character->GetLevel()->GetProfile();

                const Input::InputBinding& bind = profile->GetBind(character->GetBind());
                if (bind.Type != Input::BindType_Unbound)
                {
                    const std::string& inputNameCode = Input::InputBinding::GetInputName(bind);
                    const std::string& localizedName = strings->GetString(inputNameCode);
                    return Format("<color=white>%s</color>", localizedName.c_str());
                }
                else
                {
                    return "";
                }
            }
            else
            {
                return "";
            }

        }
        static const Color getHealthIconColor(const Character::Character* character)
        {
            if (character->IsAlive())
            {
                float perc = character->GetHealth().GetPercent();

                Color color;
                if (perc > 0.5f)
                {
                    color = Color::Lerp(Color::Yellow, Color::White, (perc - 0.5f) * 2.0f);
                }
                else
                {
                    color = Color::Lerp(Color::Red, Color::Yellow, perc * 2.0f);
                }

                return color;
            }
            else
            {
                return Color::White;
            }
        }

        static const Color getHealthBarColor(const Character::Health& health)
        {
            float perc = health.GetPercent();

            Color color;
            if (perc > 0.9f)
            {
                color = Color::Lerp(Color::Green, Color::Black, (perc - 0.9f) * 10.0f);
            }
            else if (perc > 0.5f)
            {
                color = Color::Lerp(Color::Yellow, Color::Green, (perc - 0.5f) * 2.0f);
            }
            else
            {
                color = Color::Lerp(Color::Red, Color::Yellow, perc * 2.0f);
            }

            return color;
        }

        static const float HealthDrawThreshold = 1.0f;
        class HealthPanelDrawable : public PanelDrawable
        {
        public:
            HealthPanelDrawable()
                : _health()
                , _isAlive(false)
            {
            }

            void SetHealth(const Character::Health& health, bool isAlive)
            {
                _health = health;
                _isAlive = isAlive;
            }

            virtual Vector2f GetSize(float scale, const Vector2f& maxSize) const override
            {
                return Vector2f(scale);
            }

            virtual void Draw(Graphics::SpriteRenderer* spriteRenderer, const Color& color, const RotatedRectanglef& bounds, float scale) const override
            {
                float perc = _health.GetPercent();
                if (_isAlive && perc < HealthDrawThreshold && perc > 0.0f)
                {
                    Color healthColor = getHealthBarColor(_health);

                    Rectanglef rect(bounds.TopLeft(), bounds.Extents * 2.0f);
                    //rect.Y += rect.H * 0.75f;
                    //rect.H *= 0.25f;
                    rect.W *= perc;

                    spriteRenderer->DrawSprite(rect, color * healthColor, bounds.R, Vector2f(0.5f));
                }
            }

        private:
            Character::Health _health;
            bool _isAlive;
        };

        class CharacterSelector::CharacterPile
        {
        public:
            struct characterInfo
            {
                ResourcePointer<const Character::Character> character;
                ResourcePointer<const Panel> tooltip;
            };

        private:
            ResourcePointer<ButtonPanel> _buttons;
            std::vector<ResourcePointer<HealthPanelDrawable>> _healthDrawables;

            Rectanglef _backgroundPanelDrawArea;
            ResourcePointer<Panel> _backgroundPanel;
            ResourcePointer<Panel> _backgroundInnerPannel;

            Rectanglef _area = Rectanglef();
            float _density = 0.0f;
            float _areaSum = 0.0f;

            Vector2u _buttonSize;
            ResourcePointer<const BorderPanelDrawable> _buttonBorder;
            ResourcePointer<const BorderPanelDrawable> _buttonCornerBorder;
            ResourcePointer<const PanelDrawable> _buttonHighlight;
            ResourcePointer<const Graphics::Font> _font;


            std::vector<characterInfo> _characters;

            bool _mouseOver = false;
            ResourcePointer<const Character::Character> _clickedCharacter;
            ResourcePointer<const Panel> _currentTooltip;

            ResourcePointer<Content::ContentManager> _contentManager;

            float getAreaSumWithoutCharacter(const Character::Character* skipCharacter) const
            {
                float areaSum = 0;
                for (auto character : _characters)
                {
                    if (character.character != skipCharacter)
                    {
                        const Rectanglef& bounds = character.character->GetBounds();
                        areaSum += bounds.Area();
                    }
                }

                return areaSum;
            }

        public:
            CharacterPile(Content::ContentManager* contentManager, const Vector2u& buttonSize, const BorderPanelDrawable* buttonBorder,
                const BorderPanelDrawable* buttonCornerBorder, const PanelDrawable* buttonHighlight, const Graphics::Font* font,
                Input::InputBindCode clickBind)
                : _buttonSize((buttonSize / 4u) * 3u)
                , _buttonBorder(buttonBorder)
                , _buttonCornerBorder(buttonCornerBorder)
                , _buttonHighlight(buttonHighlight)
                , _font(font)
                , _contentManager(contentManager)
            {
                assert(_buttonBorder);
                assert(_buttonHighlight);
                assert(_buttonCornerBorder);
                assert(_font);
                assert(_contentManager);

                _buttons = MakeResource<ButtonPanel>();
                _buttons->SetBorderMaterial(_buttonBorder);
                _buttons->SetCornerBorderMaterial(_buttonCornerBorder);
                _buttons->SetHighlightMaterial(_buttonHighlight);
                _buttons->SetClickBind(clickBind);

                ResourcePointer<BorderPanelDrawable> thinBorder = EmplaceResource(CreateThinBorder(contentManager));

                _backgroundInnerPannel = MakeResource<Panel>();

                _backgroundPanel = MakeResource<Panel>();
                _backgroundPanel->AddAlignedPanelDrawable(Alignment_BackgroundFill, thinBorder);
                _backgroundPanel->SetVariablePadding(thinBorder->GetPaddingFunction());
                _backgroundPanel->SetBackgroundColor(Color::Black);
                _backgroundPanel->AddAlignedPanel(Alignment_Fill, _backgroundInnerPannel);
            }

            virtual ~CharacterPile()
            {
            }

            uint32_t size() const
            {
                return _characters.size();
            }

            const Rectanglef& area() const
            {
                return _area;
            }

            float density() const
            {
                return _density;
            }

            float areaSum() const
            {
                return _areaSum;
            }

            const std::vector<characterInfo>& characters()
            {
                return _characters;
            }

            void addCharacter(const Character::Character* character)
            {
                auto charIter = std::find_if(_characters.begin(), _characters.end(), [&](const characterInfo& info) {return info.character == character; });
                assert(character != nullptr && charIter == _characters.end());

                characterInfo info;
                info.character = character;
                info.tooltip = CreateSimpleTextTooltip(_contentManager, character->GetName());

                _characters.push_back(info);
                _healthDrawables.push_back(new HealthPanelDrawable());
                updateArea();
            }

            void removeCharacter(const Character::Character* character)
            {
                assert(character != nullptr);

                auto charIter = std::find_if(_characters.begin(), _characters.end(), [&](const characterInfo& info) {return info.character == character; });
                assert(charIter != _characters.end());
                _characters.erase(charIter);

                _healthDrawables.pop_back();

                updateArea();
            }

            float getDensityWithoutCharacter(const Character::Character* skipCharacter) const
            {
                return (getAreaSumWithoutCharacter(skipCharacter)) / getAreaWithoutCharacter(skipCharacter).Area();
            }

            float getDensityWithCharacter(const Character::Character* newCharacter) const
            {
                return (areaSum() + newCharacter->GetBounds().Area()) / Rectanglef::Merge(_area, newCharacter->GetBounds()).Area();
            }

            Rectanglef getAreaWithoutCharacter(const Character::Character* skipCharacter) const
            {
                if (_characters.empty() || (_characters.size() == 1 && skipCharacter != nullptr))
                {
                    return Rectanglef();
                }
                else
                {
                    bool boundsValid = false;
                    Rectanglef bounds;

                    for (auto character : _characters)
                    {
                        if (character.character != skipCharacter)
                        {
                            if (boundsValid)
                            {
                                bounds = Rectanglef::Merge(bounds, character.character->GetBounds());
                            }
                            else
                            {
                                bounds = character.character->GetBounds();
                                boundsValid = true;
                            }
                        }
                    }

                    return bounds;
                }
            }

            static float getMergedPileDensity(std::shared_ptr<const CharacterSelector::CharacterPile> pileA, std::shared_ptr<const CharacterSelector::CharacterPile> pileB)
            {
                assert(pileA != nullptr && pileB != nullptr);
                return (pileA->areaSum() + pileB->areaSum()) / Rectanglef::Merge(pileA->area(), pileB->area()).Area();
            }

            void updateArea()
            {
                _area = getAreaWithoutCharacter(nullptr);
                _density = getDensityWithoutCharacter(nullptr);
                _areaSum = getAreaSumWithoutCharacter(nullptr);
            }

            bool IsMouseOver() const
            {
                return _mouseOver;
            }

            const Character::Character* GetClickedCharacter() const
            {
                return _clickedCharacter;
            }

            const Panel* GetTooltip() const
            {
                return _currentTooltip;
            }

            void updateButtons(const Input::FrameInput& input, const Camera& cam, const CharacterSet& selectedCharacters, float scale)
            {
                assert(_healthDrawables.size() == _characters.size());

                const uint32_t noSortSize = 3;
                if (_characters.size() <= noSortSize)
                {
                    std::stable_sort(_characters.begin(), _characters.end(), [](const characterInfo& a, const characterInfo& b)
                    {
                        return a.character->GetPosition().X < b.character->GetPosition().X;
                    });
                }

                const Camera& worldCamera = (_characters.front().character)->GetLevel()->GetCameraController().GetCamera();
                Vector2f projectedTopLeft = Camera::Transpose(worldCamera, _area.TopLeft(), cam);
                Vector2f projectedBottomRight =Camera::Transpose(worldCamera, _area.BottomRight(), cam);
                Rectanglef projectedArea(projectedTopLeft, projectedBottomRight - projectedTopLeft);

                Vector2f avgPosition;
                for (uint32_t i = 0; i < _characters.size(); i++)
                {
                    const Rectanglef& bounds = _characters[i].character->GetBounds();
                    const Vector2f& pos = _characters[i].character->GetPosition();
                    //avgPosition += Vector2f(bounds.Middle().X, bounds.Bottom());
                    avgPosition += Vector2f(pos.X, Max(pos.Y, bounds.Bottom()));
                }
                avgPosition *= (1.0f / _characters.size());
                Vector2f projectedAvgPosition = Camera::Transpose(worldCamera, avgPosition, cam);

                const uint32_t maxButtonsPerRow = 6;
                uint32_t buttonsPerRow = Min(maxButtonsPerRow, _characters.size());
                _buttons->SetButtonCount(_characters.size(), buttonsPerRow);
                for (uint32_t i = 0; i < _characters.size(); i++)
                {
                    _buttons->SetButtonIcon(i, _characters[i].character->GetIcon());
                    _buttons->ForceButtonHighlightOn(i, selectedCharacters.ContainsCharacter(_characters[i].character));
                    _buttons->SetButtonIconColor(i, getHealthIconColor(_characters[i].character));
                    _buttons->SetButtonBackground(i, _healthDrawables[i]);
                    _buttons->SetButtonCornerText(i, Alignment_TopLeft, _font, getCharacterButtonKeybindText(_characters[i].character));
                    _healthDrawables[i]->SetHealth(_characters[i].character->GetHealth(), _characters[i].character->IsAlive());
                }

                uint32_t columns = buttonsPerRow;
                uint32_t rows = (_characters.size() + buttonsPerRow - 1) / buttonsPerRow;

                Vector2f buttonsSize(columns * (_buttonSize.X * scale), rows * _buttonSize.Y * scale);
                _backgroundInnerPannel->SetWidth(buttonsSize.X);
                _backgroundInnerPannel->SetHeight(buttonsSize.Y);

                Vector2f minimumPanelSize = _backgroundPanel->GetMinimumSize(scale);
                _backgroundPanelDrawArea = Rectanglef(projectedAvgPosition.X - (minimumPanelSize.X * 0.5f), projectedAvgPosition.Y, minimumPanelSize.X, minimumPanelSize.Y);

                _buttons->SetBounds(_backgroundPanel->GetChildPanelArea(_backgroundInnerPannel, _backgroundPanelDrawArea, scale), scale);
                _buttons->Update(input, cam);

                _clickedCharacter = nullptr;
                uint32_t clickedButton = 0;
                if (_buttons->IsButtonJustClicked(clickedButton))
                {
                    _clickedCharacter = _characters[clickedButton].character;
                }

                uint32_t mouseOverButton = 0;
                if (_buttons->IsButtonMouseOver(mouseOverButton))
                {
                    _currentTooltip = _characters[mouseOverButton].tooltip;
                }

                Vector2f mousePosHUD = input.UnProjectMousePosition(cam);
                _mouseOver = Rectanglef::Contains(_backgroundPanelDrawArea, mousePosHUD);
            }

            void draw(Graphics::SpriteRenderer* spriteRenderer, const Color& color, float scale) const
            {
                _backgroundPanel->Draw(spriteRenderer, color, _backgroundPanelDrawArea, scale);
                _buttons->Draw(spriteRenderer, color);
            }
        };

        static const std::string FrameSkeletonPath = "Skeletons/UI/CharacterSelector/frame_mount.skel";
        static const std::string FrameMatsetPath = "Skeletons/UI/CharacterSelector/frame_mount.polymatset";

        static const std::string MoveInAnimation = "move_in";
        static const std::string MoveOutAnimation = "move_out";

        static const std::string FrameButtonTopJoint = "top";
        static const std::string FrameButtonBottomJoint = "bottom";
        static const std::string FrameButtonLeftJoint = "left";
        static const std::string FrameButtonRightJoint = "right";

        static const std::string FontPath = "Fonts/hud_font.ttf";

        CharacterSelector::CharacterSelector(Content::ContentManager* contentManager, Input::InputBindCode clickBind, Input::InputBindCode selectAllBind, CharacterStackLocation stackLocation)
            : _contentManager(contentManager)
            , _clickBind(clickBind)
            , _selectAllBind(selectAllBind)
            , _stackLocation(stackLocation)
        {
            _frameSkeleton = EmplaceResource(contentManager->Load<Animation::Skeleton>(FrameSkeletonPath));
            _frameMatset = EmplaceResource(contentManager->Load<Graphics::PolygonMaterialSet>(FrameMatsetPath));

            _buttonBorder = EmplaceResource(CreateButtonBorder(contentManager));
            _buttonCornerBorder = EmplaceResource(CreateButtonCornerBorder(contentManager));
            _buttonHighlight = EmplaceResource(CreateButtonHighlight(contentManager));

            _font = EmplaceResource(_contentManager->Load<Graphics::Font>(FontPath));
        }

        CharacterSelector::~CharacterSelector()
        {
        }

        void CharacterSelector::SetPosition(const Vector2f& left, const Vector2f& right, float scale)
        {
            _left = left;
            _right = right;
            _scale = scale;
            updatePlacement();
        }

        void CharacterSelector::SetCharacters(const CharacterSet& allCharacters)
        {
            _allCharacters = allCharacters;

            // Remove characters from piles that are no longer in the _allCharacters set
            CharacterPileMap::iterator iter = _pileMap.begin();
            while (iter != _pileMap.end())
            {
                if (!_allCharacters.ContainsCharacter(iter->first))
                {
                    removeCharacterFromPile(iter->first);
                    iter = _pileMap.erase(iter);
                }
                else
                {
                    iter++;
                }
            }

            // Insert missing characters into the piles
            for (uint32_t i = 0; i < _allCharacters.Count(); i++)
            {
                Character::Character* character = _allCharacters.GetCharacter(i);
                if (_pileMap.find(character) == _pileMap.end())
                {
                    _pileMap[character] = nullptr;
                }
            }

            updatePlacement();
        }

        void CharacterSelector::AnimateIn(bool instant)
        {
            for (auto stack : _stacks)
            {
                float stackAnimLen = stack.frame->GetAnimationLength(MoveInAnimation);
                stack.frame->PlayAnimation(MoveInAnimation, false, 0.0f, instant ? stackAnimLen : 0.0f);
            }
            _animatedIn = true;

            _animationDuration = _frameSkeleton->GetAnimation(MoveInAnimation).GetLength();
            _animationTimer = instant ? _animationDuration : 0.0f;
        }

        void CharacterSelector::AnimateOut(bool instant)
        {
            for (auto stack : _stacks)
            {
                float stackAnimLen = stack.frame->GetAnimationLength(MoveOutAnimation);
                stack.frame->PlayAnimation(MoveOutAnimation, false, 0.0f, instant ? stackAnimLen : 0.0f);
            }
            _animatedIn = false;

            _animationDuration = _frameSkeleton->GetAnimation(MoveOutAnimation).GetLength();
            _animationTimer = instant ? _animationDuration : 0.0f;
        }

        bool CharacterSelector::IsMouseOver() const
        {
            return _mouseOver;
        }

        const Character::Character* CharacterSelector::GetClickedCharacter() const
        {
            return _clickedCharacter.Get();
        }

        Character::Character* CharacterSelector::GetClickedCharacter()
        {
            return _clickedCharacter.Get();
        }

        const Panel* CharacterSelector::GetTooltip() const
        {
            return _currentTooltip;
        }

        void CharacterSelector::Update(double totalTime, float dt, const Input::FrameInput& input, const Camera& camera, const CharacterSet& selectedCharacters)
        {
            _clickedCharacter = nullptr;
            _mouseOver = false;
            _animationTimer += dt;

            // Update piles
            const float minimumPileDestructionDensity = 1.1f;
            for (int32_t i = 0; i < static_cast<int32_t>(_piles.size()); i++)
            {
                std::shared_ptr<CharacterPile> pile = _piles[i];
                pile->updateArea();

                // Break up piles that aren't dense enough
                if (lightenPile(pile, minimumPileDestructionDensity))
                {
                    i--;
                    break;
                }
            }

            // Add characters to piles if it makes them dense enough
            const float minimumPileCreationDensity = 1.2f;
            for (auto iter : _pileMap)
            {
                if (iter.second == nullptr)
                {
                    auto character = iter.first;

                    // Find the pile that increases in density the most
                    float maximumDensityDelta = 0.0f;
                    std::shared_ptr<CharacterPile> maximumPile = nullptr;

                    for (auto pile : _piles)
                    {
                        if (!Rectanglef::Intersects(character->GetBounds(), pile->area()))
                        {
                            continue;
                        }

                        float newDensity = pile->getDensityWithCharacter(character);
                        float densityDelta = newDensity - pile->density();
                        if (newDensity > minimumPileCreationDensity && densityDelta > maximumDensityDelta)
                        {
                            maximumDensityDelta = densityDelta;
                            maximumPile = pile;
                        }
                    }

                    if (maximumPile)
                    {
                        addCharacterToPile(character, maximumPile);
                    }
                }
            }

            // Create new piles
            for (auto iterA : _pileMap)
            {
                if (iterA.second != nullptr)
                {
                    continue;
                }

                // Create a pile of a single low-health character
                if (iterA.first->GetHealth().GetPercent() < HealthDrawThreshold)
                {
                    std::shared_ptr<CharacterPile> newPile = std::make_shared<CharacterPile>(_contentManager, _buttonSize, _buttonBorder, _buttonCornerBorder, _buttonHighlight, _font, _clickBind);
                    _piles.push_back(newPile);
                    addCharacterToPile(iterA.first, newPile);
                    continue;
                }

                const Rectanglef& boundsA = iterA.first->GetBounds();
                for (auto iterB : _pileMap)
                {
                    if (iterB.second != nullptr || iterA.first == iterB.first)
                    {
                        continue;
                    }

                    const Rectanglef& boundsB = iterB.first->GetBounds();
                    if ((boundsA.Area() + boundsB.Area()) / Rectanglef::Merge(boundsA, boundsB).Area() > minimumPileCreationDensity)
                    {
                        std::shared_ptr<CharacterPile> newPile = std::make_shared<CharacterPile>(_contentManager, _buttonSize, _buttonBorder, _buttonCornerBorder, _buttonHighlight, _font, _clickBind);
                        _piles.push_back(newPile);
                        addCharacterToPile(iterA.first, newPile);
                        addCharacterToPile(iterB.first, newPile);
                        break;
                    }
                }
            }

            // Merge piles
            for (uint32_t i = 0; i < _piles.size(); i++)
            {
                std::shared_ptr<CharacterPile> pileA = _piles[i];
                for (uint32_t j = i + 1; j < _piles.size(); j++)
                {
                    std::shared_ptr<CharacterPile> pileB = _piles[j];
                    if (Rectanglef::Intersects(pileA->area(), pileB->area()) && CharacterPile::getMergedPileDensity(pileA, pileB) >= minimumPileCreationDensity)
                    {
                        std::vector<CharacterPile::characterInfo> charactersToMove = pileB->characters();
                        for (auto character : charactersToMove)
                        {
                            addCharacterToPile(character.character, pileA);
                        }
                        j--;
                    }
                }
            }

            // Final pile update
            for (auto& pile : _piles)
            {
                pile->updateButtons(input, camera, selectedCharacters, _scale);

                Character::Character* clickedCharacter = const_cast<Character::Character*>(pile->GetClickedCharacter());
                if (clickedCharacter)
                {
                    _clickedCharacter = clickedCharacter;
                }

                if (pile->IsMouseOver())
                {
                    _currentTooltip = pile->GetTooltip();
                    _mouseOver = true;
                }
            }

            for (auto& stack : _stacks)
            {
                stack.frame->Update(totalTime, dt);
            }

            updatePlacement();

            Vector2f mousePosHUD = input.UnProjectMousePosition(camera);

            bool foundTopmostMouseoverButton = false;
            for (auto& stack : _stacks)
            {
                for (auto& button : stack.buttons)
                {
                    button.button->Update(input, camera);

                    bool mouseOverButton = RotatedRectanglef::Contains(button.button->GetBounds(), mousePosHUD);

                    if (button.button->IsJustClicked() && !foundTopmostMouseoverButton)
                    {
                        _clickedCharacter = button.character;
                    }

                    bool highlight = selectedCharacters.ContainsCharacter(button.character) || (mouseOverButton && !foundTopmostMouseoverButton);
                    button.button->ForceHighlightOn(highlight);
                    button.button->ForceHighlightOff(!highlight);

                    if (mouseOverButton)
                    {
                        _mouseOver = true;

                        if (!foundTopmostMouseoverButton)
                        {
                            _currentTooltip = button.tooltip;
                        }
                        foundTopmostMouseoverButton = true;
                    }

                    button.healthDrawable->SetHealth(button.character->GetHealth(), button.character->IsAlive());
                    button.button->SetIcon(button.character->GetIcon());
                    button.button->SetIconColor(getHealthIconColor(button.character));
                }
            }

            _justClicked = _mouseOver && input.IsBindJustPressed(_clickBind);
        }

        void CharacterSelector::Draw(Graphics::SpriteRenderer* spriteRenderer) const
        {
            Color pileColor = Color::FromBytes(255, 255, 255, _animatedIn ? 255 : 0);
            if (_animationTimer < _animationDuration)
            {
                assert(_animationTimer >= 0.0f && _animationDuration > 0.0f);
                float alpha = _animationTimer / _animationDuration;
                if (!_animatedIn)
                {
                    alpha = 1.0f - alpha;
                }
                pileColor.A = Clamp(alpha * 255.0f, 0, 255);
            }

            for (uint32_t i = 0; i < _piles.size(); i++)
            {
                std::shared_ptr<CharacterPile> pile = _piles[i];
                pile->draw(spriteRenderer, pileColor, _scale);
            }

            for (auto& stack : _stacks)
            {
                for (auto iter = stack.buttons.rbegin(); iter != stack.buttons.rend(); iter++)
                {
                    iter->button->Draw(spriteRenderer, Color::White);
                }
                stack.frame->Draw(spriteRenderer);
            }
        }

        void CharacterSelector::updatePlacement()
        {
            uint32_t typeCount = _allCharacters.GetTypeCount();
            if (typeCount < _stacks.size())
            {
                _stacks.resize(typeCount);
            }
            while (_stacks.size() < typeCount)
            {
                CharacterStack stack;
                stack.frame = MakeResource<Animation::SkeletonInstance>(_frameSkeleton, _frameMatset);

                if (_animatedIn)
                {
                    stack.frame->PlayAnimation(MoveInAnimation, false);
                }
                else
                {
                    stack.frame->PlayAnimation(MoveOutAnimation, false);
                }
                stack.frame->Update(0, 10.0f);

                _stacks.push_back(stack);
            }

            for (uint32_t i = 0; i < typeCount; i++)
            {
                auto& stack = _stacks[i];
                uint32_t numCharacters = _allCharacters.Count(i);

                if (numCharacters < _stacks[i].buttons.size())
                {
                    stack.buttons.resize(numCharacters);
                }
                while (stack.buttons.size() < numCharacters)
                {
                    CharacterButton newButton;
                    newButton.button = new Button();
                    newButton.healthDrawable = new HealthPanelDrawable();
                    newButton.button->SetBackground(newButton.healthDrawable);

                    stack.buttons.push_back(newButton);
                }

                float frameWidth = stack.frame->GetDrawBounds().W;

                Vector2f leftDir = Vector2f::Normalize(_right - _left);
                Vector2f rightDir = Vector2f::Normalize(_left - _right);

                Vector2f framePosition;
                switch (_stackLocation)
                {
                case CharacterStackLocation::Left:
                {
                    framePosition = _left + (leftDir * ((frameWidth * i) + (frameWidth * 0.5f)));
                    break;
                }

                case CharacterStackLocation::Right:
                {
                    framePosition = _right + (rightDir * ((frameWidth * i) + (frameWidth * 0.5f)));
                    break;
                }

                case CharacterStackLocation::Center:
                {
                    float totalWidth = typeCount * frameWidth;
                    Vector2f newLeft = ((_right + _left) * 0.5f) + (rightDir * (totalWidth * 0.5f));
                    framePosition = newLeft + (leftDir * ((frameWidth * i) + (frameWidth * 0.5f)));
                    break;
                }

                default:
                    assert(false);
                    break;
                }


                stack.frame->SetPosition(framePosition);
                stack.frame->SetScale(_scale);

                float left = stack.frame->GetJointPosition(FrameButtonLeftJoint).X;
                float right = stack.frame->GetJointPosition(FrameButtonRightJoint).X;
                float bottom = stack.frame->GetJointPosition(FrameButtonBottomJoint).Y;
                float top = stack.frame->GetJointPosition(FrameButtonTopJoint).Y;

                float buttonSize = right - left;
                float usableSpace = bottom - top - buttonSize;
                float verticalSpacing = Min(usableSpace / numCharacters, buttonSize);

                for (uint32_t j = 0; j < numCharacters; j++)
                {
                    auto& button = stack.buttons[j];

                    button.button->SetHighlightMaterial(_buttonHighlight);
                    button.button->SetBorder(_buttonBorder);
                    button.button->SetCornerBorder(_buttonCornerBorder);
                    button.button->SetCornerText(Alignment_TopLeft, _font, getCharacterButtonKeybindText(button.character));

                    Rectanglef buttonBounds(left, bottom - (j * verticalSpacing) - buttonSize, buttonSize, buttonSize);
                    button.button->SetBounds(buttonBounds, _scale);

                    button.button->SetClickBind(_clickBind);

                    button.character = _allCharacters.GetCharacter(i, j);
                    button.healthDrawable->SetHealth(button.character->GetHealth(), button.character->IsAlive());

                    button.button->SetIcon(button.character->GetIcon());

                    std::string tooltipText = button.character->GetName();
                    if (tooltipText != button.tooltipText)
                    {
                        button.tooltip = EmplaceResource(CreateSimpleTextTooltip(_contentManager, button.character->GetName()));
                        button.tooltipText = tooltipText;
                    }
                }
            }
        }

        void CharacterSelector::addCharacterToPile(const Character::Character* character, std::shared_ptr<CharacterSelector::CharacterPile> pile)
        {
            assert(character != nullptr && pile != nullptr);

            std::shared_ptr<CharacterSelector::CharacterPile> prevPile = _pileMap[character];
            if (prevPile == pile)
            {
                return;
            }

            if (prevPile)
            {
                removeCharacterFromPile(character);
            }

            _pileMap[character] = pile;
            pile->addCharacter(character);
        }

        bool CharacterSelector::removeCharacterFromPile(const Character::Character* character)
        {
            assert(character != nullptr);

            std::shared_ptr<CharacterSelector::CharacterPile> prevPile = _pileMap[character];
            if (prevPile)
            {
                prevPile->removeCharacter(character);
                _pileMap[character] = nullptr;

                if (prevPile->size() == 0)
                {
                    _piles.erase(std::find(_piles.begin(), _piles.end(), prevPile));
                    return true;
                }
            }

            return false;
        }

        bool CharacterSelector::lightenPile(std::shared_ptr<CharacterSelector::CharacterPile> pile, float minimumDensity)
        {
            // If this pile only contains one character and that character is above the health threshold, remove the pile
            if (pile->size() == 1)
            {
                auto character = pile->characters().front().character;
                if (character->GetHealth().GetPercent() >= HealthDrawThreshold)
                {
                    if (removeCharacterFromPile(character))
                    {
                        return true;
                    }
                }
            }

            // Build a list of unique rectangles for this pile, remove all characters that don't fit in the first pile
            std::vector<Rectanglef> characterBounds;
            characterBounds.reserve(pile->characters().size());

            for (auto character : pile->characters())
            {
                characterBounds.push_back(character.character->GetBounds());
            }

            std::vector<Rectanglef> uniqueBounds = Math::MergeRectangles(characterBounds);
            assert(uniqueBounds.size() > 0);

            // Remove characters that no longer intersect with the rest of the pile
            bool removedNonIntersectingCharacter = true;
            while (pile->size() > 1 && removedNonIntersectingCharacter)
            {
                removedNonIntersectingCharacter = false;

                for (auto character : pile->characters())
                {
                    if (!Rectanglef::Intersects(uniqueBounds[0], character.character->GetBounds()))
                    {
                        if (removeCharacterFromPile(character.character))
                        {
                            return true;
                        }

                        removedNonIntersectingCharacter = true;
                        break;
                    }
                }
            }

            // Make sure the pile isn't deleted from under us
            while (pile->size() > 1 && pile->density() < minimumDensity)
            {
                // Remove the character that increases the density of the remaining pile the most

                float maximumDensity = 0.0f;
                const Character::Character* maximumCharacter = nullptr;

                for (auto character : pile->characters())
                {
                    float densityWithoutCharacter = pile->getDensityWithoutCharacter(character.character);

                    if (densityWithoutCharacter > maximumDensity)
                    {
                        maximumDensity = densityWithoutCharacter;
                        maximumCharacter = character.character;
                    }
                }

                if (removeCharacterFromPile(maximumCharacter))
                {
                    return true;
                }
            }

            return false;
        }
    }

    template <>
    void EnumeratePreloads<HUD::CharacterSelector>(PreloadSet& preloads)
    {
        EnumerateTooltipPreloads(preloads);
        preloads.insert(HUD::FontPath);
        preloads.insert(HUD::FrameSkeletonPath);
        preloads.insert(HUD::FrameMatsetPath);
    }
}
