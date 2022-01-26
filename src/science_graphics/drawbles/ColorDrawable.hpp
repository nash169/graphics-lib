#ifndef SCIENCEGRAPHICS_COLOR_DRAWABLE_HPP
#define SCIENCEGRAPHICS_COLOR_DRAWABLE_HPP

#include "science_graphics/drawbles/AbstractDrawable.hpp"
#include <Magnum/Shaders/VertexColorGL.h>

namespace science_graphics {
    namespace drawables {
        template <size_t N = 3>
        class ColorDrawable : public AbstractDrawable<N> {
        public:
            explicit ColorDrawable(SceneGraph::Object<typename std::conditional<N == 3, SceneGraph::MatrixTransformation3D, SceneGraph::MatrixTransformation2D>::type>& object, SceneGraph::DrawableGroup<N, Float>& group, Shaders::VertexColorGL<N>& shader)
                : AbstractDrawable<N>(object, group),
                  _shader(shader) {}

        private:
            void draw(const typename std::conditional<N == 3, Matrix4, Matrix3>::type& transformationMatrix, SceneGraph::Camera<N, Float>& camera) override
            {
                _shader
                    .setTransformationProjectionMatrix(camera.projectionMatrix() * transformationMatrix * AbstractDrawable<N>::_priorTransformation)
                    .draw(AbstractDrawable<N>::_mesh);
            }

            // Shaders
            Shaders::VertexColorGL<N>& _shader;
        };

    } // namespace drawables
} // namespace science_graphics

#endif // SCIENCEGRAPHICS_COLOR_DRAWABLE_HPP