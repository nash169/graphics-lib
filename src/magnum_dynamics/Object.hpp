#ifndef MAGNUM_DYNAMICS_OBJECT_HPP
#define MAGNUM_DYNAMICS_OBJECT_HPP

#include <Magnum/Math/Math.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/SceneGraph/AbstractFeature.h>
#include <Magnum/SceneGraph/AbstractTranslationRotation3D.h>

#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Directory.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/ImageView.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/Trade/PhongMaterialData.h>
#include <Magnum/Trade/TextureData.h>

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pointer.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>

#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>

#include <Magnum/ImageView.h>

#include <Magnum/MeshTools/Compile.h>
#include <Magnum/MeshTools/CompressIndices.h>
#include <Magnum/MeshTools/Interleave.h>

#include <Magnum/PixelFormat.h>

#include <Magnum/Platform/Sdl2Application.h>

#include <Magnum/Primitives/Cube.h>

#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Object.hpp>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/SceneGraph/SceneGraph.h>

#include <Magnum/Shaders/Phong.h>

#include <Magnum/ResourceManager.h>

// #include <Magnum/Trade/AbstractImporter.h>
// #include <Magnum/Trade/ImageData.h>
// #include <Magnum/Trade/MeshData.h>
#include <Magnum/Trade/MeshObjectData3D.h>
// #include <Magnum/Trade/PhongMaterialData.h>
// #include <Magnum/Trade/SceneData.h>
// #include <Magnum/Trade/TextureData.h>

namespace magnum_dynamics {
    using namespace Magnum;

    class Object : public SceneGraph::AbstractBasicFeature3D<Float> {
    public:
        /**
         * @brief Constructor
         * @param object    SceneGraph::Object this @ref DartIntegration::Object belongs to
         * @param node      DART `ShapeNode` to connect with
         */
        template <class T>
        Object(T& object) : Object{object, object} {}

        /**
         * @brief Get transformation
         * @return Reference to self (for method chaining)
         */
        Object& update() { return *this; }

        /**
         * @brief Whether the object was updated
         *
         * @see @ref clearUpdateFlag()
         */
        bool isUpdated() const { return _updated; }

        /**
         * @brief Clear update flag
         * @return Reference to self (for method chaining)
         */
        Object& clearUpdateFlag()
        {
            _updated = false;
            return *this;
        }

        /** @brief Whether object mesh was updated */
        bool hasUpdatedMesh() const { return _updatedMesh; }

        Object& setTransformation(const Matrix4& trans)
        {
            using namespace Math::Literals;

            /* Check if any value is NaN */
            if (Math::isNan(trans.toVector()).any()) {
                Warning{} << "DartIntegration::Object::update(): Received NaN values from DART. Ignoring this update.";
                return *this;
            }

            Quaternion quat = Quaternion::fromMatrix(trans.rotationScaling());
            Vector3 axis = quat.axis();
            Rad angle = quat.angle();
            if (Math::abs(angle) <= 1e-5_radf) {
                axis = {1.f, 0.f, 0.f};
            }
            axis = axis.normalized();
            Vector3 t = trans.translation();

            /* Pass it to Magnum */
            _transformation.resetTransformation()
                .rotate(angle, axis)
                .translate(t);

            return *this;
        }

    private:
        explicit Object(SceneGraph::AbstractBasicObject3D<Float>& object, SceneGraph::AbstractBasicTranslationRotation3D<Float>& transformation);

        SceneGraph::AbstractBasicTranslationRotation3D<Float>& _transformation;

        bool _updated, _updatedMesh;
    };
} // namespace magnum_dynamics

#endif // MAGNUM_DYNAMICS_OBJECT_HPP