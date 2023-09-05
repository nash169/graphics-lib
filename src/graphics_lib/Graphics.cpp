/*
    This file is part of graphics-lib.

    Copyright (c) 2020, 2021, 2022 Bernardo Fichera <bernardo.fichera@gmail.com>

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#include "graphics_lib/Graphics.hpp"

/* PRIMITIVES */
#include <Magnum/Primitives/Axis.h>
#include <Magnum/Primitives/Capsule.h>
#include <Magnum/Primitives/Cone.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/Primitives/Cylinder.h>
#include <Magnum/Primitives/Gradient.h>
#include <Magnum/Primitives/Icosphere.h>
#include <Magnum/Primitives/Line.h>

/* MATH */
#include "graphics_lib/tools/math.hpp"

/* COLORMAPS */
#include <Magnum/DebugTools/ColorMap.h>

/* SHADERS */
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Shaders/VertexColorGL.h>

/* IMPORTERS */
#include <MagnumPlugins/AssimpImporter/AssimpImporter.h>

/* CORRADE TOOLS */
#include <Corrade/Containers/Pair.h>
#include <Corrade/Utility/DebugStl.h>

/* GL TOOLS */
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/TextureFormat.h>

/* MESH TOOLS*/
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/MeshTools/CompressIndices.h>
#include <Magnum/MeshTools/Interleave.h>

/* MAGNUM MAIN */
#include <Magnum/ImageView.h>
#include <Magnum/PixelFormat.h>

/* TRADE TOOLS */
#include <Magnum/Trade/ImageData.h>
#include <Magnum/Trade/MeshData.h>
#include <Magnum/Trade/SceneData.h>
#include <Magnum/Trade/TextureData.h>

namespace graphics_lib {
    Graphics::Graphics(const Arguments& arguments)
        : Platform::Application{arguments, NoCreate}
    {
        /* Try 8x MSAA, fall back to zero samples if not possible. Enable only 2x MSAA if we have enough DPI. */
        {
            const Vector2 dpiScaling = this->dpiScaling({});
            Configuration conf;
            conf.setTitle("Science Graphics")
                // .setSize(conf.size(), dpiScaling)
                .setWindowFlags(Configuration::WindowFlag::Resizable);
            GLConfiguration glConf;
            glConf.setSampleCount(dpiScaling.max() < 2.0f ? 8 : 2);
            if (!tryCreate(conf, glConf))
                create(conf, glConf.setSampleCount(0));
        }

        /* Create cameras */
        _cameraTemp2D.reset(new cameras::CameraHandle2D(_scene2D));
        _cameraTemp3D.reset(new cameras::CameraHandle3D(_scene3D));

        /* Basic object parent of all the others */
        _manipulator = new objects::ObjectHandle3D(&_scene3D, _drawables3D);

        /* Recall something from OpenGL study but don't precisely */
        GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
        GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
        // GL::Renderer::setClearColor(0xffffff_rgbf);
        // GL::Renderer::enable(GL::Renderer::Feature::PolygonOffsetFill);
        // GL::Renderer::setPolygonOffset(2.0f, 0.5f);

        // Phong shader
        _shadersManager.set<GL::AbstractShaderProgram>("phong",
            new Shaders::PhongGL{Shaders::PhongGL::Configuration{}.setLightCount(2)});
        _shadersManager.get<GL::AbstractShaderProgram, Shaders::PhongGL>("phong")
            ->setAmbientColor(0x111111_rgbf)
            .setSpecularColor(0xffffff_rgbf)
            .setShininess(80.0f);

        // Texture shader
        _shadersManager.set<GL::AbstractShaderProgram>("texture",
            new Shaders::PhongGL(Shaders::PhongGL::Configuration{}.setFlags(Shaders::PhongGL::Flag::DiffuseTexture).setLightCount(2)));
        _shadersManager.get<GL::AbstractShaderProgram, Shaders::PhongGL>("texture")
            ->setAmbientColor(0x111111_rgbf)
            .setSpecularColor(0x111111_rgbf)
            .setShininess(80.0f);

        // Color shader (2D/3D)
        _shadersManager.set<GL::AbstractShaderProgram>("color3D", new Shaders::VertexColorGL3D);
        _shadersManager.set<GL::AbstractShaderProgram>("color2D", new Shaders::VertexColorGL2D);

        // Default importer
        _importer = _manager.loadAndInstantiate("AnySceneImporter");

        /* Loop at 60 Hz max */
        setSwapInterval(1);
        setMinimalLoopPeriod(16);

        redraw();
    }

    Graphics::~Graphics()
    {
        _drawables3D.clear();
        _drawables2D.clear();
    }

    Graphics& Graphics::setBackground(const std::string& colorname)
    {
        GL::Renderer::setClearColor(tools::color(colorname));
        return *this;
    }

    objects::ObjectHandle3D& Graphics::frame()
    {
        auto axis_mesh = Primitives::axis3D();

        GL::Mesh mesh = MeshTools::compile(axis_mesh);

        // Create object
        auto it = _drawables3D.insert(std::make_pair(new objects::ObjectHandle3D(_manipulator, _drawables3D), nullptr));

        // Add drawable
        if (it.second) {
            // Create drawable
            it.first->second = Containers::pointer<drawables::ColorDrawable3D>(*it.first->first, _color3D, *_shadersManager.get<GL::AbstractShaderProgram, Shaders::VertexColorGL3D>("color3D"));

            // Set drawable mesh
            it.first->second->setMesh(mesh);
        }

        return *it.first->first;
    }

    // Add trajectory (only 3D for the moment)
    objects::ObjectHandle3D& Graphics::trajectory(const Eigen::Matrix<double, Eigen::Dynamic, 3>& trajectory, const std::string& color_to_set)
    {
        // handle object
        auto handle_obj = new objects::ObjectHandle3D(_manipulator, _drawables3D);

        // vertices & indices
        struct VertexData {
            Vector3 position;
            Color3 color;
        };
        Containers::Array<VertexData> vertices{size_t(trajectory.rows())};
        Containers::Array<UnsignedShort> indices;

        for (size_t i = 0; i < trajectory.rows() - 1; i++) {
            Eigen::Vector3f vertex = trajectory.row(i).cast<float>();
            Color3 color = tools::color<Color3>(color_to_set);
            vertices[i] = VertexData{Vector3(vertex), color};
            arrayAppend(indices, i);
            arrayAppend(indices, i + 1);
        }
        Eigen::Vector3f vertex = trajectory.row(trajectory.rows() - 1).cast<float>();
        vertices[trajectory.rows() - 1] = VertexData{Vector3(vertex), Color3::green()};

        // Vertex buffer
        GL::Buffer vertex_buffer;
        vertex_buffer.setData(vertices, GL::BufferUsage::StaticDraw);

        // Index buffer
        GL::Buffer index_buffer;
        index_buffer.setData(indices);

        // Create mesh
        GL::Mesh mesh;
        mesh.setPrimitive(MeshPrimitive::Lines)
            .setCount(indices.size())
            .addVertexBuffer(std::move(vertex_buffer), 0, Shaders::VertexColorGL3D::Position{}, Shaders::VertexColorGL3D::Color3{})
            .setIndexBuffer(std::move(index_buffer), 0, MeshIndexType::UnsignedShort);

        // Create object connected to drawable
        auto it = _drawables3D.insert(std::make_pair(new objects::ObjectHandle3D(handle_obj, _drawables3D), nullptr));

        // Add drawable
        if (it.second) {
            // Create drawable
            it.first->second = Containers::pointer<drawables::ColorDrawable3D>(*it.first->first, _color3D, *_shadersManager.get<GL::AbstractShaderProgram, Shaders::VertexColorGL3D>("color3D"));

            // Set drawable mesh
            it.first->second->setMesh(mesh);
        }

        return *handle_obj;
    }

    // Add primitive
    objects::ObjectHandle3D& Graphics::primitive(const std::string& primitive)
    {
        // Default mesh cube
        Trade::MeshData mesh_data = Primitives::cubeSolid();

        if (!primitive.compare("sphere"))
            mesh_data = Primitives::icosphereSolid(3);
        else if (!primitive.compare("capsule"))
            mesh_data = Primitives::capsule3DSolid(10, 10, 30, 0.5);
        else if (!primitive.compare("cone"))
            mesh_data = Primitives::coneSolid(10, 30, 1, Primitives::ConeFlag::CapEnd);
        else if (!primitive.compare("cylinder"))
            mesh_data = Primitives::cylinderSolid(10, 30, 1, Primitives::CylinderFlag::CapEnds);

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
        auto it = _drawables3D.insert(std::make_pair(new objects::ObjectHandle3D(_manipulator, _drawables3D), nullptr));

        // Add drawable
        if (it.second) {
            // Create drawable
            it.first->second = Containers::pointer<drawables::PhongDrawable3D>(*it.first->first, _phong3D, *_shadersManager.get<GL::AbstractShaderProgram, Shaders::PhongGL>("phong"));

            // Set drawable mesh and default color
            static_cast<drawables::PhongDrawable3D&>(it.first->second->setMesh(mesh)).setColor(0xffffff_rgbf);
        }

        return *it.first->first;
    }

    // Plot from vertices and indices matrices
    objects::ObjectHandle3D& Graphics::surface(const Eigen::MatrixXd& vertices, const Eigen::VectorXd& function, const Eigen::MatrixXd& indices, const double& min, const double& max, const std::string& colorset)
    {
        // Colormap
        const auto map = colormap(colorset);

        // Get colors for each value function
        Eigen::VectorXi vertex2Color = tools::linearMap(function, min, max, map.size());

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
                arrayAppend(data, Corrade::InPlaceInit, Vector3(vertex), Color3::fromSrgb(map[vertex2Color(index)]));
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
        auto it = _drawables3D.insert(std::make_pair(new objects::ObjectHandle3D(_manipulator, _drawables3D), nullptr));

        // Add drawable
        if (it.second) {
            // Create drawable
            it.first->second = Containers::pointer<drawables::ColorDrawable3D>(*it.first->first, _color3D, *_shadersManager.get<GL::AbstractShaderProgram, Shaders::VertexColorGL3D>("color3D"));

            // Set drawable mesh
            it.first->second->setMesh(mesh);
        }

        return *it.first->first;
    }

    objects::ObjectHandle3D& Graphics::import(const std::string& file, const std::string& importer)
    {
        // Set importer
        if (!importer.empty())
            _importer = _manager.loadAndInstantiate(importer);

        // Check importer & file
        if (!_importer || !_importer->openFile(file))
            std::exit(1);

        /* Textures */
        Containers::Array<Containers::Optional<GL::Texture2D>> textures{_importer->textureCount()};
        for (UnsignedInt i = 0; i != _importer->textureCount(); ++i) {
            Containers::Optional<Trade::TextureData> textureData = _importer->texture(i);
            if (!textureData || textureData->type() != Trade::TextureType::Texture2D) {
                Warning{} << "Cannot load texture" << i << _importer->textureName(i);
                continue;
            }

            Containers::Optional<Trade::ImageData2D> imageData = _importer->image2D(textureData->image());
            if (!imageData || !imageData->isCompressed()) {
                Warning{} << "Cannot load image" << textureData->image() << _importer->image2DName(textureData->image());
                continue;
            }

            (*(textures[i] = GL::Texture2D{}))
                .setMagnificationFilter(textureData->magnificationFilter())
                .setMinificationFilter(textureData->minificationFilter(),
                    textureData->mipmapFilter())
                .setWrapping(textureData->wrapping().xy())
                .setStorage(Math::log2(imageData->size().max()) + 1,
                    GL::textureFormat(imageData->format()), imageData->size())
                .setSubImage(0, {}, *imageData)
                .generateMipmap();
        }

        /* Materials */
        Containers::Array<Containers::Optional<Trade::PhongMaterialData>> materials{_importer->materialCount()};
        for (UnsignedInt i = 0; i != _importer->materialCount(); ++i) {
            Containers::Optional<Trade::MaterialData> materialData;
            if (!(materialData = _importer->material(i))) {
                Warning{} << "Cannot load material" << i << _importer->materialName(i);
                continue;
            }

            materials[i] = std::move(*materialData).as<Trade::PhongMaterialData>();
        }

        /* Meshes */
        Containers::Array<Containers::Optional<GL::Mesh>> meshes{_importer->meshCount()};

        for (UnsignedInt i = 0; i != _importer->meshCount(); ++i) {
            Containers::Optional<Trade::MeshData> meshData;
            if (!(meshData = _importer->mesh(i))) {
                Warning{} << "Cannot load mesh" << i << _importer->meshName(i);
                continue;
            }

            MeshTools::CompileFlags flags;
            if (meshData->hasAttribute(Trade::MeshAttribute::Normal))
                flags |= MeshTools::CompileFlag::GenerateFlatNormals;
            meshes[i] = MeshTools::compile(*meshData, flags);
        }

        /* The format has no scene support, display just the first loaded mesh with
           a default material (if it's there) and be done with it. */
        if (_importer->defaultScene() == -1) {
            if (!meshes.isEmpty() && meshes[0]) {
                auto it = _drawables3D.insert(std::make_pair(new objects::ObjectHandle3D(_manipulator, _drawables3D), nullptr));
                if (it.second) {
                    it.first->second = Containers::pointer<drawables::PhongDrawable3D>(*it.first->first, _phong3D, *_shadersManager.get<GL::AbstractShaderProgram, Shaders::PhongGL>("phong"));
                    static_cast<drawables::PhongDrawable3D&>(it.first->second->setMesh(*meshes[0])).setColor(0xffffff_rgbf);
                }
                return *it.first->first;
            }
            return *_manipulator;
        }

        /* Load the scene */
        Containers::Optional<Trade::SceneData> scene;
        if (!(scene = _importer->scene(_importer->defaultScene())) || !scene->is3D() || !scene->hasField(Trade::SceneField::Parent) || !scene->hasField(Trade::SceneField::Mesh))
            Fatal{} << "Cannot load scene" << _importer->defaultScene() << _importer->sceneName(_importer->defaultScene());

        /* Allocate objects that are part of the hierarchy & assign parent references*/
        Containers::Array<objects::ObjectHandle3D*> objects{std::size_t(scene->mappingBound())};
        Containers::Array<Containers::Pair<UnsignedInt, Int>> parents = scene->parentsAsArray();

        // Here we create a handle object to control all the ones loaded from the file
        // without having to pass through the manipulator object
        auto handle_object = new objects::ObjectHandle3D(_manipulator, _drawables3D);

        for (const Containers::Pair<UnsignedInt, Int>& parent : parents) {
            // std::cout << "Object: " << parent.first() << std::endl;
            // std::cout << "Parent: " << parent.second() << std::endl;
            objects[parent.first()] = new objects::ObjectHandle3D{parent.second() == -1 ? handle_object : objects[parent.second()], _drawables3D};
        }

        /* Add drawables for objects that have a mesh, again ignoring objects that
           are not part of the hierarchy. There can be multiple mesh assignments
           for one object, simply add one drawable for each. */
        for (const Containers::Pair<UnsignedInt, Containers::Pair<UnsignedInt, Int>>& meshMaterial : scene->meshesMaterialsAsArray()) {

            objects::ObjectHandle3D* object = objects[meshMaterial.first()];
            auto it = _drawables3D.insert(std::make_pair(object, nullptr));
            if (!it.second)
                Fatal{} << "Cannot add object to drawables";

            Containers::Optional<GL::Mesh>& mesh = meshes[meshMaterial.second().first()];
            if (!object || !mesh)
                continue;

            Int materialId = meshMaterial.second().second();

            /* Material not available / not loaded, use a default material */
            if (materialId == -1 || !materials[materialId]) {
                it.first->second = Containers::pointer<drawables::PhongDrawable3D>(*it.first->first, _phong3D, *_shadersManager.get<GL::AbstractShaderProgram, Shaders::PhongGL>("phong"));
                static_cast<drawables::PhongDrawable3D&>(it.first->second->setMesh(*mesh))
                    .setColor(0xffffff_rgbf); // Default color
            }
            /* Textured material, if the texture loaded correctly */
            else if (materials[materialId]->hasAttribute(Trade::MaterialAttribute::DiffuseTexture) && textures[materials[materialId]->diffuseTexture()]) {
                it.first->second = Containers::pointer<drawables::TextureDrawable3D>(*it.first->first, _texture3D, *_shadersManager.get<GL::AbstractShaderProgram, Shaders::PhongGL>("texture"));
                static_cast<drawables::TextureDrawable3D&>(it.first->second->setMesh(*mesh))
                    .setTexture(*textures[materials[materialId]->diffuseTexture()]);
            }
            /* Color-only material */
            else {
                it.first->second = Containers::pointer<drawables::PhongDrawable3D>(*it.first->first, _phong3D, *_shadersManager.get<GL::AbstractShaderProgram, Shaders::PhongGL>("phong"));
                static_cast<drawables::PhongDrawable3D&>(it.first->second->setMesh(*mesh))
                    .setColor(materials[materialId]->diffuseColor()); // set color by default but it should not be used
                                                                      // .setMaterial(*materials[materialId]) // correct here (check with reference example)
            }
        }

        /* Set transformations. Objects that are not part of the hierarchy are
           ignored, objects that have no transformation entry retain an identity
           transformation. */
        for (const Containers::Pair<UnsignedInt, Matrix4>& transformation : scene->transformations3DAsArray()) {
            if (objects::ObjectHandle3D* object = objects[transformation.first()])
                object->addPriorTransformation(transformation.second());
        }

        return *handle_object;
    }

    objects::ObjectHandle2D& Graphics::colorbar(const double& min, const double& max, const std::string& colorset)
    {
        // Map
        const auto map = colormap(colorset);

        // Ticks
        Eigen::Matrix<double, 10, 1> ticks = Eigen::VectorXd::LinSpaced(10, min, max);

        // Colors
        Eigen::Matrix<int, 10, 1> colors_index = Eigen::VectorXi::LinSpaced(10, 0, 255);

        // Vertices
        struct VertexData {
            Vector2 position;
            Color3 color;
        };

        Containers::Array<VertexData> vertices;
        for (size_t i = 0; i < 9; i++) {
            // lower triangle
            arrayAppend(vertices, Corrade::InPlaceInit, 0.5 * Vector2{0, float(i) - 1}, Color3::fromSrgb(map[colors_index[i]]));
            arrayAppend(vertices, Corrade::InPlaceInit, 0.5 * Vector2{1, float(i) - 1}, Color3::fromSrgb(map[colors_index[i]]));
            arrayAppend(vertices, Corrade::InPlaceInit, 0.5 * Vector2{1, float(i + 1) - 1}, Color3::fromSrgb(map[colors_index[i + 1]]));

            // upper triangle
            arrayAppend(vertices, Corrade::InPlaceInit, 0.5 * Vector2{0, float(i) - 1}, Color3::fromSrgb(map[colors_index[i]]));
            arrayAppend(vertices, Corrade::InPlaceInit, 0.5 * Vector2{1, float(i + 1) - 1}, Color3::fromSrgb(map[colors_index[i + 1]]));
            arrayAppend(vertices, Corrade::InPlaceInit, 0.5 * Vector2{0, float(i + 1) - 1}, Color3::fromSrgb(map[colors_index[i + 1]]));
        }

        // Mesh
        GL::Mesh mesh;
        mesh.setCount(Containers::arraySize(vertices))
            .addVertexBuffer(GL::Buffer{vertices}, 0,
                Shaders::VertexColorGL2D::Position{},
                Shaders::VertexColorGL2D::Color3{});

        // Object and drawable feature
        auto it = _drawables2D.insert(std::make_pair(new objects::ObjectHandle2D(&_scene2D, _drawables2D), nullptr));

        // Add drawable
        if (it.second) {
            // Create drawable
            it.first->second = Containers::pointer<drawables::ColorDrawable2D>(*it.first->first, _color2D, *_shadersManager.get<GL::AbstractShaderProgram, Shaders::VertexColorGL2D>("color2D"));

            // Set drawable mesh
            it.first->second->setMesh(mesh);
        }

        return *it.first->first;
    }

    void Graphics::drawEvent()
    {
        GL::defaultFramebuffer.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth);

        if (!_phong3D.isEmpty())
            _cameraTemp3D->draw(_phong3D);

        if (!_color3D.isEmpty())
            _cameraTemp3D->draw(_color3D);

        if (!_texture3D.isEmpty())
            _cameraTemp3D->draw(_texture3D);

        if (!_color2D.isEmpty())
            _cameraTemp2D->draw(_color2D);

        swapBuffers();

        redraw();
    }

    Containers::StaticArrayView<256, const Vector3ub> Graphics::colormap(const std::string& map) const
    {
        if (!map.compare("sphere"))
            return DebugTools::ColorMap::turbo();
        else if (!map.compare("capsule"))
            return DebugTools::ColorMap::magma();
        else if (!map.compare("cone"))
            return DebugTools::ColorMap::plasma();
        else if (!map.compare("cylinder"))
            return DebugTools::ColorMap::inferno();
        else if (!map.compare("cylinder"))
            return DebugTools::ColorMap::viridis();
        else
            return DebugTools::ColorMap::turbo();
    }

    void Graphics::viewportEvent(ViewportEvent& event)
    {
        GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
        _cameraTemp3D->setViewport(event.windowSize());

        redraw();
    }

    void Graphics::mousePressEvent(MouseEvent& event)
    {
        if (event.button() == MouseEvent::Button::Left)
            _previousPosition = positionOnSphere(event.position());
    }

    void Graphics::mouseReleaseEvent(MouseEvent& event)
    {
        if (event.button() == MouseEvent::Button::Left)
            _previousPosition = Vector3();
    }

    void Graphics::mouseScrollEvent(MouseScrollEvent& event)
    {
        if (event.offset().y())
            _cameraTemp3D->translate(event.offset().y());

        redraw();
    }

    void Graphics::mouseMoveEvent(MouseMoveEvent& event)
    {
        if (event.buttons() == MouseMoveEvent::Button::Left)
            _cameraTemp3D->move(event.relativePosition());

        redraw();
    }

    Vector3 Graphics::positionOnSphere(const Vector2i& position) const
    {
        const Vector2 positionNormalized = Vector2{position} / Vector2{_cameraTemp3D->viewport()} - Vector2{0.5f};
        const Float length = positionNormalized.length();
        const Vector3 result(length > 1.0f ? Vector3(positionNormalized, 0.0f) : Vector3(positionNormalized, 1.0f - length));

        return (result * Vector3::yScale(-1.0f)).normalized();
    }
} // namespace graphics_lib