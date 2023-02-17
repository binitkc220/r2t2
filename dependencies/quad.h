#ifndef QUAD_H
#define QUAD_H

#include "rt.h"

#include "hittable.h"

class quad : public hittable {
    public:
        quad() {}

        quad(vec3 _v0, vec3 _v1, vec3 _v2, vec3 _v3, shared_ptr<material> mat)
            : v0(_v0), v1(_v1), v2(_v2), v3(_v3), mp(mat) {};

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

    public:
        shared_ptr<material> mp;
        vec3 v0, v1, v2, v3;
};

bool quad::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    //Source:
    //https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-quad/ray-quad-intersection-geometric-solution.html
    // compute the plane's normal
    vec3 v0v1 = v1 - v0;
    vec3 v0v2 = v2 - v0;
    // no need to normalize
    vec3 N = cross(v0v1, v0v2); // N
    double area2 = N.length();
 
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
    auto outward_normal = vec3(0, 0, 1);
    rec.set_face_normal(r, outward_normal);
    rec.material_ptr = mp;
    rec.p = r.at(t);

    return true; // this ray hits thequad 
    
}

#endif