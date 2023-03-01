#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <memory>

#include "../renderer/rt.h"
#include "aabb.h"
#include "hittable.h"

using std::shared_ptr;

class triangle : public hittable {
    public:
        triangle() {}

        triangle(vec3 _v0, vec3 _v1, vec3 _v2, shared_ptr<material> mat)
            : v0(_v0), v1(_v1), v2(_v2), mp(mat) {};

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
        virtual bool bounding_box(aabb&) const override;

    public:
        vec3 v0, v1, v2;
        shared_ptr<material> mp;
};

bool triangle::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    //Source:
    //https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution.html
    // compute the plane's normal
    vec3 v0v1 = v1 - v0;
    vec3 v0v2 = v2 - v0;
    // no need to normalize
    vec3 N = cross(v0v1, v0v2); // N

    // Step 1: finding P
    
    // check if the ray and plane are parallel.
    double NdotRayDirection = dot(N,r.direction());
    if (fabs(NdotRayDirection) < 0.0000001) // almost 0
        return false; // they are parallel, so they don't intersect! 

    // compute d parameter using equation 2
    double d = dot(v0,-N);
    
    // compute t (equation 3)
    double t = -(dot(N,r.origin()) + d) / NdotRayDirection;
    // double t = dot(N,r.origin()) + d;
    
    // check if the triangle is behind the ray
    if (t < 0) return false; // the triangle is behind

    // bounds check for small and large t
    if (t < t_min) return false;
    if (t > t_max) return false;
 
    // compute the intersection point using equation 1
    vec3 P = r.origin() + t * r.direction();
 
    // Step 2: inside-outside test
    vec3 C; // vector perpendicular to triangle's plane
 
    // edge 0
    vec3 edge0 = v1 - v0; 
    vec3 vp0 = P - v0;
    C = cross(edge0, vp0);
    if (dot(N,C) < 0) return false; // P is on the right side
 
    // edge 1
    vec3 edge1 = v2 - v1; 
    vec3 vp1 = P - v1;
    C = cross(edge1, vp1);
    if (dot(N,C) < 0)  return false; // P is on the right side
 
    // edge 2
    vec3 edge2 = v0 - v2; 
    vec3 vp2 = P - v2;
    C = cross(edge2, vp2);
    if (dot(N,C) < 0) return false; // P is on the right side;

    rec.t = t;
    vec3 outward_normal = cross(v0v1, v0v2);
    outward_normal = outward_normal / outward_normal.length();
    rec.set_face_normal(r, outward_normal);
    rec.material_ptr = mp;
    rec.p = r.at(t);

    return true; // this ray hits the triangle
    
}

bool triangle::bounding_box(aabb& output_box) const {
    point3 min, max;
    min = point3(
        dmin3(v0.x(), v1.x(), v2.x()),
        dmin3(v0.y(), v1.y(), v2.y()),
        dmin3(v0.z(), v1.z(), v2.z()));
    max = point3(
        dmax3(v0.x(), v1.x(), v2.x()),
        dmax3(v0.y(), v1.y(), v2.y()),
        dmax3(v0.z(), v1.z(), v2.z()));
    output_box = aabb(min, max);
    return true;
}

#endif
