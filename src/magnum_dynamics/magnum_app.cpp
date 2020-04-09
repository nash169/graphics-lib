#include "magnum_dynamics/magnum_app.hpp"
#include "magnum_dynamics/drawable_obj.hpp"

namespace magnum_dynamics {
    MagnumApp::MagnumApp(const Arguments& arguments) : Platform::Application{arguments, Configuration{}.setTitle("Magnum Viewer Example").setWindowFlags(Configuration::WindowFlag::Resizable)}
    {
        if (arguments.argc > 1) {
            Utility::Arguments args;
            args.addArgument("file").setHelp("file", "file to load").addOption("importer", "AnySceneImporter").setHelp("importer", "importer plugin to use").addSkippedPrefix("magnum", "engine-specific options").setGlobalHelp("Displays a 3D scene file provided on command line.").parse(arguments.argc, arguments.argv);

            importScene(args.value("file"), args.value("importer"));
        }

        _cameraObject
            .setParent(&_scene)
            .translate(Vector3::zAxis(10.0f));
        (*(_camera = new SceneGraph::Camera3D{_cameraObject}))
            .setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
            .setProjectionMatrix(Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.01f, 1000.0f))
            .setViewport(GL::defaultFramebuffer.viewport().size());

        _manipulator.setParent(&_scene);

        GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
        GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);

        _shaderManager.set("color", new Shaders::Phong);
        _shaderManager.set("texture", new Shaders::Phong(Shaders::Phong::Flag::DiffuseTexture, 2));

        _shaderManager.get<Shaders::Phong>("color")
            ->setAmbientColor(0x111111_rgbf)
            .setSpecularColor(0xffffff_rgbf)
            .setShininess(80.0f);

        _shaderManager.get<Shaders::Phong>("texture")
            ->setAmbientColor(0x111111_rgbf)
            .setSpecularColor(0x111111_rgbf)
            .setShininess(80.0f);
    }

    void MagnumApp::addPrimitive(const Trade::MeshData3D& primitive, const Matrix4& transformation, Color4 color)
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

        auto* drawble = new DrawableObject{_shaderManager, *object, _drawables};
        (*drawble)
            .setMeshes(mesh)
            .setColor(color);

        _objects.push_back(object);
        _transformations.push_back(transformation);
    }

    void MagnumApp::Update()
    {
        // double angle = 0.01;
        // _transformations[0] = _transformations[0] * Matrix4::rotationX(Rad(angle));
        // _transformations[1] = _transformations[1] * Matrix4::rotationY(Rad(angle));

        // _objects[0]->setTransformation(_transformations[0]);
        // _objects[1]->setTransformation(_transformations[1]);
    }

    void MagnumApp::drawEvent()
    {
        GL::defaultFramebuffer.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth);

        Update();

        _camera->draw(_drawables);

        swapBuffers();

        redraw();
    }

    void MagnumApp::importScene(const std::string& file, const std::string& importer_type = "AnySceneImporter")
    {
        PluginManager::Manager<Trade::AbstractImporter> manager;
        Containers::Pointer<Trade::AbstractImporter> importer = manager.loadAndInstantiate(importer_type);
        if (!importer)
            std::exit(1);

        Debug{} << "Opening file" << file;

        /* Load file */
        if (!importer->openFile(file))
            std::exit(4);

        /* Load all textures. Textures that fail to load will be NullOpt. */
        Containers::Array<Containers::Optional<GL::Texture2D>> textures{importer->textureCount()};
        for (UnsignedInt i = 0; i != importer->textureCount(); ++i) {
            Debug{} << "Importing texture" << i << importer->textureName(i);

            Containers::Optional<Trade::TextureData> textureData = importer->texture(i);
            if (!textureData || textureData->type() != Trade::TextureData::Type::Texture2D) {
                Warning{} << "Cannot load texture properties, skipping";
                continue;
            }

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

        /* Load all materials. Materials that fail to load will be NullOpt. The
        data will be stored directly in objects later, so save them only
        temporarily. */
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

        /* Load all meshes. Meshes that fail to load will be NullOpt. */
        Containers::Array<Containers::Optional<GL::Mesh>> meshes{importer->mesh3DCount()};
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
                addObject(*importer, _manipulator, meshes, materials, textures, objectId);
        }
    }

    void MagnumApp::addObject(
        Trade::AbstractImporter& importer,
        Object3D& parent,
        Containers::ArrayView<Containers::Optional<GL::Mesh>> meshes,
        Containers::ArrayView<Containers::Optional<Trade::PhongMaterialData>> materials,
        Containers::ArrayView<Containers::Optional<GL::Texture2D>> textures,
        UnsignedInt i)
    {

        Debug{} << "Importing object" << i << importer.object3DName(i);
        Containers::Pointer<Trade::ObjectData3D> objectData = importer.object3D(i);
        if (!objectData) {
            Error{} << "Cannot import object, skipping";
            return;
        }

        /* Add the object to the scene and set its transformation */
        auto* object = new Object3D{&parent};
        object->setTransformation(objectData->transformation());

        auto* drawble = new DrawableObject{_shaderManager, *object, _drawables};

        /* Add a drawable if the object has a mesh and the mesh is loaded */
        if (objectData->instanceType() == Trade::ObjectInstanceType3D::Mesh && objectData->instance() != -1 && meshes[objectData->instance()]) {
            const Int materialId = static_cast<Trade::MeshObjectData3D*>(objectData.get())->material();

            /* Material not available / not loaded, use a default material */
            if (materialId == -1 || !materials[materialId]) {
                (*drawble)
                    .setMeshes(*meshes[objectData->instance()])
                    .setColor(0xffffff_rgbf);

                /* Textured material. If the texture failed to load, again just use a
            default colored material. */
            }
            else if (materials[materialId]->flags() & Trade::PhongMaterialData::Flag::DiffuseTexture) {
                Containers::Optional<GL::Texture2D>& texture = textures[materials[materialId]->diffuseTexture()];
                if (texture)
                    (*drawble)
                        .setMeshes(*meshes[objectData->instance()])
                        .setTextures(*texture);
                else
                    (*drawble)
                        .setMeshes(*meshes[objectData->instance()])
                        .setColor(0xffffff_rgbf);

                /* Color-only material */
            }
            else {
                (*drawble)
                    .setMeshes(*meshes[objectData->instance()])
                    .setColor(materials[materialId]->diffuseColor());
            }
        }

        /* Recursively add children */
        for (std::size_t id : objectData->children())
            addObject(importer, *object, meshes, materials, textures, id);
    }

    void MagnumApp::viewportEvent(ViewportEvent& event)
    {
        GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
        _camera->setViewport(event.windowSize());
    }

    void MagnumApp::mousePressEvent(MouseEvent& event)
    {
        if (event.button() == MouseEvent::Button::Left)
            _previousPosition = positionOnSphere(event.position());
    }

    void MagnumApp::mouseReleaseEvent(MouseEvent& event)
    {
        if (event.button() == MouseEvent::Button::Left)
            _previousPosition = Vector3();
    }

    void MagnumApp::mouseScrollEvent(MouseScrollEvent& event)
    {
        if (!event.offset().y())
            return;

        /* Distance to origin */
        const Float distance = _cameraObject.transformation().translation().z();

        /* Move 15% of the distance back or forward */
        _cameraObject.translate(Vector3::zAxis(
            distance * (1.0f - (event.offset().y() > 0 ? 1 / 0.85f : 0.85f))));

        redraw();
    }

    Vector3 MagnumApp::positionOnSphere(const Vector2i& position) const
    {
        const Vector2 positionNormalized = Vector2{position} / Vector2{_camera->viewport()} - Vector2{0.5f};
        const Float length = positionNormalized.length();
        const Vector3 result(length > 1.0f ? Vector3(positionNormalized, 0.0f) : Vector3(positionNormalized, 1.0f - length));
        return (result * Vector3::yScale(-1.0f)).normalized();
    }

    void MagnumApp::mouseMoveEvent(MouseMoveEvent& event)
    {
        if (!(event.buttons() & MouseMoveEvent::Button::Left))
            return;

        const Vector3 currentPosition = positionOnSphere(event.position());
        const Vector3 axis = Math::cross(_previousPosition, currentPosition);

        if (_previousPosition.length() < 0.001f || axis.length() < 0.001f)
            return;

        _manipulator.rotate(Math::angle(_previousPosition, currentPosition), axis.normalized());
        _previousPosition = currentPosition;

        redraw();
    }
} // namespace magnum_dynamics