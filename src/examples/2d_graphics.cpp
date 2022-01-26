#include <science_graphics/ScienceGraphics.hpp>

using namespace science_graphics;

int main(int argc, char** argv)
{
    ScienceGraphics app({argc, argv});

    app.colorbar(0, 1);

    return app.exec();
}