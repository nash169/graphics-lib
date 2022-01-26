#ifndef SCIENCEGRAPHICS_DRAWABLES_DRAWABLES_H
#define SCIENCEGRAPHICS_DRAWABLES_DRAWABLES_H

namespace science_graphics {
    namespace drawables {
        template <size_t>
        class AbstractDrawable;
        typedef AbstractDrawable<3> AbstractDrawable3D;
        typedef AbstractDrawable<2> AbstractDrawable2D;

        template <size_t>
        class ColorDrawable;
        typedef ColorDrawable<3> ColorDrawable3D;
        typedef ColorDrawable<2> ColorDrawable2D;

        template <size_t>
        class PhongDrawable;
        typedef PhongDrawable<3> PhongDrawable3D;
        typedef PhongDrawable<2> PhongDrawable2D;

        template <size_t>
        class TextureDrawable;
        typedef TextureDrawable<3> TextureDrawable3D;
        typedef TextureDrawable<2> TextureDrawable2D;
    } // namespace drawables
} // namespace science_graphics

#endif // SCIENCEGRAPHICS_DRAWABLES_DRAWABLES_H