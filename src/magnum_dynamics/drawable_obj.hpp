#ifndef MAGNUMDYNAMICS_DRAWABLEOBJ_HPP
#define MAGNUMDYNAMICS_DRAWABLEOBJ_HPP

#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/Directory.h>
#include <Magnum/DartIntegration/ConvertShapeNode.h>
#include <Magnum/DartIntegration/World.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/ResourceManager.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Object.hpp>
#include <Magnum/SceneGraph/SceneGraph.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Trade/PhongMaterialData.h>

namespace magnum_dynamics {
    using namespace Magnum;
    using namespace Math::Literals;

    typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;
    typedef ResourceManager<Shaders::Phong> ShaderManager;

    class DrawableObject : public SceneGraph::Drawable3D {
    public:
        explicit DrawableObject(ShaderManager& shader, Object3D& object, SceneGraph::DrawableGroup3D& group);

        DrawableObject& setMeshes(GL::Mesh& meshes);

        DrawableObject& setTextures(GL::Texture2D& texture);

        DrawableObject& setColor(const Color4& color);

    private:
        void draw(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera) override;

        Resource<Shaders::Phong> _colorShader, _textureShader;

        GL::Mesh _mesh;
        GL::Texture2D _texture;
        Color4 _color;
    };
} // namespace magnum_dynamics

#endif // MAGNUMDYNAMICS_DRAWABLEOBJ_HPP