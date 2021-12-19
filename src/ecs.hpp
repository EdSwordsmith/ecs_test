#ifndef ECS_HPP
#define ECS_HPP

#include <cassert>
#include <iostream>
#include <vector>
#include <bitset>

#include "memory/pool.hpp"

typedef unsigned long long EntityID;
const int START_ENTITIES = 2;
int componentCounter;

struct Scene
{
    EntityID nextID = 0;
    EntityID currentCapacity = START_ENTITIES;

    std::vector<bool> masks;

    std::vector<Memory::Pool<void> *> componentPools;

    template <class T>
    void register_component()
    {
        assert(nextID == 0);
        auto id = get_component_id<T>();
        if (id >= componentPools.size())
            componentPools.push_back(new Memory::Pool<T>(START_ENTITIES));
    }

    template <class T>
    int get_component_id()
    {
        static int componentId = componentCounter++;
        return componentId;
    }

    EntityID create()
    {
        if (nextID == 0)
        {
            masks.resize(START_ENTITIES * componentCounter);
        }
        else if (nextID == currentCapacity)
        {
            for (auto &pool: componentPools)
            {
                pool->resize(currentCapacity);
            }

            currentCapacity *= 2;
        }

        EntityID id = nextID++;
        return id;
    }

    template <typename T>
    T *add_component(EntityID id)
    {
        int componentId = get_component_id<T>();
        assert(componentId < componentPools.size());

        T *pComponent = new (componentPools[componentId]->get(id)) T();
        masks[id * componentCounter + componentId] = true;

        return pComponent;
    }

    template <typename T>
    T *get_component(EntityID id)
    {
        int componentId = get_component_id<T>();

        if (!masks[componentCounter * id + componentId])
            return nullptr;

        T *pComponent = static_cast<T *>(componentPools[componentId]->get(id));
        return pComponent;
    }
};

#endif /* ECS_HPP */
