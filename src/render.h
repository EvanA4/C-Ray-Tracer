#ifndef RENDER_H
#define RENDER_H

#include "tga.h"


typedef struct Renderer {
    float pos[3];
    float dir[3];
    float fov;
    int width;
    int height;
} Renderer;


Pixel render(Renderer *rptr, int px);


#endif