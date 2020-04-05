#include "drawable_obj.hpp"

namespace Magnum {
    DrawableObject::DrawableObject(ShaderManager& shader, Object3D& object, SceneGraph::DrawableGroup3D& group)
        : SceneGraph::Drawable3D{object, &group},
          _colorShader{shader.get<Shaders::Phong>("color")},
          _textureShader{shader.get<Shaders::Phong>("texture")}
    {
    }

    DrawableObject& DrawableObject::setMeshes(GL::Mesh& mesh)
    {
        _mesh = std::move(mesh);
        return *this;
    }

    DrawableObject& DrawableObject::setTextures(GL::Texture2D& texture)
    {
        _texture = std::move(texture);
        return *this;
    }

    DrawableObject& DrawableObject::setColor(const Color4& color)
    {
        _color = std::move(color);
        return *this;
    }

    void DrawableObject::draw(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera)
    {
        (*_colorShader)
            .setDiffuseColor(_color)
            .setLightPosition(0, camera.cameraMatrix().transformPoint({7.0f, 5.0f, 2.5f}))
            .setLightColor(0, Color3{1.0f})
            .setTransformationMatrix(transformationMatrix)
            .setNormalMatrix(transformationMatrix.normalMatrix())
            .setProjectionMatrix(camera.projectionMatrix());

        _mesh.draw(*_colorShader);
    }
} // namespace Magnum