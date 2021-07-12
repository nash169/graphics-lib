#include "magnum_dynamics/DrawableObject.hpp"

#include <iostream>

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

    DrawableObject& DrawableObject::setMaterial(Trade::PhongMaterialData& material)
    {
        _material = std::move(material);
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
        Matrix4 transformation = transformationMatrix * _primitiveTransformation;

        // If texture is present use texture shader (Phong::Flag::DiffuseTexture)
        if (_texture)
            (*_shadersManager.get<GL::AbstractShaderProgram, Shaders::PhongGL>("texture"))
                // .setLightPositions({{camera.cameraMatrix().transformPoint({0.0f, 2.0f, 3.0f}), 0.0f},
                //     {camera.cameraMatrix().transformPoint({0.0f, -2.0f, 3.0f}), 0.0f}})
                .setTransformationMatrix(transformation)
                .setNormalMatrix(transformation.normalMatrix())
                .setProjectionMatrix(camera.projectionMatrix())
                .bindDiffuseTexture(*_texture)
                .draw(_mesh);
        // if material is present (but not texture) use color shader (Phong)
        else if (_material)
            (*_shadersManager.get<GL::AbstractShaderProgram, Shaders::PhongGL>("color"))
                .setAmbientColor(_material->ambientColor())
                .setDiffuseColor(_material->diffuseColor())
                .setSpecularColor(_material->specularColor())
                .setShininess(_material->shininess())
                // .setLightPositions({{camera.cameraMatrix().transformPoint({0.0f, 2.0f, 3.0f}), 0.0f},
                //     {camera.cameraMatrix().transformPoint({0.0f, -2.0f, 3.0f}), 0.0f}})
                .setTransformationMatrix(transformation)
                .setNormalMatrix(transformation.normalMatrix())
                .setProjectionMatrix(camera.projectionMatrix())
                .draw(_mesh);
        // if color is present (but not texture and material) use color shader (Phong) with fewer color options
        else if (_color)
            (*_shadersManager.get<GL::AbstractShaderProgram, Shaders::PhongGL>("color"))
                .setDiffuseColor(*_color)
                // .setLightPositions({{camera.cameraMatrix().transformPoint({0.0f, 2.0f, 3.0f}), 0.0f},
                //     {camera.cameraMatrix().transformPoint({0.0f, -2.0f, 3.0f}), 0.0f}})
                .setTransformationMatrix(transformation)
                .setNormalMatrix(transformation.normalMatrix())
                .setProjectionMatrix(camera.projectionMatrix())
                .draw(_mesh);
        // else use vertex shader (Phong::Flag::VertexColor)
        else
            (*_shadersManager.get<GL::AbstractShaderProgram, Shaders::VertexColorGL3D>("vertex"))
                .setTransformationProjectionMatrix(camera.projectionMatrix() * transformation)
                .draw(_mesh);
    }
} // namespace magnum_dynamics