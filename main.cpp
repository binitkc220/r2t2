#include "material/texture.h"
#include "renderer/rt.h"
#include "material/color.h"
#include "geometry/hittable_list.h"
#include "geometry/sphere.h"
#include "renderer/camera.h"
#include "renderer/rt.h"
#include "material/material.h"
#include "geometry/bvh.h"
#include "geometry/rectangle.h"
#include "geometry/triangle.h"
#include "geometry/quad.h"
#include "geometry/parse.h"
#include <cstdlib>
#include <iostream>
#include <memory>
#include <sstream>

using std::make_shared;

hittable_list two_perlin_spheres() {
    hittable_list objects;

    auto perlin_texture = make_shared<noise_texture>(4);
    objects.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(perlin_texture)));
    objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(perlin_texture)));

    return objects;
}

hittable_list scene_with_light() {
    hittable_list objs, world;
    auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto earth_texture = make_shared<image_texture>("asset/texture_images/earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    objs.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_center));
    objs.add(make_shared<sphere>(point3(0,1.0,0), 1, earth_surface));

    auto difflight = make_shared<diffuse_light>(color(7,7,7));
    objs.add(make_shared<sphere>(point3(2.0,3.0,1.0),0.8, difflight));

    world.add(make_shared<bvh_node>(objs));

    return world;
}

// hittable_list mesh() {
//     hittable_list objs, world;

//     auto perlin = make_shared<noise_texture>();
//     auto perlin_lambert = make_shared<lambertian>(perlin);
//     auto material = make_shared<lambertian>(color(1,0,0));

//     auto image = make_shared<image_texture>("asset/texture_images/crate.jpg");
//     auto car = make_shared<lambertian>(image);

//     std::string objFileLocation = "asset/obj/crate.obj";


//     std::vector<vec3> vertices;
//     std::vector<vec2> uv_points;
//     std::vector<std::vector<int>> triangle_face;
//     std::vector<std::vector<int>> quad_face;
//     readObjFile(objFileLocation, vertices, triangle_face, quad_face, uv_points);
//     // for (const auto& face : quad_face) {
//     //     objs.add(make_shared<quad>(
//     //         vertices[face.at(0)-1],
//     //         vertices[face.at(1)-1],
//     //         vertices[face.at(2)-1],
//     //         vertices[face.at(3)-1], 
//     //         perlin_lambert
//     //         ));
//     // }
//     for (const auto& face : triangle_face) {
//         objs.add(make_shared<triangle>(
//             vertices[face.at(0)-1],
//             vertices[face.at(1)-1],
//             vertices[face.at(2)-1],
//             uv_points[face.at(0)-1],
//             uv_points[face.at(1)-1],
//             uv_points[face.at(2)-1], 
//             perlin_lambert));
//     }

//     world.add(make_shared<bvh_node>(objs));
    
//     std::cerr << "Found " << quad_face.size() << " quads, " << triangle_face.size() << " tris" << std::endl;

//     vertices.clear();
//     triangle_face.clear();
//     quad_face.clear();

//     return world;
// }

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
    auto earth_texture = make_shared<image_texture>("asset/texture_images/earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);

    hittable_list objs;
    hittable_list world;

    objs.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_center));
    // objs.add(make_shared<sphere>(point3( 0.0,    0.0, -1.0),   0.5, material_center));
    // objs.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    objs.add(make_shared<sphere>(point3(0,1.0,0), 1, earth_surface));
    // world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),  -0.4, material_left));
    // world.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));
    // world.add(make_shared<quad>(vec3(20,0,-5),vec3(30,0,-5),vec3(30,30,-5),vec3(4,30,-5), material_center));
    // world.add(make_shared<triangle>(vec3(4,0,0),vec3(4,-4,0),vec3(0,0,10), material_center));
    // world.add(make_shared<triangle>(vec3(0,0,-10),vec3(0,0,10),vec3(4,0,0), material_center));
    
    world.add(make_shared<bvh_node>(objs));

    return world;
}


color rayColor(const ray& r, const color& background, const hittable& world, int depth){
    hit_record rec;

    //If ray bounce limit is exceeded, no more light is gathered
    if(depth<=0)
        return color(0,0,0);
    
    if(!world.hit(r,0.001,INF,rec)){
        return background;
    }
    ray scattered;
    color attenuation;
    color emitted = rec.material_ptr->emitted(rec.u,rec.v,rec.p);

    if(!rec.material_ptr->scatter(r,rec,attenuation,scattered))
        return emitted;

    return emitted + attenuation * rayColor(scattered, background, world, depth-1);
}

int main(int argc, char** argv)
{
    //Image
    const auto aspect_ratio = 3.0 / 2.0;
    const int img_width = 200;
    const int img_height = static_cast<int>(img_width/aspect_ratio);
    double vfov = 20.0;
    int samples_per_pixel = 10;
    int max_depth = 4;

    if (argc == 3) {
        samples_per_pixel = atoi(argv[1]);
        max_depth = atoi(argv[2]);
    }

    std::cerr << "Running with " << samples_per_pixel << " samples and depth " << max_depth << std::endl;

    //World
    auto world = mesh("asset/obj/car.obj","asset/texture_images/crate.jpg");
    // auto world = random_scene();

    //Camera 30
    point3 lookfrom(7, 4, 10);
    point3 lookat(0,0,0);
    vec3 vup(0,1,0);
    auto dist_to_focus = (lookfrom-lookat).length();
    dist_to_focus = 10.0;
    auto aperture = 0.1;
    color background(1,1,1);

    camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus);
    

    //Store color information into array 'a' and then write those pixels after processing
    color *a;
    a=(color*)malloc(sizeof(color)*img_height*img_width);

    //Render

    std::cout << "P3\n" << img_width << " " << img_height << "\n255\n";

    
    for (int j = img_height-1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < img_width; ++i) {
            color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (img_width-1);
                auto v = (j + random_double()) / (img_height-1);
                ray r = cam.get_ray(u, v);
                pixel_color += rayColor(r, background, world, max_depth);
            }
            a[i*img_height+j] = pixel_color;  
        }
    }

    for (int j = img_height-1; j >= 0; --j) {
        for (int i = 0; i < img_width; ++i) {
            writeColor(std::cout, a[i*img_height+j], samples_per_pixel);
        }
    }
    std::cerr << std::endl;

}
