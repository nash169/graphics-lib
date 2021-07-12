#ifndef MAGNUM_DYNAMICS_CAMERA_HPP
#define MAGNUM_DYNAMICS_CAMERA_HPP

#include <Magnum/GL/DefaultFramebuffer.h>

#include "magnum_dynamics/types.hpp"

#include <iostream>

// Check https://github.com/alexesDev/magnum-tips for additional tools

namespace magnum_dynamics {
    using namespace Magnum;

    class Camera : public Object3D {
    public:
        explicit Camera(Object3D& object) : Object3D{&object}
        {
            // Camera motion speed
            _speed = Vector2{-0.01f, 0.01f};

            // Default camera pos
            Vector3 pos{10., 0., 0.};

            // World center in 0,0,0
            _center = Vector3{0., 0., 0.};

            // Camera x-axes
            _front = (_center - pos).normalized();

            // Camera z-axes
            _up = Vector3::zAxis();

            // Camera y-axes
            _right = Math::cross(_front, _up).normalized();

            _yawObject = new Object3D{this};
            _pitchObject = new Object3D{_yawObject};
            _cameraObject = new Object3D{_pitchObject};
            _cameraObject->setTransformation(Matrix4::lookAt(pos, _center, _up));

            _fov = Magnum::Deg(60.0f);
            _nearPlane = 0.01f;
            _farPlane = 200.f;

            (*(_camera = new SceneGraph::Camera3D{*_cameraObject}))
                .setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
                .setProjectionMatrix(Matrix4::perspectiveProjection(_fov, 1.0f, _nearPlane, _farPlane))
                .setViewport(GL::defaultFramebuffer.viewport().size());
        }

        Camera3D* camera() const
        {
            return _camera;
        }

        Object3D* object() const
        {
            return _cameraObject;
        }

        Camera& setPose(const Vector3& pos)
        {
            _pose = pos;

            _cameraObject->setTransformation(Matrix4::lookAt(pos, _center, _up));

            return *this;
        }

        Camera& setSpeed(const Vector2& speed)
        {
            _speed = speed;
            return *this;
        }

        Camera& setCenter(const Vector3& center)
        {
            _center = center;
            return *this;
        }

        Camera& move(const Magnum::Vector2i& shift)
        {
            Vector2 s = Vector2{shift} * _speed;

            // _yawObject->rotate(Rad(s.x()), (_center + Vector3::zAxis(1)).normalized());
            // _pitchObject->rotate(Rad(s.y()), (_center + Vector3::yAxis(1)).normalized());

            _yawObject->translate(-_center).rotate(Rad(s.x()), Vector3::zAxis(1)).translate(_center);
            _pitchObject->translate(-_center).rotate(Rad(s.y()), Vector3::yAxis(-1)).translate(_center);

            // _yawObject->rotate(Rad(s.x()), Vector3::zAxis(1));
            // _pitchObject->rotate(Rad(s.y()), Vector3::yAxis(1));

            return *this;
        }

        Camera& translate(const Magnum::Float& shift)
        {
            const Vector3 distance = _center - _cameraObject->transformation().translation();

            _cameraObject->translate(distance * (1.0f - (shift > 0 ? 1 / 0.85f : 0.85f)));

            return *this;
        }

    protected:
        // Object holding the camera
        Object3D *_yawObject, *_pitchObject, *_cameraObject;

        // Camera
        SceneGraph::Camera3D* _camera;

        // Camera parameters
        Vector2 _speed;
        Vector3 _center, _up, _front, _right, _pose;
        Float _aspectRatio, _nearPlane, _farPlane;
        Rad _fov;
    };
} // namespace magnum_dynamics

#endif // MAGNUM_DYNAMICS_CAMERA_HPP