namespace Dwarf
{
    namespace Level
    {
        template <typename T>
        std::vector<T*> BasicLevel::SpawnCharactersAtTriggers(const std::string& triggerLocations, Character::CharacterConstructor<T> constructor,
                                                              const std::string& nameBase, Character::Controller* controller)
        {
            std::vector<T*> result;

            for (uint32_t i = 0; i < GetLayerCount(); i++)
            {
                LevelLayerInstance* layer = GetLayer(i);
                std::vector<Vector2f> spawns = layer->GetTriggerPositions(triggerLocations);
                for (uint32_t j = 0; j < spawns.size(); j++)
                {
                    result.push_back(layer->SpawnCharacter(spawns[j], Format("%s_%u", nameBase.c_str(), j), controller, constructor));
                }
            }
            return result;
        }
    }
}
