#include <science_graphics/ScienceGraphics.hpp>

using namespace science_graphics;

int main(int argc, char** argv)
{
    ScienceGraphics app({argc, argv});

    std::string fname = (argc > 1) ? argv[1] : "rsc/franka/link0.dae";

    app.import(fname);

    app.addFrame()
        .setTransformation(Matrix4());

    return app.exec();
}