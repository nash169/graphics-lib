#ifndef SCIENCEGRAPHICS_TOOLS_MATH_HPP
#define SCIENCEGRAPHICS_TOOLS_MATH_HPP

#include <Eigen/Core>

namespace science_graphics {
    namespace tools {
        inline Eigen::VectorXi linearMap(const Eigen::VectorXd x, const double& min, const double& max, size_t n)
        {
            double q = abs(max - min) / n;

            return ((x.array() / q) + ceil(n / 2)).cast<int>();
        }
    } // namespace tools
} // namespace science_graphics

#endif // SCIENCEGRAPHICS_UTILS_MATH_HPP