#ifndef SCIENCEGRAPHICS_SCIENCEGRAPHICS_HPP
#define SCIENCEGRAPHICS_SCIENCEGRAPHICS_HPP

/* STD LIBRARY */
#include <iostream>
#include <map>
#include <memory>
#include <type_traits>
#include <unordered_map>

/* MAGNUM MAIN */
#include <Magnum/Magnum.h>

/* WINDOW APPLICATION */
#include <Magnum/Platform/Sdl2Application.h>

/* ARGUMENT PARSER */
#include <Corrade/Utility/Arguments.h>

/* TRANSFORMATIONS */
#include <Magnum/SceneGraph/MatrixTransformation2D.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>

/* SCENE */
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/SceneGraph/SceneGraph.h>

/* SHADERS MANAGER */
#include <Magnum/ResourceManager.h>

/* ABSTRACT IMPORTER & MANAGER */
#include <Corrade/PluginManager/Manager.h>
#include <Magnum/Trade/AbstractImporter.h>

/* INTEGRATIONS */
#include <Magnum/EigenIntegration/Integration.h>

/* CONTAINERS */
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/Pointer.h>

using namespace Corrade;
using namespace Magnum;
using namespace Math::Literals;

/* CAMERAS */
#include "science_graphics/cameras/CameraHandle.hpp"
#include "science_graphics/cameras/Cameras.h"

/* OBJECTS */
#include "science_graphics/objects/ObjectHandle.hpp"
#include "science_graphics/objects/Objects.h"

/* HELPERS */
#include "science_graphics/tools/helper.hpp"

namespace science_graphics {
    class ScienceGraphics : public Platform::Application {
    public:
        explicit ScienceGraphics(const Arguments& arguments);

        ~ScienceGraphics()
        {
            _drawables3D.clear();
            _drawables2D.clear();
        }

        // Get camera
        cameras::CameraHandle3D& camera3D() { return *_cameraTemp3D; }
        cameras::CameraHandle2D& camera2D() { return *_cameraTemp2D; }

        // Get manipulator (to access all the objects)
        objects::ObjectHandle3D& manipulator() { return *_manipulator; }

        // Get number objects
        size_t numObjects() const { return _drawables3D.size() + _drawables2D.size(); }

        // Set window background
        ScienceGraphics& setBackground(const std::string& colorname);

        // Cartesian frame
        objects::ObjectHandle3D& addFrame();

        // Add primitive
        objects::ObjectHandle3D& addPrimitive(const std::string& primitive);

        // Colorbar (attached to the window)
        objects::ObjectHandle2D& colorbar(const double& min, const double& max, const std::string& colormap = "turbo");

        // To plot a surface basically but it can be more general
        objects::ObjectHandle3D& surf(const Eigen::MatrixXd& vertices, const Eigen::VectorXd& fun, const Eigen::MatrixXd& indices, const double& min = -1, const double& max = 1, const std::string& colormap = "turbo");

        // Import from file (return object parent of all the objects inside the file)
        objects::ObjectHandle3D& import(const std::string& file, const std::string& importer = "");

    protected:
        // Draw
        void drawEvent() override;

        // Recursively add objects from meshes
        void addObject(Containers::ArrayView<Containers::Optional<GL::Mesh>> meshes,
            Containers::ArrayView<Containers::Optional<GL::Texture2D>> textures,
            Containers::ArrayView<Containers::Optional<Trade::PhongMaterialData>> materials,
            objects::ObjectHandle3D& parent, UnsignedInt i);

        // Transmit transformation
        bool transformation2Prior(objects::ObjectHandle3D* obj, Matrix4 transformation);

        // Colormap
        Containers::StaticArrayView<256, const Vector3ub> colormap(const std::string& map) const;

        // Handle multiple shaders
        ResourceManager<GL::AbstractShaderProgram> _shadersManager;

        // Scene
        SceneGraph::Scene<SceneGraph::MatrixTransformation2D> _scene2D;
        SceneGraph::Scene<SceneGraph::MatrixTransformation3D> _scene3D;

        // Camera
        Containers::Pointer<cameras::CameraHandle2D> _cameraTemp2D;
        Containers::Pointer<cameras::CameraHandle3D> _cameraTemp3D;

        // Parent object
        objects::ObjectHandle3D* _manipulator;

        // Unordered map object -> drawable
        std::unordered_map<objects::ObjectHandle2D*, Containers::Pointer<drawables::AbstractDrawable2D>> _drawables2D;
        std::unordered_map<objects::ObjectHandle3D*, Containers::Pointer<drawables::AbstractDrawable3D>> _drawables3D;

        // Drawables (it would be better to have in an optional container)
        SceneGraph::DrawableGroup2D _color2D;
        SceneGraph::DrawableGroup3D _phong3D, _texture3D, _color3D;

        // Manager (to set importer) & importer
        PluginManager::Manager<Trade::AbstractImporter> _manager;
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
} // namespace science_graphics

#endif // SCIENCEGRAPHICS_SCIENCEGRAPHICS_HPP