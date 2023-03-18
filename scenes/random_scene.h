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
    // world.add(make_shared<quad>(vec3(20,0,-5),vec3(30,0,-5),vec3(30,30,-5),vec3(4,30,-5), material_center));
    // world.add(make_shared<triangle>(vec3(4,0,0),vec3(4,-4,0),vec3(0,0,10), material_center));
    // world.add(make_shared<triangle>(vec3(0,0,-10),vec3(0,0,10),vec3(4,0,0), material_center));
    std::string objFileLocation = "asset/obj/monkey.obj";
    std::vector<vec3> vertices;
    std::vector<std::vector<int>> triangle_face;
    std::vector<std::vector<int>> quad_face;
    readObjFile(objFileLocation, vertices, triangle_face, quad_face);
    for (const auto& face : quad_face) {
        objs.add(make_shared<quad>(vertices[face.at(0)-1],vertices[face.at(1)-1],vertices[face.at(2)-1],vertices[face.at(3)-1], perlin_lambert));
    }
    for (const auto& face : triangle_face) {
        objs.add(make_shared<triangle>(vertices[face.at(0)-1],vertices[face.at(1)-1],vertices[face.at(2)-1], perlin_lambert));
    }

    world.add(make_shared<bvh_node>(objs));

    std::cerr << "Found " << quad_face.size() << " quads, " << triangle_face.size() << " tris" << std::endl;

    vertices.clear();
    triangle_face.clear();
    quad_face.clear();
    // objFileLocation = "asset/obj/monkey.obj";
    // readObjFile(objFileLocation, vertices, triangle_face, quad_face);
    // for (const auto& face : quad_face) {
    //     world.add(make_shared<quad>(vertices[face.at(0)-1],vertices[face.at(1)-1],vertices[face.at(2)-1],vertices[face.at(3)-1], material2));
    // }
    // for (const auto& face : triangle_face) {
    //     world.add(make_shared<triangle>(vertices[face.at(0)-1],vertices[face.at(1)-1],vertices[face.at(2)-1], material2));
    // }
    return world;
}

#endif // RANDOM_SCENE_H_
