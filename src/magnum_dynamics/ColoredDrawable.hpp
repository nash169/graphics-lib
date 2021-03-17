#ifndef MAGNUMDYNAMICS_COLOREDDRAWABLE_HPP
#define MAGNUMDYNAMICS_COLOREDDRAWABLE_HPP

#include <Magnum/Math/Color.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Trade/PhongMaterialData.h>

namespace magnum_dynamics {
    using namespace Magnum;
    using namespace Math::Literals;

    typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;

    class ColoredDrawable : public SceneGraph::Drawable3D {
    public:
        explicit ColoredDrawable(Object3D& object, SceneGraph::DrawableGroup3D& group, Shaders::Phong& shader, GL::Mesh& mesh, const Color4& color = 0xffffff_rgbf)
            : SceneGraph::Drawable3D{object, &group}, _shader(shader)
        {
            _mesh = std::move(mesh);
            _color = std::move(color);
        }

    private:
        void draw(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera) override
        {
            _shader
                .setDiffuseColor(_color)
                .setLightPositions({{camera.cameraMatrix().transformPoint({-3.0f, 10.0f, 10.0f}), 0.0f}})
                .setTransformationMatrix(transformationMatrix)
                .setNormalMatrix(transformationMatrix.normalMatrix())
                .setProjectionMatrix(camera.projectionMatrix())
                .draw(_mesh);
        }

        Shaders::Phong& _shader;
        GL::Mesh _mesh;
        Color4 _color;
    };
} // namespace magnum_dynamics

#endif // MAGNUMDYNAMICS_COLOREDDRAWABLE_HPP