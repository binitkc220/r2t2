#ifndef RANDOM_SCENE_H_
#define RANDOM_SCENE_H_

#include "../geometry/bvh.h"
#include "../geometry/hittable_list.h"
#include "../geometry/parse_obj.h"
#include "../geometry/quad.h"
#include "../geometry/sphere.h"
#include "../geometry/triangle.h"

#include "../material/texture.h"
#include "../material/material.h"

hittable_list random_scene() {

    auto perlin = make_shared<noise_texture>();
    auto perlin_lambert = make_shared<lambertian>(perlin);
    auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));

    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left   = make_shared<dielectric>(1.5);
    auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);
    auto material_rec  = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);
    auto material2 = make_shared<lambertian>(color(0.52,0.14,0.19));

    hittable_list objs;
    hittable_list world;

    objs.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_center));
    objs.add(make_shared<sphere>(point3( 0.0,    0.0, -1.0),   0.5, material_center));
    objs.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));

    // world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),  -0.4, material_left));
    // world.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));

    std::string objFileLocation = "asset/obj/car.obj";
    std::string textureFileLocation = "asset/texture_images/car1.png";
    hittable_list obj_mesh = mesh(objFileLocation, textureFileLocation);

    auto difflight = make_shared<diffuse_light>(color(20.0 * (250.0/255.0),20 * (253.0/255.0),20*(15.0/255.0)));
    // obj_mesh.add(make_shared<sphere>(point3(1000, 50000, 1000), 10000, difflight));
    obj_mesh.add(make_shared<sphere>(point3(16.29, 0, 20.59), 1, difflight));


    auto difflight_2 = make_shared<diffuse_light>(color(4.0 * (250.0/255.0),4 * (253.0/255.0), 4*(15.0/255.0)));
    double temp_min = -1000.0, temp_max = 1000.0;
    for (int i = 0; i < 3; i++) {
        obj_mesh.add(make_shared<sphere>(
                         point3(random_double(temp_min, temp_max),
                                random_double(temp_min, temp_max),
                                random_double(temp_min, temp_max)), 0.005, difflight_2));
    }
    world.add(make_shared<bvh_node>(obj_mesh));

    return obj_mesh;
}
hittable_list final_scene() {
    auto ground = make_shared<lambertian>(color(0.48, 0.83, 0.53));

    hittable_list objects;

    objects.add(make_shared<sphere>(point3(0, -100000, 0), 100000.0, ground));

    auto light = make_shared<diffuse_light>(color(7, 7, 7));

    objects.add(make_shared<sphere>(point3(0, 0, 0), 1.0, light));
    objects.add(make_shared<sphere>(point3(800, 400, 100), 60, light));
    objects.add(make_shared<sphere>(point3(0, 38, 32), 1.0, light));
    objects.add(make_shared<sphere>(point3(34, 0, 34), 1.0, light));
    objects.add(make_shared<sphere>(point3(4, 6, 1), 1.0, light));
    auto center1 = point3(400, 400, 200);

    objects.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
    objects.add(make_shared<sphere>(
        point3(0, 150, 145), 50, make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)
    ));

    auto boundary = make_shared<sphere>(point3(360,150,145), 70, make_shared<dielectric>(1.5));
    objects.add(boundary);
    boundary = make_shared<sphere>(point3(0, 0, 0), 5000, make_shared<dielectric>(1.5));

    auto emat = make_shared<lambertian>(make_shared<image_texture>("asset/texture_images/earthmap.jpg"));
    objects.add(make_shared<sphere>(point3(400,200,400), 100, emat));
    objects.add(make_shared<sphere>(point3(-400,200,400), 100, emat));
    auto pertext = make_shared<noise_texture>(0.1);
    objects.add(make_shared<sphere>(point3(220,280,300), 80, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(point3(420,480,700), 80, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(point3(220,280,-300), 80, make_shared<lambertian>(pertext)));

    hittable_list boxes2;
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<sphere>(point3::random(0,165), 10, white));
    }

    return objects;
}
#endif // RANDOM_SCENE_H_
