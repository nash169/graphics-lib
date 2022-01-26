#ifndef SCIENCEGRAPHICS_OBJECTS_HPP
#define SCIENCEGRAPHICS_OBJECTS_HPP

namespace science_graphics {
    namespace objects {
        template <size_t>
        class ObjectHandle;
        typedef ObjectHandle<2> ObjectHandle2D;
        typedef ObjectHandle<3> ObjectHandle3D;
    } // namespace objects
} // namespace science_graphics

#endif // SCIENCEGRAPHICS_OBJECTS_HPP