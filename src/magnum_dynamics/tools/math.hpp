#ifndef MAGNUM_DYNAMICS_TOOLS_MATH_HPP
#define MAGNUM_DYNAMICS_TOOLS_MATH_HPP

#include <Eigen/Core>

namespace magnum_dynamics {
    namespace tools {
        inline Eigen::VectorXi mapColors(const Eigen::VectorXd x, const double& min, const double& max, size_t n)
        {
            double q = abs(max - min) / n;

            return ((x.array() / q) + ceil(n / 2)).cast<int>();
        }
    } // namespace tools
} // namespace magnum_dynamics

#endif // MAGNUM_DYNAMICS_UTILS_MATH_HPP