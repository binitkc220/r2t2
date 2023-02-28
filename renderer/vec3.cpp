#include "vec3.h"

#include<iostream>
#include<cmath>

#include "rt.h"
#include "vec3.h"

vec3 random_in_unit_sphere() {
    while(true) {
        auto p = vec3::random(-1,1);
        if(p.lengthSquared() >= 1) continue;
        return p;
    }
}

vec3 random_in_hemisphere(const vec3& normal) {
    vec3 in_unit_sphere = random_in_unit_sphere();
    //In the same sphere as the normal
    return (dot(in_unit_sphere, normal) > 0.0) ? in_unit_sphere : -in_unit_sphere;
}

vec3 random_unit_vector() {
    return unitVector(random_in_unit_sphere());
}

vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2*dot(v,n)*n;
}

vec3 refract(const vec3& uv, const vec3& n, double eta_i_over_eta_t) {
    auto cos_theta = fmin(dot(-uv,n), 1.0);
    vec3 r_out_perp = eta_i_over_eta_t * (uv + cos_theta*n);
    vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.lengthSquared())) * n;
    return r_out_parallel + r_out_perp;
}

vec3 random_in_unit_disk() {
    while(true) {
        auto p = vec3(random_double(-1,1), random_double(-1,1), 0);
        if(p.lengthSquared() >= 1) continue;
        return p;
    }
}
