#ifndef GFX_DITHER_H
#define GFX_DITHER_H

#include <cmath>

#include "gfx/surface.h"

namespace gfx {

    void vga_dither(unsigned char *data, vga_surface &surface, const vga_palette &palette, float contrast = 1.0f,
                    float brightness = 0.0f);


}


#endif
