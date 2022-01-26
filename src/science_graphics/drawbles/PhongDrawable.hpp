#ifndef SCIENCEGRAPHICS_PHONG_DRAWABLE_HPP
#define SCIENCEGRAPHICS_PHONG_DRAWABLE_HPP

#include "science_graphics/drawbles/AbstractDrawable.hpp"
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Trade/PhongMaterialData.h>

namespace science_graphics {
    namespace drawables {
        template <size_t N = 3>
        class PhongDrawable : public AbstractDrawable<N> {
        public:
            explicit PhongDrawable(SceneGraph::Object<std::conditional_t<N == 3, SceneGraph::MatrixTransformation3D, SceneGraph::MatrixTransformation2D>>& object, SceneGraph::DrawableGroup<N, Float>& group, Shaders::PhongGL& shader)
                : AbstractDrawable<N>(object, group),
                  _shader(shader) {}

            PhongDrawable& setMaterial(Trade::PhongMaterialData& material)
            {
                _material = std::move(material);
                return *this;
            }

            PhongDrawable& setColor(const Color4& color)
            {
                _color = std::move(color);
                return *this;
            }

        protected:
            // Material
            Containers::Optional<Trade::PhongMaterialData> _material;

            // Color
            Containers::Optional<Color4> _color;

        private:
            void draw(const typename std::conditional<N == 3, Matrix4, Matrix3>::type& transformationMatrix, SceneGraph::Camera<N, Float>& camera) override
            {
                auto transformation = transformationMatrix * AbstractDrawable<N>::_priorTransformation;

                if (_material)
                    _shader
                        .setAmbientColor(_material->ambientColor())
                        .setDiffuseColor(_material->diffuseColor())
                        .setSpecularColor(_material->specularColor())
                        .setShininess(_material->shininess())
                        // .setLightPositions({{camera.cameraMatrix().transformPoint({0.0f, 2.0f, 3.0f}), 0.0f},
                        //     {camera.cameraMatrix().transformPoint({0.0f, -2.0f, 3.0f}), 0.0f}})
                        .setTransformationMatrix(transformation)
                        .setNormalMatrix(transformation.normalMatrix())
                        .setProjectionMatrix(camera.projectionMatrix())
                        .draw(AbstractDrawable<N>::_mesh);
                // if color is present (but not texture and material) use color shader (Phong) with fewer color options
                else if (_color)
                    _shader
                        .setDiffuseColor(*_color)
                        // .setLightPositions({{camera.cameraMatrix().transformPoint({0.0f, 2.0f, 3.0f}), 0.0f},
                        //     {camera.cameraMatrix().transformPoint({0.0f, -2.0f, 3.0f}), 0.0f}})
                        .setTransformationMatrix(transformation)
                        .setNormalMatrix(transformation.normalMatrix())
                        .setProjectionMatrix(camera.projectionMatrix())
                        .draw(AbstractDrawable<N>::_mesh);
            }

            // Shaders
            Shaders::PhongGL& _shader;
        };

    } // namespace drawables
} // namespace science_graphics

#endif // SCIENCEGRAPHICS_PHONG_DRAWABLE_HPP