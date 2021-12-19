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

    auto e1 = scene.create();
    auto e2 = scene.create();
    auto e3 = scene.create();
    auto e4 = scene.create();

    auto *t1 = scene.add_component<Transform>(e1);
    scene.add_component<Transform>(e2);
    auto *t4 = scene.add_component<Transform>(e4);

    auto *c1 = scene.add_component<Color>(e1);
    scene.add_component<Color>(e3);
    scene.add_component<Color>(e4);

    t1->x = 1;
    t1->y = 2;
    t4->z = 5;
    c1->r = 255;

    for (EntityID entity : SceneView<Transform, Color>(scene))
    {
        auto *t = scene.get_component<Transform>(entity);
        auto *c = scene.get_component<Color>(entity);

        std::cout << "Transform: " << t->x << " " << t->y << " " << t->z << std::endl;
        std::cout << "Color: " << c->r << " " << c->g << " " << c->b << std::endl;
    }
}
