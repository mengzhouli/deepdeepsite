#include "Drawables/BasketRope.hpp"
#include "ContentUtility.hpp"
#include "Random.hpp"

namespace Dwarf
{
    namespace Graphics
    {
        static const std::string BasketSkeletonPath = "Skeletons/Characters/Gear/basket_0.skel";
        static const std::vector<std::string> BasketMatsetPaths =
        {
            "Skeletons/Characters/Gear/bucket_0.polymatset",
            "Skeletons/Characters/Gear/bucket_1.polymatset",
        };

        BasketRopeDrawable::BasketRopeDrawable(Content::ContentManager* cm, const Splinef& spline)
            : _baskets()
            , _tStep(0.0f)
            , _spline(spline)
            , _bounds()
            , _rope(nullptr)
        {
            const float BasketSpacing = 1000.0f;
            const float BasketSpeed = 200.0f;

            _tStep = BasketSpeed / _spline.Length();

            uint32_t numBaskets = Max(_spline.Length() / BasketSpacing, 2);
            for (uint32_t i = 0; i < numBaskets; i++)
            {
                basket b;
                b.skeleton = Content::CreateSkeletonInstance(cm, BasketSkeletonPath, Random::RandomItem(BasketMatsetPaths));
                b.skeleton->PlayAnimation("idle", true, 0.0f, Random::RandomBetween(0.0f, 5.0f));
                b.currentT = float(i) / (numBaskets - 1);
                _baskets.push_back(b);
            }

            _rope = new RopeDrawable(cm);
            _rope->SetSpline(spline, 128);
        }

        void BasketRopeDrawable::Update(double totalTime, float dt)
        {
            _bounds = _rope->GetDrawBounds();
            for (auto& b : _baskets)
            {
                b.currentT = Mod(b.currentT + (_tStep * dt), 1.0f);
                b.skeleton->SetPosition(_spline.Evalulate(b.currentT));

                b.skeleton->Update(totalTime, dt);
                _bounds = Rectanglef::Merge(_bounds, b.skeleton->GetDrawBounds());
            }
        }

        BasketRopeDrawable::~BasketRopeDrawable()
        {
            for (auto& b : _baskets)
            {
                SafeRelease(b.skeleton);
            }
            SafeRelease(_rope);
        }


        const Rectanglef& BasketRopeDrawable::GetDrawBounds() const
        {
            return _bounds;
        }

        void BasketRopeDrawable::Draw(Graphics::SpriteRenderer* spriteRenderer) const
        {
            _rope->Draw(spriteRenderer);
            for (const auto& b : _baskets)
            {
                b.skeleton->Draw(spriteRenderer);
            }
        }
    }

    template <>
    void EnumeratePreloads<Graphics::BasketRopeDrawable>(PreloadSet& preloads)
    {
        EnumeratePreloads<Graphics::RopeDrawable>(preloads);
        preloads.insert(Graphics::BasketSkeletonPath);
        preloads.insert(Graphics::BasketMatsetPaths.begin(), Graphics::BasketMatsetPaths.end());
    }
}
