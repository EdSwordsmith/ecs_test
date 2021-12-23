#ifndef ECS_HPP
#define ECS_HPP

#include <cassert>
#include <iostream>
#include <vector>

#include "memory/pool.hpp"

typedef unsigned long long EntityID;
const int START_ENTITIES = 2;
size_t componentCounter = 0;

struct Scene
{
    EntityID nextID = 0;
    EntityID currentCapacity = START_ENTITIES;

    std::vector<uint8_t> masks;

    std::vector<Memory::Pool<void>*> componentPools;

    template <class T> void register_component()
    {
        assert(nextID == 0);
        auto id = get_component_id<T>();
        if (id >= componentPools.size())
            componentPools.push_back(new Memory::Pool<T>(START_ENTITIES));
    }

    template <class T> size_t get_component_id()
    {
        static size_t componentId = componentCounter++;
        return componentId;
    }

    EntityID create()
    {
        if (nextID == 0)
        {
            masks.resize(START_ENTITIES * (1 + componentCounter / 8));
        }
        else if (nextID == currentCapacity)
        {
            for (auto& pool : componentPools)
            {
                pool->resize(currentCapacity);
            }

            currentCapacity *= 2;
            masks.resize(currentCapacity * (1 + componentCounter / 8));
        }

        EntityID id = nextID++;
        return id;
    }

    template <typename T> T* add_component(EntityID id)
    {
        size_t componentId = get_component_id<T>();
        assert(componentId < componentPools.size());

        T* pComponent = new (componentPools[componentId]->get(id)) T();
        masks[id * (1 + componentCounter / 8) + componentId / 8] =
            masks[id * (1 + componentCounter / 8) + componentId / 8] | (1 << (componentId % 8));

        return pComponent;
    }

    template <typename T> T* get_component(EntityID id)
    {
        size_t componentId = get_component_id<T>();

        if ((masks[id * (1 + componentCounter / 8) + componentId / 8] & (1 << componentId % 8)) == 0)
            return nullptr;

        T* pComponent = static_cast<T*>(componentPools[componentId]->get(id));
        return pComponent;
    }
};

template <typename... ComponentTypes> struct SceneView
{
    Scene* _scene;
    std::vector<uint8_t> mask;

    SceneView(Scene& scene) : _scene(&scene)
    {
        mask.resize(1 + componentCounter / 8);

        size_t componentIds[] = {_scene->get_component_id<ComponentTypes>()...};
        for (auto id : componentIds)
        {
            mask[id / 8] = mask[id / 8] | (1 << id % 8);
        }
    }

    struct Iterator
    {
        Scene* _scene;
        EntityID current;
        std::vector<uint8_t>* _mask;

        Iterator(Scene* scene, EntityID index, std::vector<uint8_t>& mask) : _scene(scene), current(index), _mask(&mask)
        {
        }

        EntityID operator*() const
        {
            return current;
        }

        bool operator==(const Iterator& other) const
        {
            return current == other.current || current == _scene->nextID;
        }

        bool operator!=(const Iterator& other) const
        {
            return current != other.current && current != _scene->nextID;
        }

        bool valid_id()
        {
            for (size_t i = 0; i < _mask->size(); i++)
            {
                if ((_mask->at(i) & _scene->masks[current * (1 + componentCounter / 8)]) != _mask->at(i))
                    return false;
            }

            return true;
        }

        Iterator& operator++()
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
            if ((mask.at(i) & _scene->masks[index * (1 + componentCounter / 8)]) != mask.at(i))
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
