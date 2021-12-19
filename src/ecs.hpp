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
            for (auto &pool : componentPools)
            {
                pool->resize(currentCapacity);
            }

            currentCapacity *= 2;
            masks.resize(currentCapacity * componentCounter);
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

template <typename... ComponentTypes>
struct SceneView
{
    Scene *_scene;
    std::vector<bool> mask;

    SceneView(Scene &scene) : _scene(&scene)
    {
        mask.resize(componentCounter);

        int componentIds[] = {_scene->get_component_id<ComponentTypes>()...};
        for (auto id : componentIds)
        {
            mask[id] = true;
        }
    }

    struct Iterator
    {
        Scene *_scene;
        EntityID current;
        std::vector<bool> *_mask;

        Iterator(Scene *scene, EntityID index, std::vector<bool> &mask)
            : _scene(scene), current(index), _mask(&mask) {}

        EntityID operator*() const
        {
            return current;
        }

        bool operator==(const Iterator &other) const
        {
            return current == other.current || current == _scene->nextID;
        }

        bool operator!=(const Iterator &other) const
        {
            return current != other.current && current != _scene->nextID;
        }

        bool valid_id()
        {
            for (size_t i = 0; i < _mask->size(); i++)
            {
                if (_mask->at(i) && !_scene->masks[current * componentCounter + i])
                    return false;
            }

            return true;
        }

        Iterator &operator++()
        {
            do
            {
                current++;
            } while (current < _scene->nextID && !valid_id());
            return *this;
        }
    };

    bool valid_index(EntityID index)
    {
        for (size_t i = 0; i < mask.size(); i++)
        {
            if (mask[i] && !_scene->masks[index * componentCounter + i])
                return false;
        }

        return true;
    }

    Iterator begin()
    {
        EntityID firstIndex = 0;
        while (firstIndex < _scene->nextID && !valid_index(firstIndex))
        {
            firstIndex++;
        }

        Iterator it(_scene, firstIndex, mask);
        return it;
    }

    Iterator end()
    {
        Iterator it(_scene, _scene->nextID, mask);
        return it;
    }
};

#endif /* ECS_HPP */
