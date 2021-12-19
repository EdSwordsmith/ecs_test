#ifndef ECS_HPP
#define ECS_HPP

#include <cassert>
#include <iostream>
#include <vector>
#include <bitset>

#include "memory/pool.hpp"

typedef unsigned long long EntityID;
const int MAX_ENTITIES = 32;
typedef std::vector<bool> ComponentMask;

int componentCounter;

struct Scene
{
    EntityID nextID = 0;
    std::vector<bool> entityMasks;

    std::vector<Memory::Pool<void> *> componentPools;

    template <class T>
    void register_component()
    {
        assert(nextID == 0);
        auto id = get_component_id<T>();
        if (id >= componentPools.size())
            componentPools.push_back(new Memory::Pool<T>(MAX_ENTITIES));
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
            entityMasks.resize(MAX_ENTITIES * componentCounter);
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
        entityMasks[id * componentCounter + componentId] = true;

        return pComponent;
    }

    template <typename T>
    T *get_component(EntityID id)
    {
        int componentId = get_component_id<T>();

        if (!entityMasks[componentCounter * id + componentId])
            return nullptr;

        T *pComponent = static_cast<T *>(componentPools[componentId]->get(id));
        return pComponent;
    }
};

#endif /* ECS_HPP */
