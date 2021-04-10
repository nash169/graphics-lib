#include "magnum_dynamics/MagnumApp.hpp"

#include "magnum_dynamics/tools/colormaps.hpp"
#include "magnum_dynamics/tools/math.hpp"

namespace magnum_dynamics {
    MagnumApp::MagnumApp(const Arguments& arguments)
        : Platform::Application{arguments, NoCreate}
    {
        /* Try 8x MSAA, fall back to zero samples if not possible. Enable only 2x MSAA if we have enough DPI. */
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

        /* Create camera */
        _camera.reset(new Camera(_scene));

        /* Basic object parent of all the others */
        // arrayAppend(_objects, Containers::optional<Object3D>(&_scene));
        // _manipulator.setParent(&_scene);
        _manipulator = new Object(&_scene, _drawableObjs); //.setParent(&_scene);

        /* Recall something from OpenGL study but don't precisely */
        GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
        GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
        // GL::Renderer::enable(GL::Renderer::Feature::PolygonOffsetFill);
        // GL::Renderer::setPolygonOffset(2.0f, 0.5f);

        // Set colored shader
        _shadersManager.set<GL::AbstractShaderProgram>("color", new Shaders::Phong{{}, 2});

        _shadersManager.get<GL::AbstractShaderProgram, Shaders::Phong>("color")
            ->setAmbientColor(0x111111_rgbf)
            .setSpecularColor(0xffffff_rgbf)
            .setShininess(80.0f);

        // Set texture shader
        _shadersManager.set<GL::AbstractShaderProgram>("texture", new Shaders::Phong(Shaders::Phong::Flag::DiffuseTexture, 2));

        _shadersManager.get<GL::AbstractShaderProgram, Shaders::Phong>("texture")
            ->setAmbientColor(0x111111_rgbf)
            .setSpecularColor(0x111111_rgbf)
            .setShininess(80.0f);

        // Set vertex shader
        _shadersManager.set<GL::AbstractShaderProgram>("vertex", new Shaders::VertexColor3D);

        // Default importer
        setImporter("AssimpImporter");

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

    // Get object
    Object& MagnumApp::manipulator()
    {
        return *_manipulator;
    }

    SceneGraph::DrawableGroup3D& MagnumApp::drawables()
    {
        return _drawables;
    }

    size_t MagnumApp::numObjects() const
    {
        return _drawableObjects.size();
    }

    Object3D& MagnumApp::addFrame()
    {
        auto axis_mesh = Primitives::axis3D();

        GL::Mesh mesh = MeshTools::compile(axis_mesh);

        auto* obj = new Object3D(_manipulator);

        auto* drwObj = new DrawableObject(*obj, _drawables, _shadersManager);
        drwObj->setMesh(mesh);

        return *obj;
    }

    // Add primitive
    Object& MagnumApp::addPrimitive(const std::string& primitive)
    {
        Trade::MeshData mesh_data = Primitives::cubeSolid();

        if (!primitive.compare("sphere"))
            mesh_data = Primitives::cubeSolid();

        // Vertices
        GL::Buffer vertices;
        vertices.setData(MeshTools::interleave(mesh_data.positions3DAsArray(),
            mesh_data.normalsAsArray()));

        // Indices
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
        // auto it = _drawableObjects.insert(std::make_pair(new Object3D(&_manipulator), nullptr));
        auto it = _drawableObjs.insert(std::make_pair(new Object(_manipulator, _drawableObjs), nullptr));

        // auto* obj = new ObjectCustom(_manipulator);

        // auto obj = new Object(&_manipulator, _drawableObjects);

        // obj->setPrimitiveTransformation(primitive).setTransformation(transformation);

        // auto draw = new DrawableObject(*static_cast<Object3D*>(&obj->object()), _drawables, _shadersManager);
        // auto draw = new DrawableObject(*obj, _drawables, _shadersManager);
        // draw->setMesh(mesh).setColor(color);

        // if (it.second) {
        //     // Set object transformation
        //     it.first->first->setTransformation(transformation);

        //     // Create drawable
        //     // *(static_cast<Object3D*>(&(_objects.back()->object())))
        //     it.first->second = Containers::pointer<DrawableObject>(*it.first->first, _drawables, _shadersManager);

        //     // Set drawable properties
        //     it.first->second->setMesh(mesh).setColor(color).setPrimitiveTransformation(primitive);
        // }

        if (it.second) {
            // Create drawable
            it.first->second = Containers::pointer<DrawableObject>(*it.first->first, _drawables, _shadersManager);

            // Set drawable mesh and default color
            it.first->second->setMesh(mesh).setColor(0xffffff_rgbf);
        }

        return *it.first->first;
    }

    // Add from file
    Object& MagnumApp::import(const std::string& file)
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
            }
            else {
                // check how to do this
                auto ctrObj = new Object(_manipulator, _drawableObjs);

                /* Recursively add all children */
                for (UnsignedInt objectId : sceneData->children3D())
                    addObject(meshes, textures, materials, *_manipulator, objectId);

                return *ctrObj;
            }
        }
        /* The format has no scene support, display just the first loaded mesh with a default material and be done with it */
        else if (!meshes.empty() && meshes[0]) {
            // Create 3D object
            auto it = _drawableObjs.insert(std::make_pair(new Object(_manipulator, _drawableObjs), nullptr));

            if (it.second) {
                // Create drawable
                it.first->second = Containers::pointer<DrawableObject>(*it.first->first, _drawables, _shadersManager);

                // Set drawable mesh and default color
                it.first->second->setMesh(*meshes[0]).setColor(0xffffff_rgbf);
            }

            return *it.first->first;
        }

        return *_manipulator;
    }

    Object& MagnumApp::plot(const std::string& file, const Eigen::VectorXd& x, const std::string& colormap)
    {
        // Check importer
        if (!_importer)
            std::exit(1);

        // Import file
        Debug{} << "Opening file" << file;
        if (!_importer->openFile(file))
            std::exit(4);

        if (_importer->meshCount() != 1) {
            Warning{} << "Cannot plot, zero or multiple meshes present";
            return *_manipulator;
        }

        Debug{} << "Importing mesh" << 0 << _importer->meshName(0);

        Containers::Optional<Trade::MeshData> meshData = _importer->mesh(0);

        if (!meshData || !meshData->hasAttribute(Trade::MeshAttribute::Normal) || meshData->primitive() != MeshPrimitive::Triangles) {
            Warning{} << "Cannot load the mesh, skipping";
        }
        else {
            auto map = tools::Turbo;

            Containers::Array<Color4> colorsArray(x.rows());

            Eigen::VectorXi mapToColors = tools::mapColors(x, x.minCoeff(), x.maxCoeff(), 256);

            size_t iter = 0;
            for (auto& color : colorsArray) {
                color = Color4(mapToColors(iter));
                iter++;
            }

            std::cout << "Index count: " << meshData->vertexCount() << std::endl;

            GL::Buffer vertices;
            vertices.setData(MeshTools::interleave(meshData->positions3DAsArray(), colorsArray));

            std::pair<Containers::Array<char>, MeshIndexType> compressed = MeshTools::compressIndices(meshData->indicesAsArray());
            GL::Buffer indices;
            indices.setData(compressed.first);

            GL::Mesh mesh;

            mesh
                .setPrimitive(meshData->primitive())
                .setCount(meshData->indexCount())
                .addVertexBuffer(std::move(vertices), 0, Shaders::VertexColor3D::Position{}, Shaders::VertexColor3D::Color4{})
                .setIndexBuffer(std::move(indices), 0, compressed.second);

            auto it = _drawableObjs.insert(std::make_pair(new Object(_manipulator, _drawableObjs), nullptr));

            if (it.second) {
                // Create drawable
                it.first->second = Containers::pointer<DrawableObject>(*it.first->first, _drawables, _shadersManager);

                // Set drawable mesh
                it.first->second->setMesh(mesh);
            }

            return *it.first->first;
        }

        return *_manipulator;
    }

    void MagnumApp::addObject(Containers::ArrayView<Containers::Optional<GL::Mesh>> meshes,
        Containers::ArrayView<Containers::Optional<GL::Texture2D>> textures,
        Containers::ArrayView<Containers::Optional<Trade::PhongMaterialData>> materials,
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
        auto it = _drawableObjects.insert(std::make_pair(new Object(&parent, _drawableObjs), nullptr));

        if (it.second) {
            // Set object transformation
            it.first->first->setTransformation(objectData->transformation());

            /* Add a drawable if the object has a mesh and the mesh is loaded */
            if (objectData->instanceType() == Trade::ObjectInstanceType3D::Mesh && objectData->instance() != -1 && meshes[objectData->instance()]) {
                const Int materialId = static_cast<Trade::MeshObjectData3D*>(objectData.get())->material();

                /* Material not available / not loaded, use a default material */
                if (materialId == -1 || !materials[materialId]) {
                    it.first->second = Containers::pointer<DrawableObject>(*it.first->first, _drawables, _shadersManager);

                    it.first->second
                        ->setMesh(*meshes[objectData->instance()])
                        .setColor(0xffffff_rgbf); // Default color
                }
                /* Textured material. If the texture failed to load, again just use adefault colored material. */
                else if (materials[materialId]->hasAttribute(Trade::MaterialAttribute::DiffuseTexture)) {
                    Containers::Optional<GL::Texture2D>& texture = textures[materials[materialId]->diffuseTexture()];

                    it.first->second = Containers::pointer<DrawableObject>(*it.first->first, _drawables, _shadersManager);

                    it.first->second
                        ->setMesh(*meshes[objectData->instance()]);

                    if (texture)
                        it.first->second
                            ->setTexture(*texture);
                    else
                        it.first->second
                            ->setColor(0xffffff_rgbf);
                }
                /* Not textured material */
                else {
                    it.first->second = Containers::pointer<DrawableObject>(*it.first->first, _drawables, _shadersManager);

                    it.first->second
                        ->setMesh(*meshes[objectData->instance()])
                        .setMaterial(*materials[materialId])
                        .setColor(materials[materialId]->diffuseColor()); // set color by default but it should not be used
                }
            }

            /* Recursively add children */
            for (std::size_t id : objectData->children())
                addObject(meshes, textures, materials, *it.first->first, id);
        }
    }

    void MagnumApp::drawEvent()
    {
        GL::defaultFramebuffer.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth);

        _camera->camera()->draw(_drawables);

        swapBuffers();

        redraw();
    }

    void MagnumApp::viewportEvent(ViewportEvent& event)
    {
        GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
        _camera->camera()->setViewport(event.windowSize());
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
        const Vector3 distance = (*_camera->object()).transformation().translation();

        /* Move 15% of the distance back or forward */
        (*_camera->object()).translate(distance * (1.0f - (event.offset().y() > 0 ? 1 / 0.85f : 0.85f)));

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

        _manipulator->rotate(Math::angle(_previousPosition, currentPosition), axis.normalized());
        _previousPosition = currentPosition;

        redraw();
    }

    Vector3 MagnumApp::positionOnSphere(const Vector2i& position) const
    {
        const Vector2 positionNormalized = Vector2{position} / Vector2{_camera->camera()->viewport()} - Vector2{0.5f};
        const Float length = positionNormalized.length();
        const Vector3 result(length > 1.0f ? Vector3(positionNormalized, 0.0f) : Vector3(positionNormalized, 1.0f - length));
        return (result * Vector3::yScale(-1.0f)).normalized();
    }
} // namespace magnum_dynamics