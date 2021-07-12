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
    Eigen::MatrixXd vertices = io_manager.setFile("rsc/fem/armadillo.msh").read<Eigen::MatrixXd>("$Nodes", 2, "$EndNodes"),
                    indices = io_manager.read<Eigen::MatrixXd>("$Elements", 2, "$EndElements").array() - 1;

    Eigen::VectorXd fun = Eigen::VectorXd::Random(vertices.rows()),
                    fun2 = Eigen::VectorXd::Random(9564); // Assimp importer increases the number of vertices for some reason

    // std::cout << vertices.rows() << std::endl;
    // std::cout << fun.rows() << std::endl;
    // std::cout << indices.rows() << std::endl;

    Eigen::VectorXd ground = io_manager.setFile("rsc/data/ground_truth.csv").read<Eigen::MatrixXd>();

    app.plot(vertices.block(0, 1, vertices.rows(), 3), fun, indices.block(0, 5, indices.rows(), 3));
    // .setTransformation(Matrix4::scaling({0.05, 0.05, 0.05}));
    // .setTransformation(Matrix4::translation({-2.0f, 0.0f, 0.0f}));

    // app.plot("rsc/fem/sphere.stl", fun2)
    //     .setTransformation(Matrix4::translation({2.0f, 0.0f, 0.0f}));

    Eigen::Vector3f center = vertices.block(0, 1, vertices.rows(), 3).colwise().mean().cast<float>() * 0.05;
    (*app.camera())
        .setCenter(Vector3(center))
        .setPose({10., 0., 5.});

    // app.addFrame().setTransformation(Matrix4::translation(Vector3(center)));

    app.addPrimitive("cube").setTransformation(Matrix4::translation(Vector3(center) + Vector3{5, 0, 0}));

    app.manipulator().setPrimitiveTransformation(Matrix4::scaling({0.05, 0.05, 0.05}));

    return app.exec();
}