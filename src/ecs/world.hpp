#ifndef ECS_WORLD_HPP
#define ECS_WORLD_HPP

#include <cassert>
#include <cinttypes>

#include "storage.hpp"

class World {
  private:
    std::vector<std::uint8_t> _masks;
    std::vector<IStorage *> _storages;
    std::vector<void *> _resources;

    size_t _next_entity_id = 0;
    size_t _bytes_per_mask;

    template <typename T> size_t get_component_id() {
        static size_t id = _storages.size();
        return id;
    }

  public:
    ~World() {
        for (auto *storage : _storages) {
            delete storage;
        }

        for (auto *resource : _resources) {
            delete resource;
        }
    }

    template <typename T> size_t register_component(Storage<T> *storage) {
        assert(_masks.size() == 0);
        size_t component_id = get_component_id<T>();
        _storages.push_back(storage);
        return component_id;
    }

    template <typename T> T *get_resource() {
        static size_t id = _resources.size();
        if (id >= _resources.size())
            _resources.push_back(new T());
        return (T *)_resources[id];
    }

    size_t create() {
        if (_masks.size() == 0) {
            _bytes_per_mask = (7 + _storages.size()) / 8;
        }

        size_t id = _next_entity_id++;
        for (size_t i = 0; i < _bytes_per_mask; i++) {
            uint8_t n = 0;
            _masks.push_back(n);
        }

        return id;
    }

    template <typename T> T *add_component(size_t entity, T value) {
        size_t component_id = get_component_id<T>();
        Storage<T> *storage = (Storage<T> *)_storages[component_id];
        // Set the entity mask for this component
        _masks[entity * _bytes_per_mask + component_id / 8] |= 1 << (component_id % 8);

        return storage->insert(entity, value);
    }

    template <typename T> T *get_component(size_t entity) {
        size_t component_id = get_component_id<T>();

        if ((_masks[entity * _bytes_per_mask + component_id / 8] & (1 << (component_id % 8))) == 0)
            return nullptr;

        Storage<T> *storage = (Storage<T> *)_storages[component_id];
        return storage->get(entity);
    }

    template <typename... ComponentTypes>
    friend class WorldView;
};

template <typename... ComponentTypes> struct WorldView {
    World *_world;
    std::vector<uint8_t> _mask;

    WorldView(World &world) : _world(&world) {
        _mask.resize(_world->_bytes_per_mask);
        size_t componentIds[] = {_world->get_component_id<ComponentTypes>()...};
        for (auto id : componentIds) {
            _mask[id / 8] |= 1 << (id % 8);
        }
    }

    struct Iterator {
        World *_world;
        size_t current;
        std::vector<uint8_t> *_mask;

        Iterator(World *world, size_t index, std::vector<uint8_t> *mask)
            : _world(world), current(index), _mask(mask) {}

        size_t operator*() const { return current; }

        bool operator==(const Iterator &other) const {
            return current == other.current || current == _world->_next_entity_id;
        }

        bool operator!=(const Iterator &other) const {
            return current != other.current && current != _world->_next_entity_id;
        }

        bool valid_id() {
            for (size_t i = 0; i < _mask->size(); i++) {
                if ((_mask->at(i) & _world->_masks[current * _world->_bytes_per_mask + i]) != _mask->at(i))
                    return false;
            }
            return true;
        }

        Iterator &operator++() {
            do {
                current++;
            } while (current < _world->_next_entity_id && !valid_id());
            return *this;
        }
    };

    bool valid_index(size_t index) {
        for (size_t i = 0; i < _mask.size(); i++) {
            if ((_mask[i] & _world->_masks[index * _world->_bytes_per_mask + i]) != _mask[i])
                return false;
        }

        return true;
    }

    Iterator begin() {
        size_t firstIndex = 0;
        while (firstIndex < _world->_next_entity_id && !valid_index(firstIndex)) {
            firstIndex++;
        }

        Iterator it(_world, firstIndex, &_mask);
        return it;
    }

    Iterator end() {
        Iterator it(_world, _world->_next_entity_id, &_mask);
        return it;
    }
};

#endif /* ECS_WORLD_HPP */
