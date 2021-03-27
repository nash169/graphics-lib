
#ifndef MAGNUM_DYNAMICS_TYPES_HPP
#define MAGNUM_DYNAMICS_TYPES_HPP

#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Scene.h>

namespace magnum_dynamics {
    using Object3D = Magnum::SceneGraph::Object<Magnum::SceneGraph::MatrixTransformation3D>;
    using Scene3D = Magnum::SceneGraph::Scene<Magnum::SceneGraph::MatrixTransformation3D>;
    using Camera3D = Magnum::SceneGraph::Camera3D;
} // namespace magnum_dynamics

#endif // MAGNUM_DYNAMICS_TYPES_HPP