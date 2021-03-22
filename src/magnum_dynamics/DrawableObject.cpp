#include "magnum_dynamics/DrawableObject.hpp"

namespace magnum_dynamics {
    DrawableObject& DrawableObject::setMesh(GL::Mesh& mesh)
    {
        _mesh = std::move(mesh);
        return *this;
    }

    DrawableObject& DrawableObject::setTexture(GL::Texture2D& texture)
    {
        _texture = std::move(texture);
        return *this;
    }

    DrawableObject& DrawableObject::setColor(const Color4& color)
    {
        _color = std::move(color);
        return *this;
    }

    DrawableObject& DrawableObject::setPrimitiveTransformation(const Matrix4& transformation)
    {
        _primitiveTransformation = std::move(transformation);

        return *this;
    }

    void DrawableObject::draw(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera)
    {
        if (!_texture) {
            (*_colorShader)
                .setDiffuseColor(_color)
                // .setLightPositions({{camera.cameraMatrix().transformPoint({0.0f, 2.0f, 3.0f}), 0.0f},
                //     {camera.cameraMatrix().transformPoint({0.0f, -2.0f, 3.0f}), 0.0f}})
                .setTransformationMatrix(transformationMatrix)
                .setNormalMatrix(transformationMatrix.normalMatrix())
                .setProjectionMatrix(camera.projectionMatrix())
                .draw(_mesh);
        }
        else {
            (*_textureShader)
                // .setLightPositions({{camera.cameraMatrix().transformPoint({0.0f, 2.0f, 3.0f}), 0.0f},
                //     {camera.cameraMatrix().transformPoint({0.0f, -2.0f, 3.0f}), 0.0f}})
                .setTransformationMatrix(transformationMatrix)
                .setNormalMatrix(transformationMatrix.normalMatrix())
                .setProjectionMatrix(camera.projectionMatrix())
                .bindDiffuseTexture(*_texture)
                .draw(_mesh);
        }
    }
} // namespace magnum_dynamics