#include <math.h>
#include <stdlib.h>
#include "tga.h"
#include "render.h"

#define PI 3.1415926535F


typedef struct Ray {
    Vec3 pos;
    Vec3 dir;
} Ray;


// helper functions for handling vectors and matrices
static float vlen(Vec3 vec) {
    return sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
}

static void vnorm(Vec3 vec) {
    float len = vlen(vec);
    for (int i = 0; i < 3; ++i) vec[i] = vec[i] / len;
}

static void vcross(Vec3 a, Vec3 b, Vec3 dest) {
    dest[0] = a[1] * b[2] - a[2] * b[1];
    dest[1] = a[2] * b[0] - a[0] * b[2];
    dest[2] = a[0] * b[1] - a[1] * b[0];
}

static void mulM4V4(Mat4 mat, Vec4 vec, Vec4 dest) {
    for (int i = 0; i < 4; ++i) {
        // for each entry in destination...
        dest[i] = 0.0F;

        for (int j = 0; j < 4; ++j) {
            // add each operation
            dest[i] += mat[i][j] * vec[j];
        }
    }
}

static void subV3(Vec3 a, Vec3 b, Vec3 dest) {
    for (int i = 0; i < 3; ++i) dest[i] = a[i] - b[i];
}


// actual rendering functions
static Ray *create_ray(Renderer *rptr, int px) {
    // thanks to https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-generating-camera-rays/generating-camera-rays.html
    // also thanks http://www.codinglabs.net/article_world_view_projection_matrix.aspx

    // determine screen space of pixel
    float row01 = px / rptr->width / (float) rptr->height;
    float col01 = px % rptr->width / (float) rptr->width;
    row01 += .5F / rptr->width;
    col01 += .5F / rptr->height;
    float scrnx = 2 * row01 - 1;
    float scrny = 1 - 2 * col01;
    // printf("Screen Space: %.2f, %.2f\n", scrnx, scrny);

    // determine camera space of pixel
    float aspect = rptr->width / (float) rptr->height;
    float tana = tan(rptr->fov / 360.0F * PI);
    float camx = scrnx * aspect * tana;
    float camy = scrny * tana;
    // printf("Camera Space: %.2f, %.2f\n", camx, camy);

    Vec4 camSpace = {camx, camy, -1, 1};

    // get direction of ray
    Ray *out = malloc(sizeof(Ray));
    Vec4 worldSpace4 = {0.0F, 0.0F, 0.0F, 0.0F};
    mulM4V4(rptr->view, camSpace, worldSpace4);
    Vec3 worldSpace3 = {0.0F, 0.0F, 0.0F};
    for (int i = 0; i < 3; ++i) {
        out->pos[i] = rptr->pos[i];
        worldSpace3[i] = worldSpace4[i];
    }
    // printf("World Space: %.2f, %.2f, %.2f\n", worldSpace3[0], worldSpace3[1], worldSpace3[2]);

    Vec3 direction = {0.0F, 0.0F, 0.0F};
    subV3(worldSpace3, out->pos, direction);
    vnorm(direction);

    for (int i = 0; i < 3; ++i) out->dir[i] = direction[i];

    return out;
}


static Pixel uvpx(Renderer *rptr, int px) {
    float row01 = px / rptr->width / (float) rptr->height;
    float col01 = px % rptr->width / (float) rptr->width;

    Pixel out;
    out.r = (unsigned char) 255 * row01;
    out.g = (unsigned char) 255 * col01;
    out.b = (unsigned char) 0;
    return out;
}


Renderer *render_init(KerrArgs *args) {
    Renderer *out = malloc(sizeof(Renderer));
    for (int i = 0; i < 3; ++i) {
        out->pos[i] = args->pos[i];
        out->dir[i] = args->dir[i];
    }
    out->fov = args->fov;
    out->width = args->width;
    out->height = args->height;

    // compute view matrix
    //     thanks to https://www.3dgep.com/understanding-the-view-matrix/
    Vec3 forward = {args->dir[0], args->dir[1], args->dir[2]};
    Vec3 position = {args->pos[0], args->pos[1], args->pos[2]};
    for (int i = 0; i < 3; ++i) out->view[i][2] = -forward[i];

    //     cross product between cam's forward (dir) and true Y => cam's right vector
    Vec3 right = {0.0F, 0.0F, 0.0F};
    Vec3 trueY = {0.0F, 1.0F, 0.0F};
    vcross(forward, trueY, right);
    for (int i = 0; i < 3; ++i) out->view[i][0] = -right[i];

    //     cross product between cam's right and cam's forward => cam's up vector
    Vec3 up = {0.0F, 0.0F, 0.0F};
    vcross(right, forward, up);
    for (int i = 0; i < 3; ++i) out->view[i][1] = -up[i];

    //     set position of camera
    out->view[3][3] = 1.0F;
    for (int i = 0; i < 3; ++i) out->view[i][3] = position[i];

    return out;
}


Pixel render_far(Ray *cur) {
    Vec3 finalPos = {0.0F, 0.0F, 0.0F};
    for (int i = 0; i < 3; ++i) {
        finalPos[i] = cur->pos[i] + cur->dir[i] * 10000;
        if (finalPos[i] > 1.0F) finalPos[i] = 1.0F;
        else if (finalPos[i] < 0.0F) finalPos[i] = 0.0F;
    }
    Pixel out;
    out.r = (unsigned char) 255 * finalPos[0];
    out.g = (unsigned char) 255 * finalPos[1];
    out.b = (unsigned char) 255 * finalPos[2];
    return out;
}


Pixel render_dir(Ray *cur) {
    for (int i = 0; i < 3; ++i) {
        if (cur->dir[i] > 1.0F) cur->dir[i] = 1.0F;
        else if (cur->dir[i] < 0.0F) cur->dir[i] = 0.0F;
    }
    Pixel out;
    out.r = (unsigned char) 255 * cur->dir[0];
    out.g = (unsigned char) 255 * cur->dir[1];
    out.b = (unsigned char) 255 * cur->dir[2];
    return out;
}


typedef float Vec2[2];
float dotV3(Vec3 a, Vec3 b) {
    float out = 0.0F;
    for (int i = 0; i < 3; ++i) out += a[i] * b[i];
    return out;
}
void pierce_atm(Ray *cur, Vec2 dest) {
    // returns (distance to sphere, length of path through sphere)
    // credit to Sebastian Lague at https://www.youtube.com/watch?v=DxfEbulyFcY&t=154s
    Vec3 spherePos = {0.0F, 0.0F, 0.0F};
    float sphereR = 2.0F;

    // solve parameterized equation for sphere collision https://viclw17.github.io/2018/07/16/raytracing-ray-sphere-intersection
    Vec3 originDiff = {0.0F, 0.0F, 0.0F};
    subV3(cur->pos, spherePos, originDiff);
    // printf("origin diff: %.2f %.2f %.2f\n", originDiff[0], originDiff[1], originDiff[2]);
    float b = 2. * dotV3(cur->dir, originDiff);
    float c = dotV3(originDiff, originDiff) - sphereR * sphereR;
    float disc = b * b - 4. * c;

    if (disc > 0.0F) {
        // printf("Heyo collision\n");
        float sqrtDisc = sqrt(disc);
        float distNearIntersect = ((-b - sqrtDisc) / 2.0F) > 0.0F ? ((-b - sqrtDisc) / 2.0F) : 0.0F; // max for in case camera is inside sphere
        float distFarIntersect = (-b + sqrtDisc) / 2.0F;
        float rawAtmDepth = distFarIntersect - distNearIntersect;

        if (distFarIntersect >= 0.0F) {
            dest[0] = distNearIntersect;
            dest[1] = rawAtmDepth;
            return;
        }
    }
    
    dest[0] = 1000000.0F;
    dest[1] = 0.0F;
}
Pixel render_sphere(Ray *cur) {
    Vec2 info = {0.0F, 0.0F};
    pierce_atm(cur, info);
    info[1] /= 4.0F;
    if (info[1] < 0.0F) info[1] = 0.0F;
    else if (info[1] > 1.0F) info[1] = 1.0F;

    Pixel out;
    out.r = (unsigned char) 255 * info[1];
    out.g = (unsigned char) 255 * info[1];
    out.b = (unsigned char) 255 * info[1];
    return out;
}


Pixel render(Renderer *rptr, int px) {
    Ray *cur = create_ray(rptr, px);
    // printf(
    //     "[%d] Position: {%2.f, %.2f, %.2f}            Direction: {%2.f, %.2f, %.2f}\n",
    //     px, cur->pos[0], cur->pos[1], cur->pos[2], cur->dir[0], cur->dir[1], cur->dir[2]
    // );
    // if (px == 12) {
    Pixel out = render_sphere(cur);
    free(cur);
    return out;
    // } else {
    //     return uvpx(rptr, px);
    // }
}