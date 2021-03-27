#ifndef MAGNUM_DYNAMICS_CAMERA_HPP
#define MAGNUM_DYNAMICS_CAMERA_HPP

#include <Magnum/GL/DefaultFramebuffer.h>

#include "magnum_dynamics/types.hpp"

namespace magnum_dynamics {
    using namespace Magnum;

    class Camera : public Object3D {
    public:
        explicit Camera(Object3D& object);

        ~Camera();

        Camera3D* camera() const;

        Object3D* object() const
        {
            return _cameraObject;
        }

    protected:
        // Object holding the camera
        Object3D* _cameraObject;

        // Camera
        SceneGraph::Camera3D* _camera;

        // Camera parameters
        Vector3 _up, _front, _right;
        Float _aspectRatio, _nearPlane, _farPlane;
        Rad _fov;
        Int _width, _height;
    };
} // namespace magnum_dynamics

#endif // MAGNUM_DYNAMICS_CAMERA_HPP