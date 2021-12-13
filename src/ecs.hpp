#ifndef ECS_HPP
#define ECS_HPP

#include <iostream>
#include <vector>

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

struct ComponentPool
{
    char *pData = nullptr;
    size_t size = 0;

    ComponentPool(size_t _size)
    {
        // We'll allocate enough memory to hold MAX_ENTITIES, each with element size
        size = _size;
        pData = new char[this->size * MAX_ENTITIES];
    }

    ~ComponentPool()
    {
        delete[] pData;
    }

    inline void *get(size_t index)
    {
        // looking up the component at the desired index
        return pData + index * size;
    }
};

struct Scene
{
    struct EntityDesc
    {
        EntityID id;
        ComponentMask mask;
    };

    std::vector<EntityDesc> entities;
    std::vector<ComponentPool *> componentPools;

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
            componentPools[componentId] = new ComponentPool(sizeof(T));
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