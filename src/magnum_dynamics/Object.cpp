#include "magnum_dynamics/Object.hpp"

namespace magnum_dynamics {

    Object::Object(SceneGraph::AbstractBasicObject3D<Float>& object, SceneGraph::AbstractBasicTranslationRotation3D<Float>& transformation)
        : SceneGraph::AbstractBasicFeature3D<Float>{object}, _transformation(transformation), _updated(false), _updatedMesh(false) {}

} // namespace magnum_dynamics