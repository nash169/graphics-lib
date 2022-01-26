#ifndef SCIENCEGRAPHICS_OBJECT_HANDLE_HPP
#define SCIENCEGRAPHICS_OBJECT_HANDLE_HPP

#include <Magnum/GL/Mesh.h>
#include <Magnum/SceneGraph/Object.hpp>

#include "science_graphics/drawbles/ColorDrawable.hpp"
#include "science_graphics/drawbles/Drawables.h"
#include "science_graphics/drawbles/PhongDrawable.hpp"
#include "science_graphics/drawbles/TextureDrawable.hpp"

namespace science_graphics {
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
} // namespace science_graphics

#endif // SCIENCEGRAPHICS_OBJECT_HANDLE_HPP