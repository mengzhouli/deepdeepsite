#include "Items/Weapons/Bow.hpp"

#include "Characters/Arrow.hpp"

namespace Dwarf
{
    namespace Item
    {
        Bow::Bow(const ItemParameters& parameters, const std::string& skeletonPath, const std::string& matsetPath)
            : BasicRangedWeapon(parameters, "", WeaponType_Ranged_Bow, skeletonPath, matsetPath)
        {
            SetRange(1500.0f);
            SetDPS(Character::Damage(Character::DamageType_Type_Projectile | Character::DamageType_Source_Piercing | Character::DamageType_Element_Physical, 10.0f));
        }

        void Bow::Fire(const Character::Damage& dmg)
        {
            BasicRangedWeapon::Fire(dmg);

            Character::Character *owner = GetOwner();
            assert(owner);

            Character::Character *target = GetCurrentTarget();
            assert(target);

            const Rayf& ownerHandPos = owner->GetAttachPoint(GetAttachPoint());

            Character::Arrow* arrow = SpawnArrow(ownerHandPos.Position, dmg);
            const Physics::Collision *arrowCollision = arrow->GetCollision();

            float m = arrowCollision->GetMass();
            float g = arrowCollision->GetPhysicsWorld()->GetGravity().Y;

            Vector2f dir = ComputeTargetAttackPoint(target) - arrow->GetPosition();

            float rangePerc = Clamp(dir.Length() / GetRange(), 0.0f, 1.0f);

            // Adjust flight time depending on range perc
            const float t = Lerp(0.1f, 1.3f, rangePerc);

            //const Vector2f vi(dir.X / t, -0.5f * g * t * t);
            const Vector2f vi(dir.X / t, ((2.0f * dir.Y) - (g * t * t)) / (2.0f * t));

            const Vector2f impulse(vi * m);

            arrow->ApplyLinearImpulse(impulse);
        }
    }

    template <>
    void EnumeratePreloads<Item::Bow>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicRangedWeapon>(preloads);
        preloads.insert("Skeletons/Items/Weapons/BoneWeapons/skelarrow.skel");
        preloads.insert("Skeletons/Items/Weapons/BoneWeapons/skelarrow.polymatset");

        EnumeratePreloads<Character::Arrow>(preloads);
    }
}
