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

#ifndef SCIENCEGRAPHICS_PHONG_DRAWABLE_HPP
#define SCIENCEGRAPHICS_PHONG_DRAWABLE_HPP

#include "graphics_lib/drawbles/AbstractDrawable.hpp"
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Trade/PhongMaterialData.h>

namespace graphics_lib {
    namespace drawables {
        template <size_t N = 3>
        class PhongDrawable : public AbstractDrawable<N> {
        public:
            explicit PhongDrawable(SceneGraph::Object<std::conditional_t<N == 3, SceneGraph::MatrixTransformation3D, SceneGraph::MatrixTransformation2D>>& object, SceneGraph::DrawableGroup<N, Float>& group, Shaders::PhongGL& shader)
                : AbstractDrawable<N>(object, group),
                  _shader(shader) {}

            PhongDrawable& setMaterial(Trade::PhongMaterialData& material)
            {
                _material = std::move(material);
                return *this;
            }

            PhongDrawable& setColor(const Color4& color)
            {
                _color = std::move(color);
                return *this;
            }

        protected:
            // Material
            Containers::Optional<Trade::PhongMaterialData> _material;

            // Color
            Containers::Optional<Color4> _color;

        private:
            void draw(const typename std::conditional<N == 3, Matrix4, Matrix3>::type& transformationMatrix, SceneGraph::Camera<N, Float>& camera) override
            {
                auto transformation = transformationMatrix * AbstractDrawable<N>::_priorTransformation;

                if (_material)
                    _shader
                        .setAmbientColor(_material->ambientColor())
                        .setDiffuseColor(_material->diffuseColor())
                        .setSpecularColor(_material->specularColor())
                        .setShininess(_material->shininess())
                        // .setLightPositions({{camera.cameraMatrix().transformPoint({0.0f, 2.0f, 3.0f}), 0.0f},
                        //     {camera.cameraMatrix().transformPoint({0.0f, -2.0f, 3.0f}), 0.0f}})
                        .setTransformationMatrix(transformation)
                        .setNormalMatrix(transformation.normalMatrix())
                        .setProjectionMatrix(camera.projectionMatrix())
                        .draw(AbstractDrawable<N>::_mesh);
                // if color is present (but not texture and material) use color shader (Phong) with fewer color options
                else if (_color)
                    _shader
                        .setDiffuseColor(*_color)
                        // .setLightPositions({{camera.cameraMatrix().transformPoint({0.0f, 2.0f, 3.0f}), 0.0f},
                        //     {camera.cameraMatrix().transformPoint({0.0f, -2.0f, 3.0f}), 0.0f}})
                        .setTransformationMatrix(transformation)
                        .setNormalMatrix(transformation.normalMatrix())
                        .setProjectionMatrix(camera.projectionMatrix())
                        .draw(AbstractDrawable<N>::_mesh);
            }

            // Shaders
            Shaders::PhongGL& _shader;
        };

    } // namespace drawables
} // namespace graphics_lib

#endif // SCIENCEGRAPHICS_PHONG_DRAWABLE_HPP