#ifndef MAGNUM_DYNAMICS_DRAWABLE_OBJECT_HPP
#define MAGNUM_DYNAMICS_DRAWABLE_OBJECT_HPP

#include <Corrade/Containers/Optional.h>

#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Texture.h>

#include <Magnum/Math/Color.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/ResourceManager.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Shaders/VertexColorGL.h>
#include <Magnum/Trade/PhongMaterialData.h>

namespace magnum_dynamics {
    using namespace Magnum;
    using namespace Math::Literals;

    typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;

    class DrawableObject : public SceneGraph::Drawable3D {
    public:
        explicit DrawableObject(Object3D& object, SceneGraph::DrawableGroup3D& group, ResourceManager<GL::AbstractShaderProgram>& shadersManager)
            : SceneGraph::Drawable3D{object, &group},
              _shadersManager(shadersManager),
              _priorTransformation(Matrix4()) {}

        DrawableObject& setMesh(GL::Mesh& mesh);

        DrawableObject& setTexture(GL::Texture2D& texture);

        DrawableObject& setMaterial(Trade::PhongMaterialData& material);

        DrawableObject& setColor(const Color4& color);

        DrawableObject& addPriorTransformation(const Matrix4& transformation);

    private:
        void draw(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera) override;

        // Shaders
        ResourceManager<GL::AbstractShaderProgram>& _shadersManager;

        // Mesh
        GL::Mesh _mesh;

        // Texture
        Containers::Optional<GL::Texture2D> _texture;

        // Material
        Containers::Optional<Trade::PhongMaterialData> _material;

        // Color
        Containers::Optional<Color4> _color;

        // Prior and posterior transformation
        Matrix4 _priorTransformation;
    };
} // namespace magnum_dynamics

#endif // MAGNUM_DYNAMICS_DRAWABLE_OBJECT_HPP