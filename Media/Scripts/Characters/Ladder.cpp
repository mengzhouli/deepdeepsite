#include "Characters/Ladder.hpp"

#include "ContentUtility.hpp"

namespace Dwarf
{
    namespace Character
    {
        static const std::string LadderSegmentSkeletonPath = "Skeletons/Characters/Ladder/ladder_segment.skel";
        static const std::string LadderSegmentMatsetPath = "Skeletons/Characters/Ladder/ladder_segment.polymatset";
        static const std::string LadderEndASkeletonPath = "Skeletons/Characters/Ladder/ladder_end_a.skel";
        static const std::string LadderEndAMatsetPathPath = "Skeletons/Characters/Ladder/ladder_end_a.polymatset";
        static const std::string LadderEndBSkeletonPath = "Skeletons/Characters/Ladder/ladder_end_b.skel";
        static const std::string LadderEndBMatsetPathPath = "Skeletons/Characters/Ladder/ladder_end_b.polymatset";

        Ladder::Ladder(const CharacterParameters& parameters, const Vector2f& top, const Vector2f& bottom, bool valid)
            : Character(parameters)
            , _top(top)
            , _bottom(bottom)
            , _endA(nullptr)
            , _segments()
            , _endB(nullptr)
            , _pathId(Pathfinding::PathItemID_Invalid)
        {
            SetAttachToGroundOnSpawn(false);
            DisablePhysics();
            SetInvulnerable(true);

            if (!valid)
            {
                SetColor(Color::Red);
            }

            SetEntityMask(CharacterMask_Normal | CharacterMask_Constructed);
        }

        Ladder::~Ladder()
        {
        }

        void Ladder::OnSpawn()
        {
            Chainf chain;
            chain.AddPoint(_top);
            chain.AddPoint(_bottom);

            Pathfinding::PathSystem* pathSystem = GetLevelLayer()->GetPathSystem();
            _pathId = pathSystem->AddChain(chain, Pathfinding::EdgeType_Climb, Pathfinding::TerrainType::Wood);
        }

        void Ladder::OnDeath()
        {
            Pathfinding::PathSystem* pathSystem = GetLevelLayer()->GetPathSystem();
            pathSystem->RemoveObject(_pathId);
            Terminate();
        }

        void Ladder::OnLoadContent(Content::ContentManager* contentManager)
        {
            std::shared_ptr<Pathfinding::PathPosition> ledge = GetLevelLayer()->FindLedge(_top, 100.0f, Pathfinding::EdgeType_All);
            bool invert = (ledge != nullptr) ? ledge->GetNormal().X < 0.0f : false;

            Vector2f ladderDir = Vector2f::Normalize(_top - _bottom);
            if (invert)
            {
                ladderDir.X = -ladderDir.X;
            }

            _endA = Content::CreateSkeletonInstance(contentManager, LadderEndASkeletonPath, LadderEndAMatsetPathPath);
            _endA->SetPosition(_bottom);
            _endA->SetRotation(ladderDir);
            _endA->SetInvertedX(invert);

            float remainingDist = Vector2f::Distance(_top, _bottom);
            Vector2f curSegmentPos = _bottom;
            while (remainingDist > 0.0f)
            {
                Animation::SkeletonInstance* segment = Content::CreateSkeletonInstance(contentManager, LadderSegmentSkeletonPath, LadderSegmentMatsetPath);
                segment->SetPosition(curSegmentPos);
                segment->SetRotation(ladderDir);
                segment->SetInvertedX(invert);

                curSegmentPos = segment->GetJointPosition("b");
                remainingDist -= Vector2f::Distance(segment->GetJointPosition("a"), segment->GetJointPosition("b"));

                _segments.push_back(segment);
            }

            _endB = Content::CreateSkeletonInstance(contentManager, LadderEndBSkeletonPath, LadderEndBMatsetPathPath);
            _endB->SetPosition(curSegmentPos);
            _endB->SetRotation(ladderDir);
            _endB->SetInvertedX(invert);
        }

        void Ladder::OnUnloadContent()
        {
            SafeRelease(_endA);
            SafeReleaseAndClearContainer(_segments);
            SafeRelease(_endB);
        }

        void Ladder::OnUpdate(double totalTime, float dt)
        {
            _endA->Update(totalTime, dt);
            for (Animation::SkeletonInstance* segment : _segments)
            {
                segment->Update(totalTime, dt);
            }
            _endB->Update(totalTime, dt);
        }

        void Ladder::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            levelRenderer->AddDrawable(_endA, false);
            levelRenderer->AddMinimapDrawable(_endA);
            for (Animation::SkeletonInstance* segment : _segments)
            {
                levelRenderer->AddDrawable(segment, false);
                levelRenderer->AddMinimapDrawable(segment);
            }
            levelRenderer->AddDrawable(_endB, false);
            levelRenderer->AddMinimapDrawable(_endB);

            //levelRenderer->AddDrawable(_drawable, false, false);
        }
    }

    template <>
    void EnumeratePreloads<Character::Ladder>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Character>(preloads);
        preloads.insert(Character::LadderSegmentSkeletonPath);
        preloads.insert(Character::LadderSegmentMatsetPath);
        preloads.insert(Character::LadderEndASkeletonPath);
        preloads.insert(Character::LadderEndAMatsetPathPath);
        preloads.insert(Character::LadderEndBSkeletonPath);
        preloads.insert(Character::LadderEndBMatsetPathPath);
    }
}
