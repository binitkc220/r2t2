#ifndef COLOR_H
#define COLOR_H

#include "../renderer/vec3.h"

#include<iostream>

void writeColor(std::ostream &out, color pixel_color, int samples_per_pixel){
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();
    
    // //Divide the color by number of samples and gamma correct for gamma 2.0
    auto scale = 1.0/samples_per_pixel;
    r = sqrt(r*scale);
    g = sqrt(g*scale);
    b = sqrt(b*scale);
    
    // Write translated [0,255] value of each color component
    out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
        << static_cast<int>(255 * clamp(g, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
}

#endif
