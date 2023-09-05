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

#ifndef GRAPHICSLIB_GRAPHICS_HPP
#define GRAPHICSLIB_GRAPHICS_HPP

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
#include "graphics_lib/cameras/CameraHandle.hpp"
#include "graphics_lib/cameras/Cameras.h"

/* OBJECTS */
#include "graphics_lib/objects/ObjectHandle.hpp"
#include "graphics_lib/objects/Objects.h"

/* HELPERS */
#include "graphics_lib/tools/helper.hpp"

namespace graphics_lib {
    class Graphics : public Platform::Application {
    public:
        explicit Graphics(const Arguments& arguments);
        ~Graphics();

        /* GETTERS ======================================== */

        // Get camera objects
        cameras::CameraHandle3D& camera3D() { return *_cameraTemp3D; }
        cameras::CameraHandle2D& camera2D() { return *_cameraTemp2D; }

        // Get manipulator (to access all the objects)
        objects::ObjectHandle3D& manipulator() { return *_manipulator; }

        // Get number objects
        size_t numObjects() const { return _drawables3D.size() + _drawables2D.size(); }

        /* ================================================== */

        /* SETTERS ======================================== */

        // Set window background
        Graphics& setBackground(const std::string& colorname);

        /* ================================================== */

        /* DRAWINGS ======================================== */

        // Draw a 3D Cartesian frame
        objects::ObjectHandle3D& frame();

        // Draw a 3D trajectory
        objects::ObjectHandle3D& trajectory(const Eigen::Matrix<double, Eigen::Dynamic, 3>& trajectory, const std::string& color_to_set = "green");

        // Draw a 3D primitive shape
        objects::ObjectHandle3D& primitive(const std::string& primitive);

        // Draw a 2D (gradient colored) surface
        objects::ObjectHandle3D& surface(const Eigen::MatrixXd& vertices, const Eigen::VectorXd& fun, const Eigen::MatrixXd& indices, const double& min = -1, const double& max = 1, const std::string& colormap = "turbo");

        // Draw from file (return object parent of all the objects inside the file)
        objects::ObjectHandle3D& import(const std::string& file, const std::string& importer = "");

        // Draw a 2Dcolorbar (attached to the window)
        objects::ObjectHandle2D& colorbar(const double& min, const double& max, const std::string& colormap = "turbo");

        /* ================================================== */

    protected:
        // Draw
        void drawEvent() override;

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
} // namespace graphics_lib

#endif // GRAPHICSLIB_GRAPHICS_HPP