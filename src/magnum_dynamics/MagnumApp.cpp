#include "magnum_dynamics/MagnumApp.hpp"

#include "magnum_dynamics/tools/colormaps.hpp"
#include "magnum_dynamics/tools/math.hpp"

#include <Corrade/Containers/StridedArrayView.h>
#include <Magnum/MeshTools/Duplicate.h>

#include <utils_cpp/UtilsCpp.hpp>

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
        _manipulator = new Object(&_scene, _drawableObjects); //.setParent(&_scene);

        /* Recall something from OpenGL study but don't precisely */
        GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
        GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
        // GL::Renderer::setClearColor(0xffffff_rgbf);
        // GL::Renderer::enable(GL::Renderer::Feature::PolygonOffsetFill);
        // GL::Renderer::setPolygonOffset(2.0f, 0.5f);

        // Set colored shader
        _shadersManager.set<GL::AbstractShaderProgram>("color", new Shaders::PhongGL{{}, 2});

        _shadersManager.get<GL::AbstractShaderProgram, Shaders::PhongGL>("color")
            ->setAmbientColor(0x111111_rgbf)
            .setSpecularColor(0xffffff_rgbf)
            .setShininess(80.0f);

        // Set texture shader
        _shadersManager.set<GL::AbstractShaderProgram>("texture", new Shaders::PhongGL(Shaders::PhongGL::Flag::DiffuseTexture, 2));

        _shadersManager.get<GL::AbstractShaderProgram, Shaders::PhongGL>("texture")
            ->setAmbientColor(0x111111_rgbf)
            .setSpecularColor(0x111111_rgbf)
            .setShininess(80.0f);

        // Set vertex shader
        _shadersManager.set<GL::AbstractShaderProgram>("vertex", new Shaders::VertexColorGL3D);

        // Default importer
        _importer = _manager.loadAndInstantiate("AnySceneImporter");

        /* Loop at 60 Hz max */
        setSwapInterval(1);
        setMinimalLoopPeriod(16);

        redraw();
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
        // Default mesh cube
        Trade::MeshData mesh_data = Primitives::cubeSolid();

        if (!primitive.compare("sphere"))
            mesh_data = Primitives::icosphereSolid(3);
        else if (!primitive.compare("capsule"))
            mesh_data = Primitives::capsule3DSolid(10, 10, 30, 0.5);
        else if (!primitive.compare("cone"))
            mesh_data = Primitives::coneSolid(10, 30, 1);
        else if (!primitive.compare("cylinder"))
            mesh_data = Primitives::cylinderSolid(10, 30, 1);

        // Vertices
        GL::Buffer vertices;
        vertices.setData(MeshTools::interleave(mesh_data.positions3DAsArray(),
            mesh_data.normalsAsArray()));

        // Indices
        std::pair<Containers::Array<char>, MeshIndexType> compressed = MeshTools::compressIndices(mesh_data.indicesAsArray());
        GL::Buffer indices;
        indices.setData(compressed.first);

        // Mesh
        GL::Mesh mesh;
        mesh
            .setPrimitive(mesh_data.primitive())
            .setCount(mesh_data.indexCount())
            .addVertexBuffer(std::move(vertices), 0, Shaders::PhongGL::Position{},
                Shaders::PhongGL::Normal{})
            .setIndexBuffer(std::move(indices), 0, compressed.second);

        // Create object
        auto it = _drawableObjects.insert(std::make_pair(new Object(_manipulator, _drawableObjects), nullptr));

        // Add drawable
        if (it.second) {
            // Create drawable
            it.first->second = Containers::pointer<DrawableObject>(*it.first->first, _drawables, _shadersManager);

            // Set drawable mesh and default color
            it.first->second->setMesh(mesh).setColor(0xffffff_rgbf);
        }

        return *it.first->first;
    }

    // Plot from vertices and indices matrices
    Object& MagnumApp::plot(const Eigen::MatrixXd& vertices, const Eigen::VectorXd& function, const Eigen::MatrixXd& indices, const double& min, const double& max, const std::string& colormap)
    {
        // Colormap (this should be set from the outside)
        auto map = tools::Turbo;

        // Get colors for each value function
        Eigen::VectorXi vertex2Color = tools::mapColors(function, min, max, 256);

        // Data structure (positions and colors interleaved)
        struct VertexData {
            Vector3 position;
            Color3 color;
        };

        // Fill data structure
        Containers::Array<VertexData> data;
        for (size_t i = 0; i < indices.rows(); i++)
            for (size_t j = 0; j < indices.cols(); j++) {
                size_t index = indices(i, j);
                Eigen::Vector3f vertex = vertices.row(index).cast<float>();
                arrayAppend(data, Corrade::InPlaceInit, Vector3(vertex),
                    Color3{map[vertex2Color(index)][0], map[vertex2Color(index)][1], map[vertex2Color(index)][2]});
            }

        // Create buffer
        GL::Buffer buffer;
        buffer.setData(data);

        // Create mesh
        GL::Mesh mesh;
        mesh.setCount(data.size())
            .addVertexBuffer(std::move(buffer), 0,
                Shaders::VertexColorGL3D::Position{},
                Shaders::VertexColorGL3D::Color3{});

        // Add object - drawable connection
        auto it = _drawableObjects.insert(std::make_pair(new Object(_manipulator, _drawableObjects), nullptr));

        // Add drawable
        if (it.second) {
            // Create drawable
            it.first->second = Containers::pointer<DrawableObject>(*it.first->first, _drawables, _shadersManager);

            // Set drawable mesh
            it.first->second->setMesh(mesh);
        }

        return *it.first->first;
    }

    // Add from file
    Object& MagnumApp::import(const std::string& file, const std::string& importer)
    {
        // Set importer
        if (!importer.empty())
            _importer = _manager.loadAndInstantiate(importer);

        // Check importer
        if (!_importer)
            std::exit(1);

        // Import file
        Debug{} << "Opening file" << file;
        if (!_importer->openFile(file))
            std::exit(4);

        /* Textures */
        Containers::Array<Containers::Optional<GL::Texture2D>> textures{_importer->textureCount()};

        for (UnsignedInt i = 0; i != _importer->textureCount(); ++i) {
            // Import texture
            Debug{} << "Importing texture" << i << _importer->textureName(i);
            Containers::Optional<Trade::TextureData> textureData = _importer->texture(i);
            if (!textureData || textureData->type() != Trade::TextureType::Texture2D) {
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

        /* Materials */
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

        /* Meshes */
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
                auto ctrObj = new Object(_manipulator, _drawableObjects);

                /* Recursively add all children */
                for (UnsignedInt objectId : sceneData->children3D())
                    addObject(meshes, textures, materials, *ctrObj, objectId);

                /* Set prior transformation  */
                for (auto& child : ctrObj->children())
                    transformation2Prior(static_cast<Object*>(&child), ctrObj->transformation());

                return *ctrObj;
            }
        }
        /* The format has no scene support, display just the first loaded mesh with a default material and be done with it */
        else if (!meshes.empty() && meshes[0]) {
            // Create 3D object
            auto it = _drawableObjects.insert(std::make_pair(new Object(_manipulator, _drawableObjects), nullptr));

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

    void MagnumApp::addObject(Containers::ArrayView<Containers::Optional<GL::Mesh>> meshes,
        Containers::ArrayView<Containers::Optional<GL::Texture2D>> textures,
        Containers::ArrayView<Containers::Optional<Trade::PhongMaterialData>> materials,
        Object3D& parent, UnsignedInt i)
    {
        /* Import the object information */
        Debug{} << "Importing object" << i << _importer->object3DName(i);
        Containers::Pointer<Trade::ObjectData3D> objectData = _importer->object3D(i);
        if (!objectData) {
            Error{} << "Cannot import object, skipping";
            return;
        }

        /* Create 3D object */
        auto object = new Object(&parent, _drawableObjects);

        /* Object transformation */
        // Debug{} << objectData->transformation();
        object->setTransformation(objectData->transformation());
        // object->addPriorTransformation(objectData->transformation());

        /* Add a drawable if the object has a mesh and the mesh is loaded */
        if (objectData->instanceType() == Trade::ObjectInstanceType3D::Mesh && objectData->instance() != -1 && meshes[objectData->instance()]) {
            // Add object to the unordered map
            auto it = _drawableObjects.insert(std::make_pair(object, nullptr));

            if (it.second) {
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

                    it.first->second->setMesh(*meshes[objectData->instance()]);

                    if (texture)
                        it.first->second->setTexture(*texture);
                    else
                        it.first->second->setColor(0xffffff_rgbf);
                }
                /* Not textured material */
                else {
                    it.first->second = Containers::pointer<DrawableObject>(*it.first->first, _drawables, _shadersManager);

                    it.first->second
                        ->setMesh(*meshes[objectData->instance()])
                        // .setMaterial(*materials[materialId]) // correct here (check with reference example)
                        .setColor(materials[materialId]->diffuseColor()); // set color by default but it should not be used
                }
            }
        }

        /* Recursively add children */
        for (std::size_t id : objectData->children())
            addObject(meshes, textures, materials, *object, id);
    }

    bool MagnumApp::transformation2Prior(Object* obj, Matrix4 transformation)
    {
        // Transformation
        transformation = transformation * obj->transformation();

        // Set prior transformation for the (drawable feature)
        for (auto& feature : obj->features())
            static_cast<DrawableObject&>(feature).addPriorTransformation(transformation);

        // Reset tranformation
        obj->resetTransformation();

        // Recursion
        for (auto& child : obj->children())
            transformation2Prior(static_cast<Object*>(&child), transformation);

        return true;
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
        if (event.offset().y())
            _camera->translate(event.offset().y());

        redraw();
    }

    void MagnumApp::mouseMoveEvent(MouseMoveEvent& event)
    {
        if (event.buttons() == MouseMoveEvent::Button::Left) {
            _camera->move(event.relativePosition());
        }

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

// Object& MagnumApp::plot(const std::string& file, const Eigen::VectorXd& x, const std::string& colormap)
//     {
//         setImporter("AssimpImporter");
//         // _importer->configuration()

//         // Check importer
//         if (!_importer)
//             std::exit(1);

//         // Import file
//         Debug{} << "Opening file" << file;
//         if (!_importer->openFile(file))
//             std::exit(4);

//         if (_importer->meshCount() != 1) {
//             Warning{} << "Cannot plot, zero or multiple meshes present";
//             return *_manipulator;
//         }

//         Debug{} << "Importing mesh" << 0 << _importer->meshName(0);

//         Containers::Optional<Trade::MeshData> meshData = _importer->mesh(0);

//         if (!meshData || meshData->primitive() != MeshPrimitive::Triangles) {
//             Warning{} << "Cannot load the mesh, skipping";
//         }
//         else {
//             auto map = tools::Turbo;

//             struct VertexData {
//                 Vector3 position;
//                 Color3 color;
//             };

//             Containers::Array<VertexData> data;

//             /* Plot the loaded mesh */
//             Eigen::VectorXi vertex2Color = tools::mapColors(x, x.minCoeff(), x.maxCoeff(), 256);
//             for (auto& i : meshData->indicesAsArray())
//                 arrayAppend(data, Corrade::InPlaceInit, meshData->positions3DAsArray()[i], Color3{map[vertex2Color(i)][0], map[vertex2Color(i)][1], map[vertex2Color(i)][2]});

//             /* Plot the cube */
//             // Trade::MeshData cube_mesh = Primitives::cubeSolid();
//             // Eigen::VectorXd val = Eigen::VectorXd::Random(24);
//             // Eigen::VectorXi vertex2Color = tools::mapColors(val, val.minCoeff(), val.maxCoeff(), 256);
//             // for (auto& i : cube_mesh.indicesAsArray())
//             //     arrayAppend(data, Containers::InPlaceInit, cube_mesh.positions3DAsArray()[i], Color3{map[vertex2Color(i)][0], map[vertex2Color(i)][1], map[vertex2Color(i)][2]});

//             GL::Buffer buffer;
//             buffer.setData(data);

//             GL::Mesh mesh;
//             mesh.setPrimitive(MeshPrimitive::Triangles)
//                 .setCount(data.size())
//                 .addVertexBuffer(std::move(buffer), 0,
//                     Shaders::VertexColorGL3D::Position{},
//                     Shaders::VertexColorGL3D::Color3{});

//             auto it = _drawableObjs.insert(std::make_pair(new Object(_manipulator, _drawableObjs), nullptr));

//             if (it.second) {
//                 // Create drawable
//                 it.first->second = Containers::pointer<DrawableObject>(*it.first->first, _drawables, _shadersManager);

//                 // Set drawable mesh
//                 it.first->second->setMesh(mesh);
//             }

//             return *it.first->first;
//         }

//         return *_manipulator;
//     }