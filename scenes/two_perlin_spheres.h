#ifndef TWO_PERLIN_SPHERES_H_
#define TWO_PERLIN_SPHERES_H_

#include "../geometry/hittable_list.h"
#include "../geometry/sphere.h"
#include "../material/texture.h"
#include "../material/material.h"


hittable_list two_perlin_spheres() {
    hittable_list objects;

    auto perlin_texture = make_shared<noise_texture>(4);
    objects.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(perlin_texture)));
    objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(perlin_texture)));

    return objects;
}

#endif // TWO_PERLIN_SPHERES_H_
