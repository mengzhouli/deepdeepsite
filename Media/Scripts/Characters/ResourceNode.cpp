#include "Characters/ResourceNode.hpp"
#include "ContentUtility.hpp"

#include "Item/Item.hpp"
#include "Items/Weapons/WeaponTraits.hpp"

namespace Dwarf
{
    namespace Character
    {
        static const std::string MineCursorPath = "HUD/hud.cursorset";

        ResourceNode::ResourceNode(const CharacterParameters& parameters, const std::string& skeletonPath,
                                   const std::string& materialSetPath, const Item::Resources& resources)
            : SkeletonCharacter(parameters, skeletonPath, materialSetPath)
            , _resources(resources)
            , _mineCursor(nullptr)
        {
            SetEntityMask(resources.Gold > 0 ? CharacterMask_Usable : 0);

            SetMoveType(MoveType_None);

            DisablePhysics();
            SetInvulnerable(true);
            SetSkeletonJointStrength(0.0f);
        }

        const App::Cursor* ResourceNode::GetInteractCursor() const
        {
            return _mineCursor;
        }

        Item::Resources ResourceNode::ExtractResources(const Item::Resources& maximumAmmount)
        {
            if (_resources > maximumAmmount)
            {
                _resources -= maximumAmmount;
                return maximumAmmount;
            }
            else
            {
                Item::Resources ammount = _resources;
                _resources = Item::Resources();
                SetEntityMask(GetEntityMask() & ~CharacterMask_Usable);
                return ammount;
            }
        }

        const Item::Resources& ResourceNode::GetResources() const
        {
            return _resources;
        }

        bool ResourceNode::CanBeInteractedWith(const Character* interactor) const
        {
            return interactor->GetItems<Item::Miner>().size() > 0;
        }

        void ResourceNode::OnLoadContent(Content::ContentManager* contentManager)
        {
            SkeletonCharacter::OnLoadContent(contentManager);
            _mineCursor = Content::GetCursor(contentManager, MineCursorPath, "cursor_mine");
        }

        void ResourceNode::OnUnloadContent()
        {
            SkeletonCharacter::OnUnloadContent();

            SafeRelease(_mineCursor);
        }

        void ResourceNode::OnUpdate(double totalTime, float dt)
        {
            SkeletonCharacter::OnUpdate(totalTime, dt);

            // Check if empty
            if (_resources == Item::Resources())
            {
                EnablePhysics(true);
                PushAction(CreateDeathAction(), false);
            }
        }

        static const std::string GoldNodeSkeletonPath = "Skeletons/Characters/skellynode/skellynode.skel";
        static const std::string GoldNodeMatsetPath = "Skeletons/Characters/skellynode/minesnode.polymatset";

        GoldNode::GoldNode(const CharacterParameters& parameters, const Item::Resources& resources)
            : ResourceNode(parameters, GoldNodeSkeletonPath, GoldNodeMatsetPath, resources)
        {
            SetTooltip("character_gold_node_name");
        }
    }

    template <>
    void EnumeratePreloads<Character::ResourceNode>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::SkeletonCharacter>(preloads);
        preloads.insert(Character::MineCursorPath);
    }

    template <>
    void EnumeratePreloads<Character::GoldNode>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::ResourceNode>(preloads);

        preloads.insert(Character::GoldNodeSkeletonPath);
        preloads.insert(Character::GoldNodeMatsetPath);
    }
}
