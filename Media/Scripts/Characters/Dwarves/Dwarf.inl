namespace Dwarf
{
    namespace Character
    {
        template <typename T, typename... argsT>
        DwarfConstructor<T> BindDwarfConstructor(argsT... args)
        {
            using namespace std::placeholders;
            return std::bind(BindCharacterConstructor<T, const DwarfInfo&, argsT...>, _1, std::forward<argsT>(args)...);
        }
    }
}
