#ifndef MAGNUM_DYNAMICS_OBJECT_HPP
#define MAGNUM_DYNAMICS_OBJECT_HPP

#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <unordered_map>

#include "magnum_dynamics/DrawableObject.hpp"
#include "magnum_dynamics/types.hpp"

#include <iostream>
#include <memory>

using namespace Magnum;

namespace magnum_dynamics {

    class Object : public Object3D {
    public:
        Object(Object3D* object,
            std::unordered_map<Object*, Containers::Pointer<DrawableObject>>& drawableObj)
            : Object3D{object}, _drawableObjects(drawableObj) {}

        // /* Object tranformation */
        // Object& setTransformation(const Matrix4& transformation)
        // {
        //     return static_cast<Object&>(Object3D::setTransformation(transformation));
        // }

        /* Each object has access to the unordered map of drawables. 
           Not each object is drawable; in this case it recursively 
           search for drawble children to which apply the transformation. */

        Object& setMesh(GL::Mesh& mesh)
        {
            if (_drawableObjects.find(this) == _drawableObjects.end()) {
                for (auto& child : this->children())
                    static_cast<Object&>(child).setMesh(mesh);
            }
            else
                _drawableObjects[this]->setMesh(mesh);

            return *this;
        }

        Object& setTexture(GL::Texture2D& texture)
        {
            if (_drawableObjects.find(this) == _drawableObjects.end()) {
                for (auto& child : this->children())
                    static_cast<Object&>(child).setTexture(texture);
            }
            else
                _drawableObjects[this]->setTexture(texture);

            return *this;
        }

        Object& setMaterial(Trade::PhongMaterialData& material)
        {
            if (_drawableObjects.find(this) == _drawableObjects.end()) {
                for (auto& child : this->children())
                    static_cast<Object&>(child).setMaterial(material);
            }
            else
                _drawableObjects[this]->setMaterial(material);

            return *this;
        }

        Object& setColor(const Color4& color)
        {
            if (_drawableObjects.find(this) == _drawableObjects.end()) {
                for (auto& child : this->children())
                    static_cast<Object&>(child).setColor(color);
            }
            else
                _drawableObjects[this]->setColor(color);

            return *this;
        }

        Object& addPriorTransformation(const Matrix4& transformation)
        {
            if (_drawableObjects.find(this) == _drawableObjects.end()) {
                for (auto& child : this->children())
                    static_cast<Object&>(child).addPriorTransformation(transformation);
            }
            else
                _drawableObjects[this]->addPriorTransformation(transformation);

            return *this;
        }

        bool isDrawable() { return (_drawableObjects.find(this) == _drawableObjects.end()) ? false : true; }

    private:
        std::unordered_map<Object*, Containers::Pointer<DrawableObject>>& _drawableObjects;
    };

} // namespace magnum_dynamics

#endif // MAGNUM_DYNAMICS_OBJECT_HPP