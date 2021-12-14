#ifndef ECS_HPP
#define ECS_HPP

#include <iostream>
#include <vector>
#include <bitset>

#include "memorypool.hpp"

typedef unsigned long long EntityID;
const int MAX_COMPONENTS = 32;
const int MAX_ENTITIES = 32;
typedef std::bitset<MAX_COMPONENTS> ComponentMask;

int componentCounter;

template <class T>
int get_id()
{
    static int componentId = componentCounter++;
    return componentId;
}

struct Scene
{
    struct EntityDesc
    {
        EntityID id;
        ComponentMask mask;
    };

    std::vector<EntityDesc> entities;
    std::vector<MemoryPool<void> *> componentPools;

    EntityID create()
    {
        entities.push_back((EntityDesc){entities.size(), ComponentMask()});
        return entities.back().id;
    }

    template <typename T>
    T *add_component(EntityID id)
    {
        int componentId = get_id<T>();

        if (componentPools.size() <= componentId) // Not enough component pool
        {
            componentPools.resize(componentId + 1, nullptr);
        }
        if (componentPools[componentId] == nullptr) // New component, make a new pool
        {
            componentPools[componentId] = new MemoryPool<T>(MAX_ENTITIES);
        }

        // Looks up the component in the pool, and initializes it with placement new
        T *pComponent = new (componentPools[componentId]->get(id)) T();
        entities[id].mask.set(componentId);

        return pComponent;
    }

    template <typename T>
    T *get_component(EntityID id)
    {
        int componentId = get_id<T>();
        if (!entities[id].mask.test(componentId))
            return nullptr;

        T *pComponent = static_cast<T *>(componentPools[componentId]->get(id));
        return pComponent;
    }
};

#endif /* ECS_HPP */