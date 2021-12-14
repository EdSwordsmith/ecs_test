#include "ecs.hpp"

struct Component
{

};

struct Transform : public Component
{
    float x, y, z;
};

struct Color : public Component
{
    int r, g, b;
};

int main()
{
    std::vector<Component> components;
    Color c;
    c.r = 255;

    std::cout << "Transform Id: " << get_id<Transform>() << std::endl;
    std::cout << "Color Id: " << get_id<Color>() << std::endl;

    Scene scene;
    EntityID entity = scene.create();
    Transform *transform = scene.add_component<Transform>(entity);
    transform->x = 5;
    std::cout << scene.entities[entity].mask << std::endl;
    std::cout << scene.get_component<Transform>(entity)->x << std::endl;
}
