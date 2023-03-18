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
#include "renderer/renderer.h"

#include "external/imgui/imgui.h"
#include "external/imgui/backends/imgui_impl_sdl2.h"
#include "external/imgui/backends/imgui_impl_sdlrenderer.h"

#include "scenes/two_perlin_spheres.h"
#include "scenes/random_scene.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "external/tiny_obj_loader.h"

#include <SDL2/SDL.h>
#include <SDL_pixels.h>
#include <SDL_render.h>
#include <SDL_video.h>

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <sstream>


const int SCREEN_HEIGHT = 720;
const int SCREEN_WIDTH = 1280;
const double aspect_ratio = 3.0 / 2.0;
const int img_width = 200;
const int img_height = static_cast<int>(img_width/aspect_ratio);


int main(int argc, char** argv)
{
    //Image
    static int samples_per_pixel = 100;
    static int max_depth = 16;

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

    color *a;
    a = (color*)malloc(sizeof(color)*img_height*img_width);

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
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_s)
                    cam.rotate(0.04, 0, 0);
                if (event.key.keysym.sym == SDLK_w)
                    cam.rotate(-0.04, 0, 0);
                if (event.key.keysym.sym == SDLK_a)
                    cam.rotate(0, 0.04, 0);
                if (event.key.keysym.sym == SDLK_d)
                    cam.rotate(0, -0.04, 0);
                if (event.key.keysym.sym == SDLK_UP)
                    cam.dolly(-1);
                if (event.key.keysym.sym == SDLK_DOWN)
                    cam.dolly(1);
                if (event.key.keysym.sym == SDLK_LEFT)
                    cam.truck(-1);
                if (event.key.keysym.sym == SDLK_RIGHT)
                    cam.truck(1);
            }
        }

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

        ImGui::Text("Realtime raytracing");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

        ImGui::SliderFloat("pwaal", &pwaal, 0.0001f, 3.0f);
        ImGui::SliderFloat("d2f", &d2f, 0.0001f, 1.0f);

        ImGui::SliderInt("spp", &samples_per_pixel, 1, 32);
        ImGui::SliderInt("depth", &max_depth, 1, 16);

        ImGui::End();

        ImGui::Render();

        cam.set_camera_intrinsics(d2f, pwaal);

        SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
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
