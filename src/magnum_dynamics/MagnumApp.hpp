#ifndef MAGNUMDYNAMICS_MAGNUMAPP_HPP
#define MAGNUMDYNAMICS_MAGNUMAPP_HPP

// Corrade
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Reference.h>
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
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/MeshTools/CompressIndices.h>
#include <Magnum/MeshTools/Interleave.h>

// Platform
#include <Magnum/Platform/Sdl2Application.h>

// Primitives
#include <Magnum/Primitives/Cube.h>

// SceneGraph
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Object.hpp>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/SceneGraph/SceneGraph.h>

// Shader
#include <Magnum/Shaders/Phong.h>

// Trade
#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/Trade/MeshData3D.h>
#include <Magnum/Trade/MeshObjectData3D.h>
#include <Magnum/Trade/PhongMaterialData.h>
#include <Magnum/Trade/SceneData.h>
#include <Magnum/Trade/TextureData.h>

// Others
#include <Magnum/ImageView.h>
#include <Magnum/Mesh.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/ResourceManager.h>

#include "magnum_dynamics/ColoredDrawable.hpp"
#include "magnum_dynamics/TexturedDrawable.hpp"
#include <iostream>

namespace magnum_dynamics {
    using namespace Magnum;
    using namespace Math::Literals;

    typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D> Scene3D;
    typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;

    class MagnumApp : public Platform::Application {
    public:
        explicit MagnumApp(const Arguments& arguments) : Platform::Application{arguments, Configuration{}.setTitle("Magnum Viewer Example").setWindowFlags(Configuration::WindowFlag::Resizable)}
        {
            // Create the camera object for the scene
            _cameraObject
                .setParent(&_scene)
                .translate(Vector3::zAxis(10.0f));
            (*(_camera = new SceneGraph::Camera3D{_cameraObject}))
                .setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
                .setProjectionMatrix(Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.01f, 1000.0f))
                .setViewport(GL::defaultFramebuffer.viewport().size());

            // Basic object parent of all the others
            _manipulator.setParent(&_scene);

            // Recall something from OpenGL study but don't precisely
            GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
            GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);

            // Set colored shader
            _coloredShader
                .setAmbientColor(0x111111_rgbf)
                .setSpecularColor(0xffffff_rgbf)
                .setShininess(80.0f);

            // Set texture shader
            _texturedShader
                .setAmbientColor(0x111111_rgbf)
                .setSpecularColor(0x111111_rgbf)
                .setShininess(80.0f);

            // Read main arguments if present and load the scene
            if (arguments.argc > 1) {
                Utility::Arguments args;
                args.addArgument("file")
                    .setHelp("file", "file to load")
                    .addOption("importer", "AnySceneImporter")
                    .setHelp("importer", "importer plugin to use")
                    .addSkippedPrefix("magnum", "engine-specific options")
                    .setGlobalHelp("Displays a 3D scene file provided on command line.")
                    .parse(arguments.argc, arguments.argv);

                importScene(args.value("file"), args.value("importer"));
            }
        }

        // Import primitives
        void addPrimitive(const Trade::MeshData3D& primitive, const Matrix4& transformation, Color4 color)
        {
            GL::Buffer vertices;
            vertices.setData(MeshTools::interleave(primitive.positions(0), primitive.normals(0)));

            Containers::Array<char> indexData;
            MeshIndexType indexType;
            UnsignedInt indexStart, indexEnd;
            std::tie(indexData, indexType, indexStart, indexEnd) = MeshTools::compressIndices(primitive.indices());
            GL::Buffer indices;
            indices.setData(indexData);

            GL::Mesh mesh;

            mesh
                .setPrimitive(primitive.primitive())
                .setCount(primitive.indices().size())
                .addVertexBuffer(std::move(vertices), 0, Shaders::Phong::Position{}, Shaders::Phong::Normal{})
                .setIndexBuffer(std::move(indices), 0, indexType, indexStart, indexEnd);

            auto* object = new Object3D{&_manipulator};
            object->setTransformation(transformation);

            new ColoredDrawable{*object, _drawables, _coloredShader, mesh, color};

            _objects.push_back(object);
            _transformations.push_back(transformation);
        }

        // Import scene
        void importScene(const std::string& file, const std::string& importer_type = "AnySceneImporter")
        {
            // Create manager
            PluginManager::Manager<Trade::AbstractImporter> manager;

            // Create importer
            Containers::Pointer<Trade::AbstractImporter> importer = manager.loadAndInstantiate(importer_type);
            if (!importer)
                std::exit(1);

            // Import file
            Debug{} << "Opening file" << file;
            if (!importer->openFile(file))
                std::exit(4);

            /* TEXTURES */
            Containers::Array<Containers::Optional<GL::Texture2D>> textures{importer->textureCount()};

            for (UnsignedInt i = 0; i != importer->textureCount(); ++i) {
                // Import texture
                Debug{} << "Importing texture" << i << importer->textureName(i);
                Containers::Optional<Trade::TextureData> textureData = importer->texture(i);
                if (!textureData || textureData->type() != Trade::TextureData::Type::Texture2D) {
                    Warning{} << "Cannot load texture properties, skipping";
                    continue;
                }

                // Import image
                Debug{} << "Importing image" << textureData->image() << importer->image2DName(textureData->image());
                Containers::Optional<Trade::ImageData2D> imageData = importer->image2D(textureData->image());
                GL::TextureFormat format;
                if (imageData && imageData->format() == PixelFormat::RGB8Unorm)
                    format = GL::TextureFormat::RGB8;
                else if (imageData && imageData->format() == PixelFormat::RGBA8Unorm)
                    format = GL::TextureFormat::RGBA8;
                else {
                    Warning{} << "Cannot load texture image, skipping";
                    continue;
                }

                /* Configure the texture */
                GL::Texture2D texture;
                texture
                    .setMagnificationFilter(textureData->magnificationFilter())
                    .setMinificationFilter(textureData->minificationFilter(), textureData->mipmapFilter())
                    .setWrapping(textureData->wrapping().xy())
                    .setStorage(Math::log2(imageData->size().max()) + 1, format, imageData->size())
                    .setSubImage(0, {}, *imageData)
                    .generateMipmap();

                textures[i] = std::move(texture);
            }

            /* MATERIALS */
            Containers::Array<Containers::Optional<Trade::PhongMaterialData>> materials{importer->materialCount()};

            for (UnsignedInt i = 0; i != importer->materialCount(); ++i) {
                Debug{} << "Importing material" << i << importer->materialName(i);

                Containers::Pointer<Trade::AbstractMaterialData> materialData = importer->material(i);
                if (!materialData || materialData->type() != Trade::MaterialType::Phong) {
                    Warning{} << "Cannot load material, skipping";
                    continue;
                }

                materials[i] = std::move(static_cast<Trade::PhongMaterialData&>(*materialData));
            }

            /* MESHES */
            Containers::Array<Containers::Optional<GL::Mesh>> meshes{importer->meshCount()};

            for (UnsignedInt i = 0; i != importer->mesh3DCount(); ++i) {
                Debug{} << "Importing mesh" << i << importer->mesh3DName(i);

                Containers::Optional<Trade::MeshData3D> meshData = importer->mesh3D(i);
                if (!meshData || !meshData->hasNormals() || meshData->primitive() != MeshPrimitive::Triangles) {
                    Warning{} << "Cannot load the mesh, skipping";
                    continue;
                }

                /* Compile the mesh */
                meshes[i] = MeshTools::compile(*meshData);
            }

            /* Load the scene */
            if (importer->defaultScene() != -1) {
                Debug{} << "Adding default scene" << importer->sceneName(importer->defaultScene());

                Containers::Optional<Trade::SceneData> sceneData = importer->scene(importer->defaultScene());
                if (!sceneData) {
                    Error{} << "Cannot load scene, exiting";
                    return;
                }

                /* Recursively add all children */
                for (UnsignedInt objectId : sceneData->children3D())
                    addObject(*importer, meshes, textures, materials, _manipulator, objectId);
            }
            else if (!meshes.empty() && meshes[0]) {
                auto* object = new Object3D{&_manipulator};
                new ColoredDrawable{*object, _drawables, _coloredShader, *meshes[0]};
                _objects.push_back(object);
            }
        }

    protected:
        void drawEvent() override
        {
            GL::defaultFramebuffer.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth);

            _transformations[0] = _transformations[0] * Matrix4::rotation(0.01_radf, Vector3::xAxis());
            _objects[0]->setTransformation(_transformations[0]);

            _camera->draw(_drawables);

            swapBuffers();

            redraw();
        }

        void addObject(Trade::AbstractImporter& importer, Containers::ArrayView<Containers::Optional<GL::Mesh>> meshes, Containers::ArrayView<Containers::Optional<GL::Texture2D>> textures, Containers::ArrayView<const Containers::Optional<Trade::PhongMaterialData>> materials, Object3D& parent, UnsignedInt i)
        {
            // Import the object information
            Debug{} << "Importing object" << i << importer.object3DName(i);
            Containers::Pointer<Trade::ObjectData3D> objectData = importer.object3D(i);
            if (!objectData) {
                Error{} << "Cannot import object, skipping";
                return;
            }

            // Create the object
            auto* object = new Object3D{&parent};
            object->setTransformation(objectData->transformation());

            /* Add a drawable if the object has a mesh and the mesh is loaded */
            if (objectData->instanceType() == Trade::ObjectInstanceType3D::Mesh && objectData->instance() != -1 && meshes[objectData->instance()]) {
                const Int materialId = static_cast<Trade::MeshObjectData3D*>(objectData.get())->material();

                /* Material not available / not loaded, use a default material */
                if (materialId == -1 || !materials[materialId])
                    new ColoredDrawable{*object, _drawables, _coloredShader, *meshes[objectData->instance()]};
                /* Textured material. If the texture failed to load, again just use adefault colored material. */
                else if (materials[materialId]->flags() & Trade::PhongMaterialData::Flag::DiffuseTexture) {
                    Containers::Optional<GL::Texture2D>& texture = textures[materials[materialId]->diffuseTexture()];
                    if (texture)
                        new TexturedDrawable{*object, _drawables, _coloredShader, *meshes[objectData->instance()], *texture};
                    else
                        new ColoredDrawable{*object, _drawables, _coloredShader, *meshes[objectData->instance()]};
                }
                /* Color-only material */
                else
                    new ColoredDrawable{*object, _drawables, _coloredShader, *meshes[objectData->instance()], materials[materialId]->diffuseColor()};
            }

            _objects.push_back(object);

            /* Recursively add children */
            for (std::size_t id : objectData->children())
                addObject(importer, meshes, textures, materials, *object, id);
        }

        void viewportEvent(ViewportEvent& event) override;
        void mousePressEvent(MouseEvent& event) override;
        void mouseReleaseEvent(MouseEvent& event) override;
        void mouseMoveEvent(MouseMoveEvent& event) override;
        void mouseScrollEvent(MouseScrollEvent& event) override;
        Vector3 positionOnSphere(const Vector2i& position) const;

        Shaders::Phong _coloredShader, _texturedShader{Shaders::Phong::Flag::DiffuseTexture};

        Scene3D _scene;
        Object3D _manipulator, _cameraObject;

        std::vector<Object3D*> _objects;
        std::vector<Matrix4> _transformations;

        SceneGraph::Camera3D* _camera;
        SceneGraph::DrawableGroup3D _drawables;

        Vector3 _previousPosition;
    };
} // namespace magnum_dynamics

#endif // MAGNUMDYNAMICS_MAGNUMAPP_HPP