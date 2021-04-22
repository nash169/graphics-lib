#include <Eigen/Core>
#include <iostream>
#include <utils_cpp/UtilsCpp.hpp>

#include <magnum_dynamics/MagnumApp.hpp>

using namespace magnum_dynamics;

int main(int argc, char** argv)
{
    MagnumApp app({argc, argv});

    // Load mesh
    utils_cpp::FileManager io_manager;
    Eigen::MatrixXd vertices = io_manager.setFile("rsc/fem/sphere.msh").read<Eigen::MatrixXd>("$Nodes", 2),
                    indices = io_manager.read<Eigen::MatrixXd>("$Elements", 2).array() - 1;

    Eigen::VectorXd fun = Eigen::VectorXd::Random(vertices.rows()),
                    fun2 = Eigen::VectorXd::Random(9564); // Assimp importer increases the number of vertices for some reason

    // std::cout << vertices.rows() << std::endl;
    // std::cout << fun.rows() << std::endl;
    // std::cout << indices.rows() << std::endl;

    app.plot(vertices.block(0, 1, vertices.rows(), 3), fun, indices.block(0, 5, indices.rows(), 3))
        .setTransformation(Matrix4::translation({-2.0f, 0.0f, 0.0f}));

    app.plot("rsc/fem/sphere.stl", fun2)
        .setTransformation(Matrix4::translation({2.0f, 0.0f, 0.0f}));

    return app.exec();
}