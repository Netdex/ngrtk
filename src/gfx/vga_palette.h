//
// Created by netdex on 18/11/19.
//

#ifndef GFX_VGA_PALETTE_H
#define GFX_VGA_PALETTE_H


namespace gfx {
    class vga_palette {
    public:
        vga_palette() {}

    private:
        uint8_t palette_data_[vga::kNumColors * 3];
        size_t size_;
    };
}


#endif //NAGARETEKU_VGA_PALETTE_H
