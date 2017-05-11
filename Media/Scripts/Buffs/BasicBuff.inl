namespace Dwarf
{
    namespace Character
    {
        template <typename T, typename... argsT>
        T* BasicBuff::AddComponent(argsT... args)
        {
            BuffComponentParameters params;
            params.owner = GetOwner();

            T* component = new T(params, std::forward<argsT>(args)...);
            _components.push_back(component);
            return component;
        }
    }
}
