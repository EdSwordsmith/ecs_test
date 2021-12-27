#include <iostream>

#include "ecs/world.hpp"

struct Player {};

struct Position {
    float x, y, z;
};

struct Velocity {
    float x, y, z;
};

int main() {
    World world;
    world.register_component<Player>(new NullStorage<Player>());
    world.register_component<Position>(new VecStorage<Position>());
    world.register_component<Velocity>(new MapStorage<Velocity>());

    auto player = world.create();
    world.add_component<Position>(player, (Position){0, 5, 1});
    world.add_component<Velocity>(player, (Velocity){0, 3, 1});
    world.add_component<Player>(player, (Player){});

    auto e2 = world.create();
    world.add_component<Position>(e2, (Position){0, 8, 7});

    auto e3 = world.create();
    world.add_component<Position>(e3, (Position){0, 15, 7});
    world.add_component<Velocity>(e3, (Velocity){0, 4, 1});

    auto e4 = world.create();
    world.add_component<Position>(e4, (Position){0, 12, 7});
    world.add_component<Velocity>(e4, (Velocity){0, 5, 1});

    for (auto entity : WorldView<Position, Velocity>(world)) {
        auto *pos = world.get_component<Position>(entity);
        std::cout << pos->y << std::endl;
    }

    for (auto entity : WorldView<Player, Position, Velocity>(world)) {
        auto *pos = world.get_component<Position>(entity);
        std::cout << pos->y << std::endl;
    }
}
