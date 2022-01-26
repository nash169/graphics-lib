#include <science_graphics/ScienceGraphics.hpp>
#include <utils_cpp/FileManager.hpp>

using namespace science_graphics;
using namespace utils_cpp;

int main(int argc, char** argv)
{
    ScienceGraphics app({argc, argv});

    // Load mesh
    FileManager io_manager;
    Eigen::MatrixXd vertices = io_manager.setFile("rsc/fem/armadillo.msh").read<Eigen::MatrixXd>("$Nodes", 2, "$EndNodes"),
                    indices = io_manager.read<Eigen::MatrixXd>("$Elements", 2, "$EndElements").array() - 1;

    Eigen::VectorXd fun = Eigen::VectorXd::Random(vertices.rows());

    app.surf(vertices.block(0, 1, vertices.rows(), 3), fun, indices.block(0, 5, indices.rows(), 3))
        .setTransformation(Matrix4::scaling({0.05, 0.05, 0.05}));

    return app.exec();
}