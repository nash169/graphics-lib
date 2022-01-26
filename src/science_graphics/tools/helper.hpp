#ifndef SCIENCEGRAPHICS_TOOLS_HELPER_HPP
#define SCIENCEGRAPHICS_TOOLS_HELPER_HPP

namespace science_graphics {
    namespace tools {
        // Get colors
        inline Color4 color(const std::string& color)
        {
            if (!color.compare("red"))
                return Color4::red();
            else if (!color.compare("green"))
                return Color4::green();
            else if (!color.compare("blue"))
                return Color4::blue();
            else if (!color.compare("cyan"))
                return Color4::cyan();
            else if (!color.compare("magenta"))
                return Color4::magenta();
            else if (!color.compare("yellow"))
                return Color4::yellow();
            else if (!color.compare("white"))
                return Color4(0xffffff_rgbf);
            else if (!color.compare("grey"))
                return Color4(0x585858_rgbf);
            else if (!color.compare("black"))
                return Color4();
            else {
                std::cerr << "Color not found; setting default color WHITE." << std::endl;
                return Color4(0xffffff_rgbf);
            }
        }
    } // namespace tools
} // namespace science_graphics

#endif // SCIENCEGRAPHICS_TOOLS_HELPER_HPP