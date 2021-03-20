#ifndef MAGNUMDYNAMICS_MAGNUMAPP_HPP
#define MAGNUMDYNAMICS_MAGNUMAPP_HPP

#include <iostream>

#include <Eigen/Core>

// Corrade
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Optional.h>
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

#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/Trade/MeshData.h>
#include <Magnum/Trade/MeshObjectData3D.h>
#include <Magnum/Trade/PhongMaterialData.h>
#include <Magnum/Trade/SceneData.h>
#include <Magnum/Trade/TextureData.h>

/* Magnum Integration */
// #include <Magnum/BulletIntegration/Integration.h>
#include <Magnum/EigenIntegration/Integration.h>

#include "magnum_dynamics/ColoredDrawable.hpp"
#include "magnum_dynamics/TexturedDrawable.hpp"

namespace magnum_dynamics {
    using namespace Magnum;
    using namespace Math::Literals;

    typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D> Scene3D;
    typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;

    class MagnumApp : public Platform::Application {
    public:
        explicit MagnumApp(const Arguments& arguments)
            : Platform::Application{arguments, Configuration{}.setTitle("Magnum Dynamics").setWindowFlags(Configuration::WindowFlag::Resizable)}
        {
            /* Create the camera object for the scene */
            _cameraObject
                .setParent(&_scene)
                .translate(Vector3::zAxis(30.0f));
            (*(_camera = new SceneGraph::Camera3D{_cameraObject}))
                .setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
                .setProjectionMatrix(Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.01f, 1000.0f))
                .setViewport(GL::defaultFramebuffer.viewport().size());

            /* Basic object parent of all the others */
            _manipulator.setParent(&_scene);

            /* Recall something from OpenGL study but don't precisely */
            GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
            GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
            // GL::Renderer::enable(GL::Renderer::Feature::PolygonOffsetFill);
            // GL::Renderer::setPolygonOffset(2.0f, 0.5f);

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

            // Default importer
            setImporter("AnySceneImporter");

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

                setImporter(args.value("importer")).import(args.value("file"));
            }
        }

        MagnumApp& setImporter(const std::string& importer)
        {
            _importer = _manager.loadAndInstantiate(importer);

            return *this;
        }

        // Import scene
        void add(const std::string& path, const std::string& extension = "", const Matrix4& transformation = Matrix4::translation(Vector3::xAxis(0.0f)), const Color4& color = 0xffffff_rgbf)
        {
            if (Corrade::Utility::Directory::isDirectory(path)) {
                for (auto& file : Corrade::Utility::Directory::list(path)) {
                    auto pair = Corrade::Utility::Directory::splitExtension(file);
                    if (!extension.compare(pair.second)) {
                        import(Corrade::Utility::Directory::join(path, file));
                    }
                }
            }
            else if (!path.compare("cube"))
                addPrimitive(Primitives::cubeSolid(), transformation, color);
            else {
                import(path);
            }
        }

        std::vector<Object3D*> getObjects()
        {
            return _objects;
        }

    protected:
        // Add from file
        void import(const std::string& file)
        {
            // Check importer
            if (!_importer)
                std::exit(1);

            // Import file
            Debug{} << "Opening file" << file;
            if (!_importer->openFile(file))
                std::exit(4);

            /* TEXTURES */
            Containers::Array<Containers::Optional<GL::Texture2D>> textures{_importer->textureCount()};

            for (UnsignedInt i = 0; i != _importer->textureCount(); ++i) {
                // Import texture
                Debug{} << "Importing texture" << i << _importer->textureName(i);
                Containers::Optional<Trade::TextureData> textureData = _importer->texture(i);
                if (!textureData || textureData->type() != Trade::TextureData::Type::Texture2D) {
                    Warning{} << "Cannot load texture properties, skipping";
                    continue;
                }

                // Import image
                Debug{} << "Importing image" << textureData->image() << _importer->image2DName(textureData->image());
                Containers::Optional<Trade::ImageData2D> imageData = _importer->image2D(textureData->image());
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
            Containers::Array<Containers::Optional<Trade::PhongMaterialData>> materials{_importer->materialCount()};

            for (UnsignedInt i = 0; i != _importer->materialCount(); ++i) {
                Debug{} << "Importing material" << i << _importer->materialName(i);

                Containers::Optional<Trade::MaterialData> materialData = _importer->material(i);
                if (!materialData || materialData->types() != Trade::MaterialType::Phong) {
                    Warning{} << "Cannot load material, skipping";
                    continue;
                }

                materials[i] = std::move(static_cast<Trade::PhongMaterialData&>(*materialData));
            }

            /* MESHES */
            Containers::Array<Containers::Optional<GL::Mesh>> meshes{_importer->meshCount()};

            for (UnsignedInt i = 0; i != _importer->meshCount(); ++i) {
                Debug{} << "Importing mesh" << i << _importer->meshName(i);

                Containers::Optional<Trade::MeshData> meshData = _importer->mesh(i);
                if (!meshData || !meshData->hasAttribute(Trade::MeshAttribute::Normal) || meshData->primitive() != MeshPrimitive::Triangles) {
                    Warning{} << "Cannot load the mesh, skipping";
                    continue;
                }

                /* Compile the mesh */
                meshes[i] = MeshTools::compile(*meshData);
            }

            /* Load the scene */
            if (_importer->defaultScene() != -1) {
                Debug{} << "Adding default scene" << _importer->sceneName(_importer->defaultScene());

                Containers::Optional<Trade::SceneData> sceneData = _importer->scene(_importer->defaultScene());
                if (!sceneData) {
                    Error{} << "Cannot load scene, exiting";
                    return;
                }

                /* Recursively add all children */
                for (UnsignedInt objectId : sceneData->children3D())
                    addObject(meshes, textures, materials, _manipulator, objectId);
            }
            else if (!meshes.empty() && meshes[0]) {
                auto* object = new Object3D{&_manipulator};
                new ColoredDrawable{*object, _drawables, _coloredShader, *meshes[0]};
                _transformations.push_back(object->transformation());
                _objects.push_back(object);
            }
        }

        // Add primitive
        void addPrimitive(const Trade::MeshData& mesh_data, const Matrix4& transformation, Color4 color)
        {
            GL::Buffer vertices;
            vertices.setData(MeshTools::interleave(mesh_data.positions3DAsArray(),
                mesh_data.normalsAsArray()));

            std::pair<Containers::Array<char>, MeshIndexType> compressed = MeshTools::compressIndices(mesh_data.indicesAsArray());
            GL::Buffer indices;
            indices.setData(compressed.first);

            GL::Mesh mesh;

            mesh
                .setPrimitive(mesh_data.primitive())
                .setCount(mesh_data.indexCount())
                .addVertexBuffer(std::move(vertices), 0, Shaders::Phong::Position{},
                    Shaders::Phong::Normal{})
                .setIndexBuffer(std::move(indices), 0, compressed.second);

            auto* object = new Object3D{&_manipulator};
            object->setTransformation(transformation);

            new ColoredDrawable{*object, _drawables, _coloredShader, mesh, color};

            _objects.push_back(object);
            _transformations.push_back(transformation);
        }

        void drawEvent() override
        {
            GL::defaultFramebuffer.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth);

            // _transformations[0] = _transformations[0] * Matrix4::rotation(0.01_radf, Vector3::xAxis());
            // _transformations[1] = _transformations[1] * Matrix4::rotation(0.01_radf, Vector3::yAxis());
            // _transformations[2] = _transformations[2] * Matrix4::rotation(0.01_radf, Vector3::zAxis());

            // for (size_t i = 0; i < 3; i++)
            //     _objects[i]->setTransformation(_transformations[i]);

            _camera->draw(_drawables);

            swapBuffers();

            redraw();
        }

        void addObject(Containers::ArrayView<Containers::Optional<GL::Mesh>> meshes, Containers::ArrayView<Containers::Optional<GL::Texture2D>> textures, Containers::ArrayView<const Containers::Optional<Trade::PhongMaterialData>> materials, Object3D& parent, UnsignedInt i)
        {
            // Import the object information
            Debug{} << "Importing object" << i << _importer->object3DName(i);
            Containers::Pointer<Trade::ObjectData3D> objectData = _importer->object3D(i);
            if (!objectData) {
                Error{} << "Cannot import object, skipping";
                return;
            }

            // Create the object
            auto* object = new Object3D{&parent};
            object->setTransformation(objectData->transformation());
            _transformations.push_back(object->transformation());

            /* Add a drawable if the object has a mesh and the mesh is loaded */
            if (objectData->instanceType() == Trade::ObjectInstanceType3D::Mesh && objectData->instance() != -1 && meshes[objectData->instance()]) {
                const Int materialId = static_cast<Trade::MeshObjectData3D*>(objectData.get())->material();

                /* Material not available / not loaded, use a default material */
                if (materialId == -1 || !materials[materialId])
                    new ColoredDrawable{*object, _drawables, _coloredShader, *meshes[objectData->instance()]};
                /* Textured material. If the texture failed to load, again just use adefault colored material. */
                else if (materials[materialId]->hasAttribute(Trade::MaterialAttribute::DiffuseTexture)) {
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
                addObject(meshes, textures, materials, *object, id);
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

        PluginManager::Manager<Trade::AbstractImporter> _manager;
        Containers::Pointer<Trade::AbstractImporter> _importer;
    };
} // namespace magnum_dynamics

#endif // MAGNUMDYNAMICS_MAGNUMAPP_HPP