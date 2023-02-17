#include "rt.h"
#include "color.h"
#include "hittable_list.h"
#include "shpere.h"
#include "camera.h"
#include "material.h"
#include "rectangle.h"
#include "triangle.h"
#include "quad.h"
#include<iostream>

#ifndef x_pos
#define x_pos 2
#endif

hittable_list random_scene() {
    hittable_list world;

    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left   = make_shared<dielectric>(1.5);
    auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);
    auto material_rec  = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

    // world.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    // world.add(make_shared<sphere>(point3( 0.0,    0.0, -1.0),   0.5, material_center));
    // world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    // world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),  -0.4, material_left));
    // world.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));
    world.add(make_shared<quad>(vec3(20,0,-5),vec3(30,0,-5),vec3(30,30,-5),vec3(4,30,-5), material_center));
    // world.add(make_shared<triangle>(vec3(4,0,0),vec3(4,-4,0),vec3(0,0,10), material_center));
    // world.add(make_shared<triangle>(vec3(0,0,-10),vec3(0,0,10),vec3(4,0,0), material_center));
    
    return world;
}


color rayColor(const ray& r, const hittable& world, int depth){
    hit_record rec;

    //If ray bounce limit is exceeded, no more light is gathered
    if(depth<=0)
        return color(0,0,0);
    
    if(world.hit(r,0.001,infinity,rec)){
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

int main()
{
    //Image
    const auto aspect_ratio = 3.0 / 2.0;
    const int img_width = 400;
    const int img_height = static_cast<int>(img_width/aspect_ratio);
    const int samples_per_pixel = 50;
    const int max_depth = 50;
    

    //World
    auto world = random_scene();

    //Camera
    point3 lookfrom(-20,-30,-100);
    point3 lookat(4,0,0);
    vec3 vup(0,1,0);
    auto dist_to_focus = (lookfrom-lookat).length();
    auto aperture = 1.0;
    camera cam(lookfrom, lookat, vup, 30, aspect_ratio, aperture, dist_to_focus);
    

    //Store color information into array 'a' and then write those pixels after processing
    color *a;
    a=(color*)malloc(sizeof(color)*img_height*img_width);

    //Render

    std::cout << "P3\n" << img_width << " " << img_height << "\n255\n";

    
    for (int j = img_height-1; j >= 0; --j) {
        // #pragma omp parallel
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < img_width; ++i) {
            color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (img_width-1);
                auto v = (j + random_double()) / (img_height-1);
                ray r = cam.get_ray(u, v);
                pixel_color += rayColor(r, world, max_depth);
            }
            a[i*img_height+j] = pixel_color;  
        }
    }

    for (int j = img_height-1; j >= 0; --j) {
        for (int i = 0; i < img_width; ++i) {
            writeColor(std::cout, a[i*img_height+j], samples_per_pixel);
        }
    }

}