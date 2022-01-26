#ifndef SCIENCEGRAPHICS_ABSTRACT_DRAWABLE_HPP
#define SCIENCEGRAPHICS_ABSTRACT_DRAWABLE_HPP

#include <Magnum/SceneGraph/Drawable.h>

namespace science_graphics {
    namespace drawables {
        template <size_t N>
        class AbstractDrawable : public SceneGraph::Drawable<N, Float> {
        public:
            explicit AbstractDrawable(SceneGraph::Object<typename std::conditional<N == 3, SceneGraph::MatrixTransformation3D, SceneGraph::MatrixTransformation2D>::type>& object, SceneGraph::DrawableGroup<N, Float>& group)
                : SceneGraph::Drawable<N, Float>{object, &group} {}

            AbstractDrawable<N>& setMesh(GL::Mesh& mesh)
            {
                _mesh = std::move(mesh);
                return *this;
            }

            AbstractDrawable<N>& addPriorTransformation(const typename std::conditional<N == 3, Matrix4, Matrix3>::type transformation)
            {
                _priorTransformation = transformation * _priorTransformation;
                return *this;
            }

        protected:
            // Mesh
            GL::Mesh _mesh;

            // Prior and posterior transformation
            typename std::conditional<N == 3, Matrix4, Matrix3>::type _priorTransformation;
        };
    } // namespace drawables
} // namespace science_graphics

#endif // SCIENCEGRAPHICS_ABSTRACT_DRAWABLE_HPP