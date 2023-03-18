#ifndef RENDERER_H_
#define RENDERER_H_

#include "../material/material.h"

color rayColor(const ray& r, const hittable& world, int depth){
    hit_record rec;

    //If ray bounce limit is exceeded, no more light is gathered
    if(depth<=0)
        return color(0,0,0);

    if(world.hit(r,0.001,INF,rec)){
        ray scattered;
        color attenuation;
        if(rec.material_ptr->scatter(r,rec,attenuation,scattered))
            return attenuation*rayColor(scattered, world, depth-1);
        return color(0,0,0);
        // point3 target = rec.p + random_in_hemisphere(rec.normal);
        // return 0.5 * rayColor(ray(rec.p, target-rec.p), world, depth-1);
    }

    vec3 unit_direction = unitVector(r.direction());
    auto t=0.5*(unit_direction.y()+1.0);
    return (1.0-t)*color(1.0,1.0,1.0)+t*color(0.5,0.7,1.0);
}

#endif // RENDERER_H_
