#ifndef RENDERER_H_
#define RENDERER_H_

#include "../material/material.h"

color rayColor(const ray& r, const color& background, const hittable& world, int depth){
    hit_record rec;

    //If ray bounce limit is exceeded, no more light is gathered
    if(depth<=0)
        return color(0,0,0);

        // If the ray hits nothing, return the background color.
    if (!world.hit(r, 0.001, INF, rec))
        return background;

    ray scattered;
    color attenuation;
    color emitted = rec.material_ptr->emitted(rec.u, rec.v, rec.p);

    if (!rec.material_ptr->scatter(r, rec, attenuation, scattered))
        return emitted;

    return emitted + attenuation * rayColor(scattered, background, world, depth-1);
}

#endif // RENDERER_H_
