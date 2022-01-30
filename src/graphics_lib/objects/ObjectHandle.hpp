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

#ifndef SCIENCEGRAPHICS_OBJECT_HANDLE_HPP
#define SCIENCEGRAPHICS_OBJECT_HANDLE_HPP

#include <Magnum/GL/Mesh.h>
#include <Magnum/SceneGraph/Object.hpp>

#include "graphics_lib/drawbles/ColorDrawable.hpp"
#include "graphics_lib/drawbles/Drawables.h"
#include "graphics_lib/drawbles/PhongDrawable.hpp"
#include "graphics_lib/drawbles/TextureDrawable.hpp"

namespace graphics_lib {
    namespace objects {
        template <size_t N = 3>
        class ObjectHandle : public SceneGraph::Object<typename std::conditional<N == 3, SceneGraph::MatrixTransformation3D, SceneGraph::MatrixTransformation2D>::type> {
        public:
            ObjectHandle(SceneGraph::Object<typename std::conditional<N == 3, SceneGraph::MatrixTransformation3D, SceneGraph::MatrixTransformation2D>::type>* object, std::unordered_map<ObjectHandle<N>*, Containers::Pointer<drawables::AbstractDrawable<N>>>& drawableObj)
                : SceneGraph::Object<typename std::conditional<N == 3, SceneGraph::MatrixTransformation3D, SceneGraph::MatrixTransformation2D>::type>{object},
                  _drawableObjects(drawableObj) {}

            ObjectHandle<N>& setMesh(GL::Mesh& mesh)
            {
                if (_drawableObjects.find(this) == _drawableObjects.end()) {
                    for (auto& child : this->children())
                        static_cast<ObjectHandle<N>&>(child).setMesh(mesh);
                }
                else
                    _drawableObjects[this]->setMesh(mesh);

                return *this;
            }

            ObjectHandle<N>& setTexture(GL::Texture2D& texture)
            {
                if (_drawableObjects.find(this) == _drawableObjects.end()) {
                    for (auto& child : this->children())
                        static_cast<ObjectHandle<N>&>(child).setTexture(texture);
                }
                else
                    static_cast<drawables::TextureDrawable3D*>(_drawableObjects[this].get())->setTexture(texture);

                return *this;
            }

            ObjectHandle<N>& setMaterial(Trade::PhongMaterialData& material)
            {
                if (_drawableObjects.find(this) == _drawableObjects.end()) {
                    for (auto& child : this->children())
                        static_cast<ObjectHandle<N>&>(child).setMaterial(material);
                }
                else
                    static_cast<drawables::PhongDrawable3D*>(_drawableObjects[this].get())->setMaterial(material);

                return *this;
            }

            ObjectHandle<N>& setColor(const Color4& color)
            {
                if (_drawableObjects.find(this) == _drawableObjects.end()) {
                    for (auto& child : this->children())
                        static_cast<ObjectHandle<N>&>(child).setColor(color);
                }
                else
                    static_cast<drawables::PhongDrawable3D*>(_drawableObjects[this].get())->setColor(color);

                return *this;
            }

            ObjectHandle<N>& addPriorTransformation(const std::conditional_t<N == 3, Matrix4, Matrix3>& transformation)
            {
                if (_drawableObjects.find(this) == _drawableObjects.end()) {
                    for (auto& child : this->children())
                        static_cast<ObjectHandle<N>&>(child).addPriorTransformation(transformation);
                }
                else
                    _drawableObjects[this]->addPriorTransformation(transformation);

                return *this;
            }

            bool isDrawable() { return (_drawableObjects.find(this) == _drawableObjects.end()) ? false : true; }

        private:
            std::unordered_map<ObjectHandle<N>*, Containers::Pointer<drawables::AbstractDrawable<N>>>& _drawableObjects;
        };
    } // namespace objects
} // namespace graphics_lib

#endif // SCIENCEGRAPHICS_OBJECT_HANDLE_HPP