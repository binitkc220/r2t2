#include "SDL_events.h"
#include "SDL_keyboard.h"
#include "SDL_keycode.h"
#include "geometry/bvh.h"
#include "geometry/hittable_list.h"
#include "geometry/quad.h"
#include "geometry/parse_obj.h"
#include "geometry/rectangle.h"
#include "geometry/sphere.h"
#include "geometry/triangle.h"

#include "material/material.h"
#include "material/color.h"
#include "material/texture.h"

#include "renderer/camera.h"
#include "renderer/rt.h"

#include "external/imgui/imgui.h"
#include "external/imgui/backends/imgui_impl_sdl2.h"
#include "external/imgui/backends/imgui_impl_sdlrenderer.h"

#include <SDL2/SDL.h>
#include <SDL_pixels.h>
#include <SDL_render.h>
#include <SDL_video.h>

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <sstream>

using std::make_shared;

const int SCREEN_HEIGHT = 720;
const int SCREEN_WIDTH = 1280;
const ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
const double aspect_ratio = 3.0 / 2.0;
const int img_width = 200;
const int img_height = static_cast<int>(img_width/aspect_ratio);
const double vfov = 20.0;

hittable_list two_perlin_spheres() {
    hittable_list objects;

    auto perlin_texture = make_shared<noise_texture>(4);
    objects.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(perlin_texture)));
    objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(perlin_texture)));

    return objects;
}

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
    hittable_list mesh_objs = mesh(objFileLocation, "");
    // readObjFile(objFileLocation, vertices, triangle_face, quad_face);
    for (const auto& face : quad_face) {
        objs.add(make_shared<quad>(vertices[face.at(0)-1],vertices[face.at(1)-1],vertices[face.at(2)-1],vertices[face.at(3)-1], perlin_lambert));
    }
    for (const auto& face : triangle_face) {
        objs.add(make_shared<triangle>(vertices[face.at(0)-1],vertices[face.at(1)-1],vertices[face.at(2)-1], perlin_lambert));
    }

    world.add(make_shared<bvh_node>(mesh_objs));

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

int main(int argc, char** argv)
{
    //Image
    int samples_per_pixel = 100;
    int max_depth = 16;

    if (argc == 3) {
        samples_per_pixel = atoi(argv[1]);
        max_depth = atoi(argv[2]);
    }

    std::cerr << "Running with " << samples_per_pixel << " samples and depth " << max_depth << std::endl;

    //World
    auto world = random_scene();

    //Camera 30
    point3 lookfrom(7, 2, 3);
    point3 lookat(0,0,0);
    vec3 vup(0,1,0);
    camera cam(lookfrom, lookat);

    static float d2f=10.0f, pwaal=0.1f;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        return -1;
    }

    SDL_WindowFlags window_flags = (SDL_WindowFlags) (SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("R2T2",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          window_flags);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
                                                SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

    if (renderer == NULL)
        return -1;


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;


    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer_Init(renderer);

    SDL_Texture* texture;
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, img_width, img_height);

    uint32_t *pixels;

    //Store color information into array 'a' and then write those pixels after processing
    color *a;
    a=(color*)malloc(sizeof(color)*img_height*img_width);

    //Render

    std::cout << "P3\n" << img_width << " " << img_height << "\n255\n";

    bool done = false;

    while (!done) {

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_w)
                done = true;
        }

        cam = camera(lookfrom, lookat, vup, vfov, aspect_ratio, pwaal, d2f);

        for (int j = img_height-1; j >= 0; --j) {
            std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
            for (int i = 0; i < img_width; ++i) {
                color pixel_color(0, 0, 0);
                for (int s = 0; s < samples_per_pixel; ++s) {
                    auto u = (i + random_double()) / (img_width-1);
                    auto v = (j + random_double()) / (img_height-1);
                    ray r = cam.get_ray(u, v);
                    pixel_color += rayColor(r, world, max_depth);
                }
                a[i*img_height+j] = pixel_color/samples_per_pixel;
            }
        }

        int pitch = img_width * 3;
        uint32_t format;
        SDL_QueryTexture(texture, &format, NULL, NULL, NULL);
        SDL_LockTexture(texture, NULL, (void**) &pixels, &pitch);

        SDL_PixelFormat *pixelFormat = SDL_AllocFormat(format);

        for (int j = 0; j < img_height; ++j) {
            for (int i = 0; i < img_width; ++i) {
                uint8_t red   = static_cast<uint8_t>(256.0 * clamp(a[i*img_height+j].x(), 0.0, 0.999));
                uint8_t green = static_cast<uint8_t>(256.0 * clamp(a[i*img_height+j].y(), 0.0, 0.999));
                uint8_t blue  = static_cast<uint8_t>(256.0 * clamp(a[i*img_height+j].y(), 0.0, 0.999));
                j = img_height - j - 1;
                pixels[j * img_width + i] = SDL_MapRGB(pixelFormat, red, green, blue);
            }
        }

        SDL_UnlockTexture(texture);

        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Renderer Window");

        ImGui::Text("This is some text");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

        ImGui::SliderFloat("pwaal", &pwaal, 0.0001f, 3.0f);
        ImGui::SliderFloat("d2f", &d2f, 0.0001f, 1.0f);

        ImGui::End();

        ImGui::Render();
        SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        SDL_SetRenderDrawColor(renderer,
                               (uint8_t)(clear_color.x*255),
                               (uint8_t)(clear_color.y*255),
                               (uint8_t)(clear_color.z*255),
                               (uint8_t)(clear_color.w*255));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(renderer);
    }

    for (int j = img_height-1; j >= 0; --j) {
        for (int i = 0; i < img_width; ++i) {
            writeColor(std::cout, a[i*img_height+j], samples_per_pixel);
        }
    }
    std::cerr << std::endl;

    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(texture);

    SDL_Quit();

    return 0;

}
