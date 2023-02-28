#ifndef HITTABLE_H
#define HITTABLE_H

#include "../renderer/ray.h"
#include "../renderer/rt.h"
#include "aabb.h"

using std::shared_ptr;

class material;

struct hit_record {
    point3 p;
    vec3 normal;
    shared_ptr<material> material_ptr;
    double t;
    bool front_face;
    double u,v;

    inline void set_face_normal(const ray& r, const vec3& outward_normal){
        front_face = dot(r.direction(),outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable{
    public:
        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
        virtual bool bounding_box(aabb&) const = 0;
};

#endif
