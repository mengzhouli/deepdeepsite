namespace Dwarf
{
    namespace Item
    {
        template <typename T>
        FindableItemSet MergeFindableItems(const T& value)
        {
            return value;
        }

        template <typename T, typename... Args>
        FindableItemSet MergeFindableItems(const T& value, Args... args)
        {
            auto mergeTwoFindableItems = [](const FindableItems& a, const FindableItems& b)
            {
                FindableItems result = b;
                for (const auto& characterType : a)
                {
                    for (const auto& itemType : characterType.second)
                    {
                        uint32_t valueItemCount = itemType.second;
                        uint32_t baseItemCount = result[characterType.first][itemType.first];
                        uint32_t mergedItemCount = (valueItemCount == InfiniteItemCount || baseItemCount == InfiniteItemCount) ? InfiniteItemCount : valueItemCount + baseItemCount;
                        result[characterType.first][itemType.first] = mergedItemCount;
                    }
                }
                return result;
            };

            FindableItemSet a = value;
            FindableItemSet b = MergeFindableItems(args...);

            a.Weapons = mergeTwoFindableItems(a.Weapons, b.Weapons);
            a.Trinkets = mergeTwoFindableItems(a.Trinkets, b.Trinkets);
            a.Armors = mergeTwoFindableItems(a.Armors, b.Armors);

            return a;
        }

        template <typename T>
        ItemInfo<T>::ItemInfo()
            : ItemInfo("", nullptr, 0)
        {
        }

        template <typename T>
        ItemInfo<T>::ItemInfo(std::string key, ItemConstructor<T> constructor, uint32_t count)
            : Key(key)
            , Constructor(constructor)
            , Count(count)
        {
        }

        template <typename T>
        GameItem<T>::GameItem(const std::string& key, ItemConstructor<T> constructor, PreloadFunction preloadFunction)
            : _key(key)
        {
            _items[_key] = std::make_pair(constructor, preloadFunction);
        }

        template <typename T>
        const std::string& GameItem<T>::operator()() const
        {
            return _key;
        }

        template <typename T>
        bool GameItem<T>::IsItem(const std::string& key)
        {
            return _items.find(key) != _items.end();
        }

        template <typename T>
        ItemConstructor<T> GameItem<T>::GetConstructor(const std::string& key)
        {
            return _items[key].first;
        }

        template <typename T>
        PreloadFunction GameItem<T>::GetPreloadFunction(const std::string& key)
        {
            return _items[key].second;
        }

        template <typename T>
        std::vector<std::string> GameItem<T>::GetAllItems()
        {
            std::vector<std::string> keys;
            for (auto info : _items)
            {
                keys.push_back(info.first);
            }
            return keys;
        }

        template <typename T> std::map<std::string, std::pair<ItemConstructor<T>, PreloadFunction>> GameItem<T>::_items;
    }
}
