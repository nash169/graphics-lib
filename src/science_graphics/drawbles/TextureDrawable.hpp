#ifndef SCIENCEGRAPHICS_TEXTURE_DRAWABLE_HPP
#define SCIENCEGRAPHICS_TEXTURE_DRAWABLE_HPP

#include "science_graphics/drawbles/AbstractDrawable.hpp"
#include <Magnum/GL/Texture.h>
#include <Magnum/Shaders/Phong.h>

namespace science_graphics {
    namespace drawables {
        template <size_t N = 3>
        class TextureDrawable : public AbstractDrawable<N> {
        public:
            explicit TextureDrawable(SceneGraph::Object<std::conditional_t<N == 3, SceneGraph::MatrixTransformation3D, SceneGraph::MatrixTransformation2D>>& object, SceneGraph::DrawableGroup<N, Float>& group, Shaders::PhongGL& shader)
                : AbstractDrawable<N>(object, group),
                  _shader(shader) {}

            TextureDrawable& setTexture(GL::Texture2D& texture)
            {
                _texture = std::move(texture);
                return *this;
            }

        protected:
            // Texture
            GL::Texture2D _texture;

        private:
            void draw(const std::conditional_t<N == 3, Matrix4, Matrix3>& transformationMatrix, SceneGraph::Camera<N, Float>& camera) override
            {
                auto transformation = transformationMatrix * AbstractDrawable<N>::_priorTransformation;

                // Use texture shader (Phong::Flag::DiffuseTexture)
                _shader
                    // .setLightPositions({{camera.cameraMatrix().transformPoint({0.0f, 2.0f, 3.0f}), 0.0f},
                    //     {camera.cameraMatrix().transformPoint({0.0f, -2.0f, 3.0f}), 0.0f}})
                    .setTransformationMatrix(transformation)
                    .setNormalMatrix(transformation.normalMatrix())
                    .setProjectionMatrix(camera.projectionMatrix())
                    .bindDiffuseTexture(_texture)
                    .draw(AbstractDrawable<N>::_mesh);
            }

            // Shaders
            Shaders::PhongGL& _shader;
        };

    } // namespace drawables
} // namespace science_graphics

#endif // SCIENCEGRAPHICS_TEXTURE_DRAWABLE_HPP