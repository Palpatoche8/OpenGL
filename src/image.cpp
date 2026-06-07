//************************************************
//*                                              *
//*   TP 1&2    (c) 2017 J. FABRIZIO             *
//*                                              *
//*                               LRDE EPITA     *
//*                                              *
//************************************************

#include "image.hh"
#include <cstdlib>

namespace tifo {

gray8_image::gray8_image(int _sx, int _sy) {
    sx = _sx;
    sy = _sy;

    length = sx*sy;
    pixels = (GRAY8)_aligned_malloc(TL_IMAGE_ALIGNMENT, length);
}

gray8_image::~gray8_image() {
  _aligned_free(pixels);
}

const GRAY8& gray8_image::get_buffer() const {
    return pixels;
}

GRAY8& gray8_image::get_buffer() {
    return pixels;
}

rgb24_image::rgb24_image(int _sx, int _sy) {
    sx = _sx;
    sy = _sy;

    length = sx*sy*3;
    pixels = (RGB8)_aligned_malloc(TL_IMAGE_ALIGNMENT, length);
}

rgb24_image::~rgb24_image() {
  _aligned_free(pixels);
}

const RGB8& rgb24_image::get_buffer() const {
    return pixels;
}

RGB8& rgb24_image::get_buffer() {
    return pixels;
}


}
