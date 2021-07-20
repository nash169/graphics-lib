#ifndef MAGNUMDYNAMICS_MAGNUMAPP_HPP
#define MAGNUMDYNAMICS_MAGNUMAPP_HPP

#include <iostream>
#include <map>
#include <memory>

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

#include <Magnum/Primitives/Axis.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/Primitives/Icosphere.h>

#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Object.hpp>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/SceneGraph/SceneGraph.h>

#include <Magnum/Shaders/Phong.h>
#include <Magnum/Shaders/VertexColorGL.h>

#include <Magnum/ResourceManager.h>

#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/Trade/MeshData.h>
#include <Magnum/Trade/MeshObjectData3D.h>
#include <Magnum/Trade/PhongMaterialData.h>
#include <Magnum/Trade/SceneData.h>
#include <Magnum/Trade/TextureData.h>

#include <MagnumPlugins/AssimpImporter/AssimpImporter.h>

#include <Magnum/DebugTools/ColorMap.h>

/* Magnum Integration */
#include <Magnum/EigenIntegration/Integration.h>

#include "magnum_dynamics/Camera.hpp"
#include "magnum_dynamics/DrawableObject.hpp"
#include "magnum_dynamics/Object.hpp"

namespace magnum_dynamics {
    using namespace Magnum;
    using namespace Math::Literals;

    typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D> Scene3D;

    class MagnumApp : public Platform::Application {
    public:
        explicit MagnumApp(const Arguments& arguments);

        ~MagnumApp()
        {
            _drawableObjects.clear();
        }

        // Set importer
        MagnumApp& setImporter(const std::string& importer);

        // Get Object
        Object& manipulator();

        SceneGraph::DrawableGroup3D& drawables();

        // Get number objects
        size_t numObjects() const;

        // Cartesian frame
        Object3D& addFrame();

        // Add primitive
        Object& addPrimitive(const std::string& primitive);

        // Import from file (return object parent of all the objects inside the file)
        Object& import(const std::string& file);

        // To plot a surface basically but it can be more general
        Object& plot(const Eigen::MatrixXd& vertices, const Eigen::VectorXd& fun, const Eigen::MatrixXd& indices, const std::string& colormap = "turbo");
        Object& plot(const std::string& file, const Eigen::VectorXd& fun, const std::string& colormap = "turbo");

        // Import multiple files from directory (check how to handle this)
        // Containers::Array<Object&> import(const std::string& directory, const std::string& extension)
        // {
        //     if (Corrade::Utility::Directory::isDirectory(path))

        //     Containers::Array<Object> objects;

        //     for (auto& file : Corrade::Utility::Directory::list(directory)) {
        //         auto pair = Corrade::Utility::Directory::splitExtension(file);
        //         if (!extension.compare(pair.second)) {
        //             arrayAppend(objects, import(Corrade::Utility::Directory::join(directory, file)));
        //         }
        //     }

        //     return objects;
        // }

        Containers::Pointer<Camera>& camera()
        {
            return _camera;
        }

    protected:
        // Recursively add objects from meshes
        void addObject(Containers::ArrayView<Containers::Optional<GL::Mesh>> meshes,
            Containers::ArrayView<Containers::Optional<GL::Texture2D>> textures,
            Containers::ArrayView<Containers::Optional<Trade::PhongMaterialData>> materials,
            Object3D& parent, UnsignedInt i);

        // Draw
        void drawEvent() override;

        // Handle multiple shaders
        ResourceManager<GL::AbstractShaderProgram> _shadersManager;

        // Scene
        Scene3D _scene;

        // Camera
        Containers::Pointer<Camera> _camera;

        // Parent object
        Object* _manipulator;

        // Objects
        std::unordered_map<Object3D*, Containers::Pointer<DrawableObject>> _drawableObjects;
        std::unordered_map<Object*, Containers::Pointer<DrawableObject>> _drawableObjs;

        // Drawables
        SceneGraph::DrawableGroup3D _drawables;

        // Manager (to set importer)
        PluginManager::Manager<Trade::AbstractImporter> _manager;

        // Importer
        Containers::Pointer<Trade::AbstractImporter> _importer;

        // Mouse interaction
        Vector3 _previousPosition;

        void viewportEvent(ViewportEvent& event) override;
        void mousePressEvent(MouseEvent& event) override;
        void mouseReleaseEvent(MouseEvent& event) override;
        void mouseMoveEvent(MouseMoveEvent& event) override;
        void mouseScrollEvent(MouseScrollEvent& event) override;
        Vector3 positionOnSphere(const Vector2i& position) const;
    };
} // namespace magnum_dynamics

#endif // MAGNUMDYNAMICS_MAGNUMAPP_HPP