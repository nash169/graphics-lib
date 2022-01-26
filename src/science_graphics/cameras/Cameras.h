#ifndef SCIENCEGRAPHICS_CAMERAS_HPP
#define SCIENCEGRAPHICS_CAMERAS_HPP

namespace science_graphics {
    namespace cameras {
        template <size_t>
        class CameraHandle;
        typedef CameraHandle<3> CameraHandle3D;
        typedef CameraHandle<2> CameraHandle2D;
    } // namespace cameras
} // namespace science_graphics

#endif // SCIENCEGRAPHICS_CAMERAS_HPP