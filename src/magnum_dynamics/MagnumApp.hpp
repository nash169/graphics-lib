#ifndef MAGNUMDYNAMICS_MAGNUMAPP_HPP
#define MAGNUMDYNAMICS_MAGNUMAPP_HPP

#include <iostream>

// #include <Eigen/Core>

// Corrade
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pointer.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>

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

#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/Trade/MeshData.h>
#include <Magnum/Trade/MeshObjectData3D.h>
#include <Magnum/Trade/PhongMaterialData.h>
#include <Magnum/Trade/SceneData.h>
#include <Magnum/Trade/TextureData.h>

/* Magnum Integration */
// #include <Magnum/EigenIntegration/Integration.h>

#include "magnum_dynamics/Camera.hpp"
#include "magnum_dynamics/DrawableObject.hpp"

namespace magnum_dynamics {
    using namespace Magnum;
    using namespace Math::Literals;

    typedef ResourceManager<GL::Buffer, GL::Mesh, Shaders::Phong> ViewerResourceManager;
    typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D> Scene3D;
    // typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;

    class MagnumApp : public Platform::Application {
    public:
        explicit MagnumApp(const Arguments& arguments);

        // Set importer
        MagnumApp& setImporter(const std::string& importer);

        // Get 3D objects
        std::vector<Object3D*> getObjects();

        int getNumObjects()
        {
            return _objects.size();
        }

        // Import scene
        void add(const std::string& path,
            const std::string& extension = "",
            const Matrix4& transformation = Matrix4(),
            const Color4& color = 0xffffff_rgbf,
            const Matrix4& primitive = Matrix4());

    protected:
        // Add from file
        void import(const std::string& file, const Matrix4& transformation, const Matrix4& primitive);

        // Add primitive
        void addPrimitive(const Trade::MeshData& mesh_data, const Matrix4& transformation, const Color4& color, const Matrix4& primitive);

        // Add Object
        void addObject(Containers::ArrayView<Containers::Optional<GL::Mesh>> meshes,
            Containers::ArrayView<Containers::Optional<GL::Texture2D>> textures,
            Containers::ArrayView<const Containers::Optional<Trade::PhongMaterialData>> materials,
            const Matrix4& transformation,
            const Matrix4& primitive,
            Object3D& parent, UnsignedInt i);

        // Draw
        void drawEvent() override;

        void viewportEvent(ViewportEvent& event) override;
        void mousePressEvent(MouseEvent& event) override;
        void mouseReleaseEvent(MouseEvent& event) override;
        void mouseMoveEvent(MouseMoveEvent& event) override;
        void mouseScrollEvent(MouseScrollEvent& event) override;
        Vector3 positionOnSphere(const Vector2i& position) const;

        ViewerResourceManager _resourceManager;

        Scene3D _scene;
        Object3D _manipulator, *_cameraObject, *_cameraRig;

        std::vector<Object3D*> _objects;

        // SceneGraph::Camera3D* _camera;
        SceneGraph::DrawableGroup3D _drawables;

        Vector3 _previousPosition;

        PluginManager::Manager<Trade::AbstractImporter> _manager;
        Containers::Pointer<Trade::AbstractImporter> _importer;

        std::unique_ptr<Camera> _camera;
    };
} // namespace magnum_dynamics

#endif // MAGNUMDYNAMICS_MAGNUMAPP_HPP