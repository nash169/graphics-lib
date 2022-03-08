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

#ifndef GRAPHICSLIB_COLOR_DRAWABLE_HPP
#define GRAPHICSLIB_COLOR_DRAWABLE_HPP

#include "graphics_lib/drawbles/AbstractDrawable.hpp"
#include <Magnum/Shaders/VertexColorGL.h>

namespace graphics_lib {
    namespace drawables {
        template <size_t N = 3>
        class ColorDrawable : public AbstractDrawable<N> {
        public:
            explicit ColorDrawable(SceneGraph::Object<typename std::conditional<N == 3, SceneGraph::MatrixTransformation3D, SceneGraph::MatrixTransformation2D>::type>& object, SceneGraph::DrawableGroup<N, Float>& group, Shaders::VertexColorGL<N>& shader)
                : AbstractDrawable<N>(object, group),
                  _shader(shader) {}

        private:
            void draw(const typename std::conditional<N == 3, Matrix4, Matrix3>::type& transformationMatrix, SceneGraph::Camera<N, Float>& camera) override
            {
                _shader
                    .setTransformationProjectionMatrix(camera.projectionMatrix() * transformationMatrix * AbstractDrawable<N>::_priorTransformation)
                    .draw(AbstractDrawable<N>::_mesh);
            }

            // Shaders
            Shaders::VertexColorGL<N>& _shader;
        };

    } // namespace drawables
} // namespace graphics_lib

#endif // GRAPHICSLIB_COLOR_DRAWABLE_HPP