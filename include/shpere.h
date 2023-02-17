#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable{
    public:
        sphere() {}
        sphere(point3 cen, double r, shared_ptr<material> m) : center(cen), radius(r), mat_ptr(m) {};
        
        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
        
    public:
        point3 center;
        double radius;
        shared_ptr<material> mat_ptr;
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
    
    rec.material_ptr = mat_ptr;
    
    return true;
}

#endif