#include "magnum_dynamics/MagnumApp.hpp"

namespace magnum_dynamics {
    MagnumApp::MagnumApp(const Arguments& arguments)
        : Platform::Application{arguments, Corrade::Containers::NoCreate}
    {
        {
            const Vector2 dpiScaling = this->dpiScaling({});
            Configuration conf;
            conf.setTitle("Magnum Dynamics")
                // .setSize(conf.size(), dpiScaling)
                .setWindowFlags(Configuration::WindowFlag::Resizable);
            GLConfiguration glConf;
            glConf.setSampleCount(dpiScaling.max() < 2.0f ? 8 : 2);
            if (!tryCreate(conf, glConf))
                create(conf, glConf.setSampleCount(0));
        }

        /* Create the camera object for the scene */
        (*(_cameraRig = new Object3D{&_scene}))
            .translate(Vector3::yAxis(3.0f))
            .rotateY(40.0_degf);
        (*(_cameraObject = new Object3D{_cameraRig}))
            .translate(Vector3::zAxis(20.0f))
            .rotateX(-25.0_degf);
        (*(_camera = new SceneGraph::Camera3D{*_cameraObject}))
            .setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
            .setProjectionMatrix(Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.001f, 100.0f))
            .setViewport(GL::defaultFramebuffer.viewport().size());

        /* Basic object parent of all the others */
        _manipulator.setParent(&_scene);

        /* Recall something from OpenGL study but don't precisely */
        GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
        GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
        // GL::Renderer::enable(GL::Renderer::Feature::PolygonOffsetFill);
        // GL::Renderer::setPolygonOffset(2.0f, 0.5f);

        // // Set colored shader
        _resourceManager.set("color", new Shaders::Phong{{}, 2});

        _resourceManager.get<Shaders::Phong>("color")
            ->setAmbientColor(0x111111_rgbf)
            .setSpecularColor(0xffffff_rgbf)
            .setShininess(80.0f);

        // Set texture shader
        _resourceManager.set("texture", new Shaders::Phong(Shaders::Phong::Flag::DiffuseTexture, 2));

        _resourceManager.get<Shaders::Phong>("texture")
            ->setAmbientColor(0x111111_rgbf)
            .setSpecularColor(0x111111_rgbf)
            .setShininess(80.0f);

        // Default importer
        setImporter("AnySceneImporter");

        // Read main arguments if present and load the scene
        // if (arguments.argc > 1) {
        //     Utility::Arguments args;
        //     args.addArgument("file")
        //         .setHelp("file", "file to load")
        //         .addOption("importer", "AnySceneImporter")
        //         .setHelp("importer", "importer plugin to use")
        //         .addSkippedPrefix("magnum", "engine-specific options")
        //         .setGlobalHelp("Displays a 3D scene file provided on command line.")
        //         .parse(arguments.argc, arguments.argv);

        //     setImporter(args.value("importer")).import(args.value("file"));
        // }

        /* Loop at 60 Hz max */
        setSwapInterval(1);
        setMinimalLoopPeriod(16);

        redraw();
    }

    // Set importer
    MagnumApp& MagnumApp::setImporter(const std::string& importer)
    {
        _importer = _manager.loadAndInstantiate(importer);

        return *this;
    }

    // Get 3D objects
    std::vector<Object3D*> MagnumApp::getObjects()
    {
        return _objects;
    }

    // Import scene
    void MagnumApp::add(const std::string& path,
        const std::string& extension,
        const Matrix4& transformation,
        const Color4& color,
        const Matrix4& primitive)
    {
        if (Corrade::Utility::Directory::isDirectory(path)) {
            for (auto& file : Corrade::Utility::Directory::list(path)) {
                auto pair = Corrade::Utility::Directory::splitExtension(file);
                if (!extension.compare(pair.second)) {
                    import(Corrade::Utility::Directory::join(path, file), transformation, primitive);
                }
            }
        }
        else if (!path.compare("cube"))
            addPrimitive(Primitives::cubeSolid(), transformation, color, primitive);
        else {
            import(path, transformation, primitive);
        }
    }

    // Add from file
    void MagnumApp::import(const std::string& file, const Matrix4& transformation, const Matrix4& primitive)
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
            // Create 3D object
            auto* object = new Object3D{&_manipulator};

            // Set transformation
            object->setTransformation(transformation);

            // Create drawable
            auto drawableObj = new DrawableObject{*object, _drawables, _resourceManager};

            // Set drawable properties
            drawableObj->setMesh(*meshes[0]).setPrimitiveTransformation(primitive);

            // Store object
            _objects.push_back(object);
        }
    }

    // Add primitive
    void MagnumApp::addPrimitive(const Trade::MeshData& mesh_data, const Matrix4& transformation, const Color4& color, const Matrix4& primitive)
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

        // Create 3D object
        auto* object = new Object3D{&_manipulator};

        // Set object transformation
        object->setTransformation(transformation);

        // Create drawable
        auto drawableObj = new DrawableObject{*object, _drawables, _resourceManager};

        // Set drawable properties
        drawableObj->setMesh(mesh).setColor(color).setPrimitiveTransformation(primitive);

        // Store object
        _objects.push_back(object);
    }

    void MagnumApp::addObject(Containers::ArrayView<Containers::Optional<GL::Mesh>> meshes,
        Containers::ArrayView<Containers::Optional<GL::Texture2D>> textures,
        Containers::ArrayView<const Containers::Optional<Trade::PhongMaterialData>> materials,
        Object3D& parent, UnsignedInt i)
    {
        // Import the object information
        Debug{} << "Importing object" << i << _importer->object3DName(i);
        Containers::Pointer<Trade::ObjectData3D> objectData = _importer->object3D(i);
        if (!objectData) {
            Error{} << "Cannot import object, skipping";
            return;
        }

        // Create 3D object
        auto* object = new Object3D{&parent};

        // Set object transformation
        object->setTransformation(objectData->transformation());

        /* Add a drawable if the object has a mesh and the mesh is loaded */
        if (objectData->instanceType() == Trade::ObjectInstanceType3D::Mesh && objectData->instance() != -1 && meshes[objectData->instance()]) {
            const Int materialId = static_cast<Trade::MeshObjectData3D*>(objectData.get())->material();

            /* Material not available / not loaded, use a default material */
            if (materialId == -1 || !materials[materialId]) {
                auto drawableObj = new DrawableObject{*object, _drawables, _resourceManager};
                drawableObj->setMesh(*meshes[objectData->instance()]);
            }
            /* Textured material. If the texture failed to load, again just use adefault colored material. */
            else if (materials[materialId]->hasAttribute(Trade::MaterialAttribute::DiffuseTexture)) {
                Containers::Optional<GL::Texture2D>& texture = textures[materials[materialId]->diffuseTexture()];

                auto drawableObj = new DrawableObject{*object, _drawables, _resourceManager};

                drawableObj->setMesh(*meshes[objectData->instance()]);

                if (texture)
                    drawableObj->setTexture(*texture);
            }
            /* Color-only material */
            else {
                auto drawableObj = new DrawableObject{*object, _drawables, _resourceManager};

                drawableObj->setMesh(*meshes[objectData->instance()]).setColor(materials[materialId]->diffuseColor());
            }
        }

        _objects.push_back(object);

        /* Recursively add children */
        for (std::size_t id : objectData->children())
            addObject(meshes, textures, materials, *object, id);
    }

    void MagnumApp::drawEvent()
    {
        GL::defaultFramebuffer.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth);

        _camera->draw(_drawables);

        swapBuffers();

        redraw();
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
        const Float distance = (*_cameraObject).transformation().translation().z();

        /* Move 15% of the distance back or forward */
        (*_cameraObject).translate(Vector3::zAxis(distance * (1.0f - (event.offset().y() > 0 ? 1 / 0.85f : 0.85f))));

        redraw();
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

    Vector3 MagnumApp::positionOnSphere(const Vector2i& position) const
    {
        const Vector2 positionNormalized = Vector2{position} / Vector2{_camera->viewport()} - Vector2{0.5f};
        const Float length = positionNormalized.length();
        const Vector3 result(length > 1.0f ? Vector3(positionNormalized, 0.0f) : Vector3(positionNormalized, 1.0f - length));
        return (result * Vector3::yScale(-1.0f)).normalized();
    }
} // namespace magnum_dynamics