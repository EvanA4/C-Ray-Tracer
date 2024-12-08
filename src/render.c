#include "tga.h"
#include "render.h"


Pixel render(Renderer *rptr, int px) {
    float row01 = px / rptr->width / (float) rptr->height;
    float col01 = px % rptr->width / (float) rptr->width;

    Pixel out;
    out.r = (unsigned char) 255 * row01;
    out.g = (unsigned char) 255 * col01;
    out.b = (unsigned char) 0;
    return out;
}