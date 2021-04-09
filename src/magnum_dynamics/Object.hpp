#ifndef MAGNUM_DYNAMICS_OBJECT_HPP
#define MAGNUM_DYNAMICS_OBJECT_HPP

#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <unordered_map>

#include "magnum_dynamics/DrawableObject.hpp"
#include "magnum_dynamics/types.hpp"

using namespace Magnum;

namespace magnum_dynamics {

    class Object : public Object3D {
    public:
        Object(Object3D* object,
            std::unordered_map<Object*, Containers::Pointer<DrawableObject>>& drawableObj)
            : Object3D{object}, _primitive(Matrix4()), _drawableObjects(drawableObj) {}

        Object& setTransformation(const Matrix4& transformation)
        {
            Object3D::setTransformation(transformation * _primitive);

            return *this;
        }

        // Not working
        // Object& setPrimitiveTransformation(const Matrix4& primitive)
        // {
        //     _primitive = primitive;

        //     return *this;
        // }

        Object& setPrimitiveTransformation(const Matrix4& primitive)
        {
            _drawableObjects[this]->setPrimitiveTransformation(primitive);

            return *this;
        }

        Object& setColor(const Color4& color)
        {
            _drawableObjects[this]->setColor(color);

            return *this;
        }

    private:
        Matrix4 _primitive;

        std::unordered_map<Object*, Containers::Pointer<DrawableObject>>& _drawableObjects;
    };

} // namespace magnum_dynamics

#endif // MAGNUM_DYNAMICS_OBJECT_HPP