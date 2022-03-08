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

#ifndef GRAPHICSLIB_CAMERA_HANDLE_HPP
#define GRAPHICSLIB_CAMERA_HANDLE_HPP

#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/SceneGraph/Camera.h>

// Inspired from https://github.com/alexesDev/magnum-tips
namespace graphics_lib {
    namespace cameras {
        template <size_t N>
        class CameraHandle : public SceneGraph::Object<std::conditional_t<N == 3, SceneGraph::MatrixTransformation3D, SceneGraph::MatrixTransformation2D>> {
        public:
            explicit CameraHandle(SceneGraph::Object<std::conditional_t<N == 3, SceneGraph::MatrixTransformation3D, SceneGraph::MatrixTransformation2D>>& object)
                : SceneGraph::Object<std::conditional_t<N == 3, SceneGraph::MatrixTransformation3D, SceneGraph::MatrixTransformation2D>>{&object}
            {
                if constexpr (N == 3) {
                    // Default 3D camera pose
                    arrayAppend(_pose, Corrade::InPlaceInit, Vector3{10., 0., 0.}); // pos
                    arrayAppend(_pose, Corrade::InPlaceInit, Vector3{0., 0., 0.}); // center
                    arrayAppend(_pose, Corrade::InPlaceInit, Vector3::zAxis()); // up

                    // Default speed
                    _speed = Vector2{-0.01, 0.01};

                    // Planes and FOV
                    _planes = Containers::optional(Containers::Array<Float>(2));
                    (*_planes)[0] = 0.01f;
                    (*_planes)[1] = 200.0f;
                    _fov = Containers::Optional<Rad>(Deg(60.0f));

                    // Init camera holding objects
                    arrayAppend(_objects, Corrade::InPlaceInit, new SceneGraph::Object<SceneGraph::MatrixTransformation3D>(this));
                    arrayAppend(_objects, Corrade::InPlaceInit, new SceneGraph::Object<SceneGraph::MatrixTransformation3D>(_objects[0]));
                    arrayAppend(_objects, Corrade::InPlaceInit, new SceneGraph::Object<SceneGraph::MatrixTransformation3D>(_objects[1]));
                    updatePose();

                    // Init camera
                    (*(_camera = new SceneGraph::Camera3D{*_objects[2]}))
                        .setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
                        .setProjectionMatrix(Matrix4::perspectiveProjection(*_fov, 1.0f, (*_planes)[0], (*_planes)[1]))
                        .setViewport(GL::defaultFramebuffer.viewport().size());
                }
                else {
                    arrayAppend(_objects, Corrade::InPlaceInit, new SceneGraph::Object<SceneGraph::MatrixTransformation2D>(this));
                    (*(_camera = new SceneGraph::Camera2D{*_objects[0]}))
                        .setProjectionMatrix(Matrix3::projection({10.0f, 10.0f}))
                        .setViewport(GL::defaultFramebuffer.viewport().size());
                }
            }

            Containers::Array<Math::Vector<N, Float>>& pose() { return _pose; }

            // Partial fill vector with variadic template and fold expression with callable
            // https://stackoverflow.com/questions/7230621/how-can-i-iterate-over-a-packed-variadic-template-argument-list
            template <typename... Args>
            void setPose(Args&&... args)
            {
                size_t i = 0;

                ([&](auto& arg) {
                    _pose[i] = arg;
                    ++i;
                }(args),
                    ...);

                updatePose();
            }

            CameraHandle& setSpeed(const Vector2& speed)
            {
                _speed = speed;
                return *this;
            }

            CameraHandle& move(const Magnum::Vector2i& shift)
            {
                if constexpr (N == 3) {
                    Vector2 s = Vector2{shift} * _speed;

                    // Yaw
                    _objects[0]->translate(-_pose[1]).rotate(Rad(s.x()), Vector3::zAxis(1)).translate(_pose[1]);

                    // Pitch
                    _objects[1]->translate(-_pose[1]).rotate(Rad(s.y()), Vector3::yAxis(-1)).translate(_pose[1]);
                }
                else {
                    std::cout << "hello" << std::endl;
                }

                return *this;
            }

            CameraHandle& translate(const Magnum::Float& shift)
            {
                if constexpr (N == 3) {
                    const Vector3 distance = _pose[1] - _objects[2]->transformation().translation();

                    _objects[2]->translate(distance * (1.0f - (shift > 0 ? 1 / 0.85f : 0.85f)));
                }
                else {
                    std::cout << "hello" << std::endl;
                }

                return *this;
            }

            /* Wrapped functions */
            CameraHandle& draw(SceneGraph::DrawableGroup<N, Float>& _group)
            {
                _camera->draw(_group);

                return *this;
            }

            inline Vector2i viewport() const { return _camera->viewport(); }

            CameraHandle& setViewport(const Vector2i& size)
            {
                _camera->setViewport(size);

                return *this;
            }

        protected:
            // Camera
            SceneGraph::Camera<N, Float>* _camera;

            // Camera holding objects [yaw, pitch, main]
            Containers::Array<SceneGraph::Object<std::conditional_t<N == 3, SceneGraph::MatrixTransformation3D, SceneGraph::MatrixTransformation2D>>*> _objects;

            // Camera pose [pos, center, up]
            Containers::Array<Math::Vector<N, Float>> _pose;

            // Camera view (only for 3D scene)
            Containers::Optional<Containers::Array<Float>> _planes; // [near, far]
            Containers::Optional<Rad> _fov;

            // Camera speed
            Vector2 _speed;

            void updatePose()
            {
                if constexpr (N == 3)
                    _objects[2]->setTransformation(Matrix4::lookAt(_pose[0], _pose[1], _pose[2]));
                else
                    std::cout << "Hello2" << std::endl;
            }
        };
    } // namespace cameras
} // namespace graphics_lib

#endif // GRAPHICSLIB_CAMERA_HANDLE_HPP