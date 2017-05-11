#include "HUD/ActionDisplay.hpp"

#include "Item/Item.hpp"

namespace Dwarf
{
    namespace HUD
    {
        static const std::string ActionDisplaySkeletonPath = "Skeletons/UI/ActionWidget/go_here.skel";
        static const std::string ActionDisplayMoveMatsetPath = "Skeletons/UI/ActionWidget/go_here.polymatset";
        static const std::string ActionDisplayAttackMatsetPath = "Skeletons/UI/ActionWidget/attack_here.polymatset";
        static const std::string ActionDisplayInteractMatsetPath = "Skeletons/UI/ActionWidget/interact_here.polymatset";
        static const float ActionDisplaySkeletonScale = 1.0f;

        static const float fadeInTime = 0.15f;
        static const float fadeOutTime = 0.25f;

        ActionDisplay::ActionItem::ActionItem()
            : targetPosition()
            , targetCharacter(0)
            , targetItem(0)
            , skeleton(nullptr)
            , fadeInTimer(fadeInTime)
            , fadeOutTimer(fadeOutTime)
        {
        }

        ActionDisplay::ActionDisplay(Content::ContentManager* contentManager, const Level::LevelLayerInstance* layer)
            : _layer(layer)
            , _moveMatset(nullptr)
            , _attackMatset(nullptr)
            , _interactMatset(nullptr)
            , _skeleton(nullptr)
            , _animation()
            , _activeItems()
            , _scale(1.0f)
        {
            assert(_layer);
            SafeAddRef(_layer);

            _moveMatset = contentManager->Load<Graphics::PolygonMaterialSet>(ActionDisplayMoveMatsetPath);
            _attackMatset = contentManager->Load<Graphics::PolygonMaterialSet>(ActionDisplayAttackMatsetPath);
            _interactMatset = contentManager->Load<Graphics::PolygonMaterialSet>(ActionDisplayInteractMatsetPath);
            _skeleton = contentManager->Load<Animation::Skeleton>(ActionDisplaySkeletonPath);
            _animation.AddAnimation("go_here", 1.0f);
        }

        ActionDisplay::~ActionDisplay()
        {
            SafeRelease(_layer);

            SafeRelease(_moveMatset);
            SafeRelease(_attackMatset);
            SafeRelease(_interactMatset);
            SafeRelease(_skeleton);

            for (ActionItem& item : _activeItems)
            {
                SafeRelease(item.skeleton);
            }
        }

        void ActionDisplay::SetScale(float scale)
        {
            _scale = scale;
        }

        void ActionDisplay::OnCharactersMove(const Vector2f& position)
        {
            ActionItem item;
            item.targetPosition = position;
            item.skeleton = new Animation::SkeletonInstance(_skeleton, _moveMatset);
            item.skeleton->SetScale(ActionDisplaySkeletonScale);
            item.skeleton->PlayAnimationSet(_animation, false);
            item.interaction = false;

            _activeItems.push_back(item);
        }

        void ActionDisplay::OnCharactersFollow(Character::CharacterID target)
        {
            ActionItem item;
            item.targetCharacter = target;
            item.skeleton = new Animation::SkeletonInstance(_skeleton, _moveMatset);
            item.skeleton->SetScale(ActionDisplaySkeletonScale);
            item.skeleton->PlayAnimationSet(_animation, false);
            item.interaction = false;

            _activeItems.push_back(item);
        }

        void ActionDisplay::OnCharactersAttack(Character::CharacterID target)
        {
            ActionItem item;
            item.targetCharacter = target;
            item.skeleton = new Animation::SkeletonInstance(_skeleton, _attackMatset);
            item.skeleton->SetScale(ActionDisplaySkeletonScale);
            item.skeleton->PlayAnimationSet(_animation, false);
            item.interaction = false;

            _activeItems.push_back(item);
        }

        void ActionDisplay::OnCharactersAttackMove(const Vector2f& position)
        {
            ActionItem item;
            item.targetPosition = position;
            item.skeleton = new Animation::SkeletonInstance(_skeleton, _attackMatset);
            item.skeleton->SetScale(ActionDisplaySkeletonScale);
            item.skeleton->PlayAnimationSet(_animation, false);
            item.interaction = false;

            _activeItems.push_back(item);
        }

        void ActionDisplay::OnCharactersInteractCharacter(Character::CharacterID source, Character::CharacterID target)
        {
            ActionItem item;
            item.sourceCharacter = source;
            item.targetCharacter = target;
            item.skeleton = new Animation::SkeletonInstance(_skeleton, _interactMatset);
            item.skeleton->SetScale(ActionDisplaySkeletonScale);
            item.skeleton->PlayAnimationSet(_animation, false);
            item.interaction = true;

            _activeItems.push_back(item);
        }

        void ActionDisplay::OnCharactersInteractItem(Item::ItemID target)
        {
            ActionItem item;
            item.targetItem = target;
            item.skeleton = new Animation::SkeletonInstance(_skeleton, _interactMatset);
            item.skeleton->SetScale(ActionDisplaySkeletonScale);
            item.skeleton->PlayAnimationSet(_animation, false);
            item.interaction = true;

            _activeItems.push_back(item);
        }

        void ActionDisplay::Update(double totalTime, float dt, const Camera& worldCamera, const Camera& hudCamera)
        {
            for (int32_t i = 0; i < static_cast<int32_t>(_activeItems.size()); i++)
            {
                ActionItem& item = _activeItems[i];
                Character::Character* sourceCharacter = item.sourceCharacter ? _layer->GetCharacter(item.sourceCharacter) : nullptr;
                Character::Character* targetCharacter = item.targetCharacter ? _layer->GetCharacter(item.targetCharacter) : nullptr;
                Item::Item* targetItem = item.targetItem ? _layer->GetItem(item.targetItem) : nullptr;

                if (item.fadeOutTimer < 0.0f || (item.targetCharacter && targetCharacter == nullptr) || (item.targetItem && targetItem == nullptr))
                {
                    SafeRelease(item.skeleton);
                    _activeItems.erase(_activeItems.begin() + i);
                    i--;
                    continue;
                }

                item.skeleton->Update(totalTime, dt);

                item.fadeInTimer -= dt;
                float alphaPerc = 1.0f - (item.fadeInTimer / fadeInTime);

                if (item.skeleton->IsAnimationFinished())
                {
                    item.fadeOutTimer -= dt;
                    alphaPerc = (item.fadeOutTimer / fadeOutTime);
                }

                Color color = Color::FromFloats(1.0f, 1.0f, 1.0f, Clamp(alphaPerc, 0.0f, 1.0f));
                item.skeleton->SetColor(color);

                if (item.targetCharacter)
                {
                    assert(targetCharacter);
                    const Rectanglef& bounds = targetCharacter->GetBounds();

                    Vector2f targetPos = Vector2f(bounds.Middle().X, bounds.Top());
                    if (item.interaction && sourceCharacter)
                    {
                        targetPos.X = targetCharacter->GetInteractionMoveTarget(sourceCharacter).X;
                    }

                    item.skeleton->SetPosition(Camera::Transpose(worldCamera, targetPos, hudCamera));
                    item.skeleton->SetScale(_scale);
                }
                else if (item.targetItem)
                {
                    assert(targetItem);
                    const Rectanglef& bounds = targetItem->GetBounds();
                    item.skeleton->SetPosition(Camera::Transpose(worldCamera, Vector2f(bounds.Middle().X, bounds.Top()), hudCamera));
                    item.skeleton->SetScale(_scale);
                }
                else
                {
                    item.skeleton->SetPosition(Camera::Transpose(worldCamera, item.targetPosition, hudCamera));
                    item.skeleton->SetScale(_scale);
                }
            }
        }

        void ActionDisplay::Draw(Graphics::SpriteRenderer* spriteRenderer) const
        {
            for (const ActionItem& item : _activeItems)
            {
                item.skeleton->Draw(spriteRenderer);
            }
        }
    }

    template <>
    void EnumeratePreloads<HUD::ActionDisplay>(PreloadSet& preloads)
    {
        preloads.insert(HUD::ActionDisplaySkeletonPath);
        preloads.insert(HUD::ActionDisplayMoveMatsetPath);
        preloads.insert(HUD::ActionDisplayAttackMatsetPath);
        preloads.insert(HUD::ActionDisplayInteractMatsetPath);
    }
}
