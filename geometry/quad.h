#ifndef QUAD_H
#define QUAD_H

#include "../renderer/rt.h"
#include "hittable.h"

using std::shared_ptr;

class quad : public hittable {
    public:
        quad() {}

        quad(vec3 _v0, vec3 _v1, vec3 _v2, vec3 _v3, shared_ptr<material> mat)
            : v0(_v0), v1(_v1), v2(_v2), v3(_v3), mp(mat) {};

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
        virtual bool bounding_box(aabb&) const override;

    public:
        vec3 v0, v1, v2, v3;
        shared_ptr<material> mp;
};

bool quad::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    //Source:
    //https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-quad/ray-quad-intersection-geometric-solution.html
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
    
    // check if the quad is behind the ray
    if (t < 0) return false; // the quad is behind

    // bounds check for small and large t
    if (t < t_min) return false;
    if (t > t_max) return false;
 
    // compute the intersection point using equation 1
    vec3 P = r.origin() + t * r.direction();
 
    // Step 2: inside-outside test
    vec3 C; // vector perpendicular to quad's plane
    
    int firstt = 1;
    int secondt = 1;
    // edge 0
    vec3 edge0 = v1 - v0; 
    vec3 vp0 = P - v0;
    C = cross(edge0, vp0);
    if (dot(N,C) < 0) firstt=0; // P is on the right side
 
    // edge 1
    vec3 edge1 = v2 - v1; 
    vec3 vp1 = P - v1;
    C = cross(edge1, vp1);
    if (dot(N,C) < 0)  firstt=0; // P is on the right side
 
    // edge 2
    vec3 edge2 = v0 - v2; 
    vec3 vp2 = P - v2;
    C = cross(edge2, vp2);
    if (dot(N,C) < 0) firstt=0; // P is on the right side;

    //second triangle

    // edge 0
    vec3 edge3 = v3 - v2; 
    vec3 vp3 = P - v2;
    C = cross(edge3, vp3);
    if (dot(N,C) < 0) secondt=0; // P is on the right side
    
    // edge 1
    vec3 edge4 = v0 - v3; 
    vec3 vp4 = P - v3;
    C = cross(edge4, vp4);
    if (dot(N,C) < 0)  secondt=0; // P is on the right side
 
    // edge 2
    vec3 edge5 = v2 - v0; 
    vec3 vp5 = P - v0;
    C = cross(edge5, vp5);
    if (dot(N,C) < 0) secondt=0; // P is on the right side;

    if(firstt==0 && secondt==0){
        return false;
    }
    rec.t = t;
    vec3 outward_normal = cross(v0v1, v0v2);
    rec.set_face_normal(r, outward_normal);
    rec.material_ptr = mp;
    rec.p = r.at(t);

    return true; // this ray hits thequad 
    
}

bool quad::bounding_box(aabb &output_box) const {
    point3 min, max;
    min = point3(
        dmin4(v0.x(), v1.x(), v2.x(), v3.x()),
        dmin4(v0.y(), v1.y(), v2.y(), v3.y()),
        dmin4(v0.z(), v1.z(), v2.z(), v3.z()));
    max = point3(
        dmax4(v0.x(), v1.x(), v2.x(), v3.x()),
        dmax4(v0.y(), v1.y(), v2.y(), v3.y()),
        dmax4(v0.z(), v1.z(), v2.z(), v3.z()));
    output_box = aabb(min, max);
    return true;
}

#endif
