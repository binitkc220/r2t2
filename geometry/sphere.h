#ifndef SPHERE_H
#define SPHERE_H

#include "aabb.h"
#include "hittable.h"
#include "../renderer/vec3.h"

using std::shared_ptr;

class sphere : public hittable{
    public:
        sphere() {}
        sphere(point3 cen, double r, shared_ptr<material> m) : center(cen), radius(r), mat_ptr(m) {};
        
        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
        virtual bool bounding_box(aabb&) const override;
        
    public:
        point3 center;
        double radius;
        shared_ptr<material> mat_ptr;

    private:
        static void get_sphere_uv(const point3& p, double& u, double& v) {
            auto theta = acos(-p.y());
            auto phi = atan2(-p.z(), p.x()) + PI;

            u = phi / (2 * PI);
            v = theta / PI;
        }
};

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const{
    vec3 oc = r.origin() - center;
    auto a = r.direction().lengthSquared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.lengthSquared() - radius*radius;

    auto discriminant = half_b*half_b - a*c;
    if(discriminant<0) return false;
    auto sqrtdisc = sqrt(discriminant);

    //Find the nearest root that lies in the acceptable range
    auto root = (-half_b-sqrtdisc) / a;
    if(root<t_min || t_max<root){
        root = (-half_b+sqrtdisc) / a;
        if(root<t_min || t_max<root)
            return false;
    }
    
    rec.t = root;
    rec.p = r.at(rec.t);
    rec.normal = (rec.p - center) / radius;

    vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r,outward_normal);

    get_sphere_uv(outward_normal, rec.u, rec.v);
    
    rec.material_ptr = mat_ptr;
    
    return true;
}

bool sphere::bounding_box(aabb &output_box) const {
    output_box = aabb(
        center - vec3(radius, radius, radius),
        center + vec3(radius, radius, radius));
    return true;
}

#endif
