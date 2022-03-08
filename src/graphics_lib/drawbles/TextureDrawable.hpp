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

#ifndef GRAPHICSLIB_TEXTURE_DRAWABLE_HPP
#define GRAPHICSLIB_TEXTURE_DRAWABLE_HPP

#include "graphics_lib/drawbles/AbstractDrawable.hpp"
#include <Magnum/GL/Texture.h>
#include <Magnum/Shaders/Phong.h>

namespace graphics_lib {
    namespace drawables {
        template <size_t N = 3>
        class TextureDrawable : public AbstractDrawable<N> {
        public:
            explicit TextureDrawable(SceneGraph::Object<std::conditional_t<N == 3, SceneGraph::MatrixTransformation3D, SceneGraph::MatrixTransformation2D>>& object, SceneGraph::DrawableGroup<N, Float>& group, Shaders::PhongGL& shader)
                : AbstractDrawable<N>(object, group),
                  _shader(shader) {}

            TextureDrawable& setTexture(GL::Texture2D& texture)
            {
                _texture = std::move(texture);
                return *this;
            }

        protected:
            // Texture
            GL::Texture2D _texture;

        private:
            void draw(const std::conditional_t<N == 3, Matrix4, Matrix3>& transformationMatrix, SceneGraph::Camera<N, Float>& camera) override
            {
                auto transformation = transformationMatrix * AbstractDrawable<N>::_priorTransformation;

                // Use texture shader (Phong::Flag::DiffuseTexture)
                _shader
                    // .setLightPositions({{camera.cameraMatrix().transformPoint({0.0f, 2.0f, 3.0f}), 0.0f},
                    //     {camera.cameraMatrix().transformPoint({0.0f, -2.0f, 3.0f}), 0.0f}})
                    .setTransformationMatrix(transformation)
                    .setNormalMatrix(transformation.normalMatrix())
                    .setProjectionMatrix(camera.projectionMatrix())
                    .bindDiffuseTexture(_texture)
                    .draw(AbstractDrawable<N>::_mesh);
            }

            // Shaders
            Shaders::PhongGL& _shader;
        };

    } // namespace drawables
} // namespace graphics_lib

#endif // GRAPHICSLIB_TEXTURE_DRAWABLE_HPP