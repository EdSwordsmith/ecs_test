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

    EntityID e2 = scene.create();
    auto *c2 = scene.add_component<Color>(e2);
    EntityID e3 = scene.create();
    auto *t3 = scene.add_component<Transform>(e3);
    c2->g = 255;
    t3->z = -4;

    std::cout << scene.currentCapacity << std::endl;
    std::cout << c2->g << std::endl;
    std::cout << t3->z << std::endl;
}
