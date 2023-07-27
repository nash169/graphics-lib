/*
    This file is part of graphics-lib.

    Copyright (c) 2020, 2021, 2022 Bernardo Fichera <bernardo.fichera@gmail.com>

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#include <graphics_lib/Graphics.hpp>
#include <utils_lib/FileManager.hpp>

using namespace graphics_lib;
using namespace utils_lib;

int main(int argc, char** argv)
{
    Graphics app({argc, argv});

    auto prior = Matrix4(Vector4{1, 0, 0, 0},
        Vector4{0, 0, 1, 0},
        Vector4{0, -1, 0, 0},
        Vector4{0, 0, 0, 1});

    std::string fname = (argc > 1) ? argv[1] : "rsc/franka/link5.dae";

    // app.import(fname).setTransformation(Matrix4::translation(Vector3{0.0, -1.0, 0.0}));
    // app.import(fname).addPriorTransformation(prior);
    // app.import2(fname).addPriorTransformation(prior);

    // app.addFrame()
    //     .setTransformation(prior);

    FileManager mng;
    Eigen::MatrixXd traj = mng.setFile("rsc/data/trajectory.csv").read<Eigen::MatrixXd>();

    // std::cout << traj << std::endl;

    // Eigen::MatrixXd traj(2, 3);
    // traj << 0, 0, 0,
    //     1, 1, 1;

    app.addTrajectory(traj);

    // app.addPrimitive("cube");

    // app.addFrame();

    return app.exec();
}