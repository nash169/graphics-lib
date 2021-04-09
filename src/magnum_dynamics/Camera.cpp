#include "magnum_dynamics/Camera.hpp"

namespace magnum_dynamics {
    Camera::Camera(Object3D& object) : Object3D{&object}
    {
        Vector3 center{0., 0., 0.};
        Vector3 camera{0., 5., 5.};
        _front = (center - camera).normalized();
        _up = Vector3::zAxis();
        _right = Math::cross(_front, _up).normalized();

        _cameraObject = new Object3D{this};
        _cameraObject->setTransformation(Matrix4::lookAt(camera, center, _up));
        // _cameraObject->translate(Vector3::zAxis(5.0f));

        _fov = Magnum::Deg(60.0f);
        // _aspectRatio = width / static_cast<Float>(height);
        _nearPlane = 0.01f;
        _farPlane = 100.f;
        // _width = width;
        // _height = height;

        (*(_camera = new SceneGraph::Camera3D{*_cameraObject}))
            .setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
            .setProjectionMatrix(Matrix4::perspectiveProjection(_fov, 1.0f, _nearPlane, _farPlane))
            .setViewport(GL::defaultFramebuffer.viewport().size());
    }

    Camera::~Camera()
    {
    }

    Camera3D* Camera::camera() const
    {
        return _camera;
    }
} // namespace magnum_dynamics
