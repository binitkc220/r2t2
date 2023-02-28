#ifndef VEC3_H
#define VEC3_H

#include<iostream>
#include<cmath>

#include "rt.h"

//Vector3 Class
class vec3
{
    public:
        vec3(): p{0,0,0} {}
        vec3(double p0, double p1, double p2): p{p0,p1,p2} {}

        double x() const{return p[0];}
        double y() const{return p[1];}
        double z() const{return p[2];}

        vec3 operator - () const{return vec3(-p[0],-p[1],-p[2]);}
        double operator [] (int i) const{return p[i];}
        double& operator [] (int i) {return p[i];}

        vec3& operator += (const vec3 &v){
            p[0] += v.p[0];
            p[1] += v.p[1];
            p[2] += v.p[2];
            return *this;
        }

        vec3& operator *= (const double t){
            p[0] *= t;
            p[1] *= t;
            p[2] *= t;
            return *this;
            //or, return *this *=t;
        }

        vec3& operator /= (const double t){
            return *this *= 1/t; 
        }

        double length() const{
            return std::sqrt(lengthSquared());
        }

        double lengthSquared() const{
            return p[0]*p[0]+p[1]*p[1]+p[2]*p[2];
        }

        inline static vec3 random() {
            return vec3(random_double(),random_double(),random_double());
        }

        inline static vec3 random(double min, double max) {
            return vec3(random_double(min,max), random_double(min,max), random_double(min,max));
        }

        bool near_zero() const{
            //returns true if vector is close to zero in all dimensions
            const auto s = 1e-8;
            return (fabs(p[0])<s) && (fabs(p[1])<s) && (fabs(p[2])<s);
        }
    
    public:
        double p[3];
};

//aliases for vec3
using point3 = vec3;    //3d point
using color = vec3;     //RGB color


//Vector3 Utility Functions

inline std::ostream& operator << (std::ostream &out, const vec3 &v){
    return out << v.p[0] << ' ' << v.p[1] << ' ' << v.p[2];
}

inline vec3 operator + (const vec3 &a, const vec3 &b){
    return vec3(a.p[0]+b.p[0],a.p[1]+b.p[1],a.p[2]+b.p[2]);
}

inline vec3 operator - (const vec3 &a, const vec3 &b){
    return vec3(a.p[0]-b.p[0],a.p[1]-b.p[1],a.p[2]-b.p[2]);
}

inline vec3 operator * (const vec3 &a, const vec3 &b){
    return vec3(a.p[0]*b.p[0],a.p[1]*b.p[1],a.p[2]*b.p[2]);
}

inline vec3 operator * (double t, const vec3 &v){
    return vec3(t*v.p[0],t*v.p[1],t*v.p[2]);
}

inline vec3 operator * (const vec3 &v, double t){
    return t*v;
}

inline vec3 operator / (const vec3 &v, double t){
    return (1/t)*v;
}

inline double dot (const vec3 &a, const vec3 &b){
    return a.p[0]*b.p[0]+a.p[1]*b.p[1]+a.p[2]*b.p[2];
}

inline vec3 cross (const vec3 &a, const vec3 &b){
    return vec3(a.p[1] * b.p[2] - a.p[2] * b.p[1],
                a.p[2] * b.p[0] - a.p[0] * b.p[2],
                a.p[0] * b.p[1] - a.p[1] * b.p[0]);
}

inline vec3 unitVector(vec3 v){
    return v/v.length();
}

vec3 random_in_unit_sphere();
vec3 random_in_hemisphere(const vec3& normal);
vec3 random_unit_vector();
vec3 reflect(const vec3& v, const vec3& n);
vec3 refract(const vec3& uv, const vec3& n, double eta_i_over_eta_t);
vec3 random_in_unit_disk();

#endif
