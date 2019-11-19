#ifndef GFX_DITHER_H
#define GFX_DITHER_H

#include <cmath>

#include "gfx/surface.h"

namespace gfx {

    void vga_dither(unsigned char *data, vga_surface &surface, float contrast, float brightness);


}


#endif //NAGARETEKU_DITHER_H
