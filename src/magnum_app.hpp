// Corrade
#include <Corrade/Containers/Reference.h>
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>

// GL
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>

// MeshTools
#include <Magnum/MeshTools/CompressIndices.h>
#include <Magnum/MeshTools/Interleave.h>
#include <Magnum/MeshTools/Compile.h>

// Platform
#include <Magnum/Platform/Sdl2Application.h>

// Primitives
#include <Magnum/Primitives/Cube.h>

// SceneGraph
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Object.hpp>
#include <Magnum/SceneGraph/SceneGraph.h>
#include <Magnum/SceneGraph/Scene.h>

// Shader
#include <Magnum/Shaders/Phong.h>

// Trade
#include <Magnum/Trade/MeshData3D.h>
#include <Magnum/Trade/PhongMaterialData.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/Trade/MeshObjectData3D.h>
#include <Magnum/Trade/SceneData.h>
#include <Magnum/Trade/TextureData.h>

// Others
#include <Magnum/ResourceManager.h>
#include <Magnum/ImageView.h>
#include <Magnum/Mesh.h>
#include <Magnum/PixelFormat.h>
#include <iostream>

namespace Magnum {
    using namespace Math::Literals;

    typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D> Scene3D;
    typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;
    typedef ResourceManager<Shaders::Phong> ShaderManager;

    class MagnumApp : public Platform::Application {
    public:
        explicit MagnumApp(const Arguments& arguments);
        void addPrimitive(const Trade::MeshData3D& primitive, const Matrix4& transformation, Color4 color);

    private:
        void drawEvent() override;
        void Update();
        void importScene(const std::string& file, const std::string& importer_type);
        void addObject(
            Trade::AbstractImporter& importer, 
            Object3D& parent, 
            Containers::ArrayView<Containers::Optional<GL::Mesh>> meshes,
            Containers::ArrayView<Containers::Optional<Trade::PhongMaterialData>> materials,
            Containers::ArrayView<Containers::Optional<GL::Texture2D>> textures,
            UnsignedInt i);

        void viewportEvent(ViewportEvent& event) override;
        void mousePressEvent(MouseEvent& event) override;
        void mouseReleaseEvent(MouseEvent& event) override;
        void mouseMoveEvent(MouseMoveEvent& event) override;
        void mouseScrollEvent(MouseScrollEvent& event) override;

        Vector3 positionOnSphere(const Vector2i& position) const;

        // Shaders
        ShaderManager _shaderManager;

        // Scene elements
        Scene3D _scene;
        Object3D _manipulator, _cameraObject;
        SceneGraph::Camera3D* _camera;
        SceneGraph::DrawableGroup3D _drawables;

        std::vector<Object3D*> _objects;
        std::vector<Matrix4> _transformations;

        Vector3 _previousPosition;
    };
} // namespace Magnum