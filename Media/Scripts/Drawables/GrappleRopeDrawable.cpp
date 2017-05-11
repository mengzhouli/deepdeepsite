#include "Drawables/GrappleRopeDrawable.hpp"

#include "Drawables/RopeUtility.hpp"
#include "Drawables/RopeDrawable.hpp"

#include "ContentUtility.hpp"

namespace Dwarf
{
    namespace Graphics
    {
        static const std::string HookSkeletonPath = "Skeletons/Items/Tools/Grapple/GrappleHook.skel";
        static const std::string HookMatsetPath = "Skeletons/Items/Tools/Grapple/GrappleHook.polymatset";
        static const std::string PegSkeletonPath = "Skeletons/Items/Tools/Grapple/GrapplePeg.skel";
        static const std::string PegMatsetPath = "Skeletons/Items/Tools/Grapple/GrapplePeg.polymatset";

        GrappleRopeDrawable::GrappleRopeDrawable(Content::ContentManager* cm)
            : _hook(nullptr)
            , _peg(nullptr)
            , _rope(nullptr)
            , _drawPeg(true)
            , _bounds()
        {
            _hook = Content::CreateSkeletonInstance(cm, HookSkeletonPath, HookMatsetPath);
            _hook->SetScale(1.2f);

            _peg = Content::CreateSkeletonInstance(cm, PegSkeletonPath, PegMatsetPath);
            _peg->SetScale(1.2f);

            _rope = new Graphics::RopeDrawable(cm);
        }

        GrappleRopeDrawable::~GrappleRopeDrawable()
        {
            SafeRelease(_rope);
            SafeRelease(_hook);
            SafeRelease(_peg);
        }

        void GrappleRopeDrawable::Draw(Graphics::SpriteRenderer* spriteRenderer) const
        {
            _rope->Draw(spriteRenderer);
            _hook->Draw(spriteRenderer);

            if (_drawPeg)
            {
                _peg->Draw(spriteRenderer);
            }
        }

        const Rectanglef& GrappleRopeDrawable::GetDrawBounds() const
        {
            return _bounds;
        }

        void GrappleRopeDrawable::SetDrawPeg(bool drawPeg)
        {
            _drawPeg = drawPeg;
        }

        const Rectanglef& GrappleRopeDrawable::GetHookBounds() const
        {
            return _hook->GetDrawBounds();
        }

        void GrappleRopeDrawable::SetColor(const Color& color)
        {
            _rope->SetColor(color);
            _peg->SetColor(color);
            _hook->SetColor(color);
        }

        void GrappleRopeDrawable::SetChain(Chainf chain)
        {
            _hook->SetPosition(chain.At(0));
            _hook->SetRotation(Rotatorf(chain.At(0) - chain.At(1)) - Rotatorf::PiOver2);
            chain.UpdatePoint(_hook->GetJointPosition("rope_attach"), 0);

            if (_drawPeg)
            {
                _peg->SetPosition(chain.At(chain.Size() - 1));
                _peg->SetRotation(Rotatorf(chain.At(chain.Size() - 1) - chain.At(chain.Size() - 2)) + Rotatorf::PiOver2);
                chain.UpdatePoint(_peg->GetJointPosition("rope_attach"), chain.Size() - 1);
            }

            _rope->SetChain(chain);

            _bounds = Rectanglef::Merge(_hook->GetDrawBounds(), chain.Bounds());
            if (_drawPeg)
            {
                _bounds = Rectanglef::Merge(_bounds, _peg->GetDrawBounds());
            }
        }
    }

    template <>
    void EnumeratePreloads<Graphics::GrappleRopeDrawable>(PreloadSet& preloads)
    {
        EnumeratePreloads<Graphics::RopeDrawable>(preloads);
        preloads.insert(Graphics::HookSkeletonPath);
        preloads.insert(Graphics::HookMatsetPath);
        preloads.insert(Graphics::PegSkeletonPath);
        preloads.insert(Graphics::PegMatsetPath);
    }
}
