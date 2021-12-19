#include "ecs.hpp"

struct Transform
{
    float x, y, z;
};

struct Color
{
    int r, g, b;
};

int main()
{
    Scene scene;
    scene.register_component<Transform>();
    scene.register_component<Color>();

    std::cout << "Transform Id: " << scene.get_component_id<Transform>() << std::endl;
    std::cout << "Color Id: " << scene.get_component_id<Color>() << std::endl;

    EntityID entity = scene.create();
    Transform *transform = scene.add_component<Transform>(entity);
    transform->x = 5;

    std::cout << scene.get_component<Transform>(entity)->x << std::endl;
}
