#ifndef MAGNUMDYNAMICS_TEXTUREDDRAWABLE_HPP
#define MAGNUMDYNAMICS_TEXTUREDDRAWABLE_HPP

#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/Shaders/Phong.h>

namespace magnum_dynamics {
    using namespace Magnum;

    typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;

    class TexturedDrawable : public SceneGraph::Drawable3D {
    public:
        explicit TexturedDrawable(Object3D& object, SceneGraph::DrawableGroup3D& group, Shaders::Phong& shader, GL::Mesh& mesh, GL::Texture2D& texture) : SceneGraph::Drawable3D{object, &group}, _shader(shader), _mesh(mesh), _texture(texture) {}

    private:
        void draw(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera) override
        {
            _shader
                .setLightPosition(camera.cameraMatrix().transformPoint({-3.0f, 10.0f, 10.0f}))
                .setTransformationMatrix(transformationMatrix)
                .setNormalMatrix(transformationMatrix.normalMatrix())
                .setProjectionMatrix(camera.projectionMatrix())
                .bindDiffuseTexture(_texture)
                .draw(_mesh);
        }

        Shaders::Phong& _shader;
        GL::Mesh& _mesh;
        GL::Texture2D& _texture;
    };
} // namespace magnum_dynamics

#endif // MAGNUMDYNAMICS_TEXTUREDDRAWABLE_HPP