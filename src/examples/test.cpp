#include <iostream>

#include <magnum_dynamics/MagnumApp.hpp>

using namespace magnum_dynamics;

int main(int argc, char** argv)
{
    MagnumApp app({argc, argv});

    // Eigen::Vector3f a{1.0f, 2.0f, 3.0f};
    // Vector3 b(a);

    app.add("cube", "", Matrix4::scaling({0.5f, 0.5f, 0.5f}) * Matrix4::translation({0.0f, -2.0f, 0.0f}), 0xff0000_rgbf);
    // app.add("cube", "", Matrix4::scaling({0.5f, 0.5f, 0.5f}) * Matrix4::translation({0.0f, 2.0f, 0.0f}), 0x00ff00_rgbf);

    auto* obj = app.manipulator().children().last();

    app.add("rsc/franka/link0.dae");

    int index = 0;
    for (Object3D* child = obj->nextSibling(); child; child = child->nextSibling()) {
        std::cout << index << std::endl;
        index++;
    }

    // auto* obj1 = new Object3D(&app.manipulator());

    // auto* obj2 = new Object3D(obj1);

    // auto* obj3 = new Object3D(obj2);

    // for (Object3D& child : app.manipulator().children()) {
    //     std::cout << index << std::endl;
    //     index++;
    // }

    return app.exec();
}