#ifndef CAMERA_H
#define CAMERA_H

#include "rt.h"
#include "vec3.h"
#include "ray.h"

class camera {
    public:
        //vfov = vertical field-of-view in degrees
        camera(point3 lookfrom, point3 lookat, vec3 vup = vec3(0,1,0),
               double _vfov=20.0, double _aspect_ratio=3.0/2.0,
               double aperture=0.1, double focus_dist=10.0){
            vfov = _vfov;
            aspect_ratio = _aspect_ratio;
            auto theta = degree_to_radian(vfov);
            auto h = tan(theta/2);

            auto viewport_height = 2.0 * h;
            auto viewport_width = aspect_ratio * viewport_height;

            w = unitVector(lookfrom - lookat);
            u = unitVector(cross(vup, w));
            v = cross(w,u);

            origin = lookfrom;
            horizontal = focus_dist * viewport_width * u;
            vertical = focus_dist * viewport_height * v;
            lower_left_corner = origin - horizontal/2 - vertical/2 - focus_dist * w;

            this->focus_dist = focus_dist;

            lens_radius = aperture/2;
        }

        void set_camera_intrinsics(double _focus_dist, double _aperture) {
            auto theta = degree_to_radian(vfov);
            auto h = tan(theta/2);

            auto viewport_height = 2.0 * h;
            auto viewport_width = aspect_ratio * viewport_height;

            horizontal = focus_dist * viewport_width * u;
            vertical = focus_dist * viewport_height * v;
            lower_left_corner = origin - horizontal/2 - vertical/2 - focus_dist * w;

            this->focus_dist = _focus_dist;

            lens_radius = _aperture/2;
        }

        ray get_ray(double s, double t) const{
            vec3 rd = lens_radius * random_in_unit_disk();
            vec3 offset = u*rd.x() + v*rd.y();
            return ray(origin+offset, lower_left_corner+s*horizontal+t*vertical-origin-offset);
        }

        void translate(const vec3 translation_vector) {
            origin += translation_vector;
            lower_left_corner += translation_vector;
        }

        void truck(const double how_much) {
            origin += how_much * u;
            lower_left_corner += how_much * u;
        }

        void dolly(const double how_much) {
            origin += how_much * w;
            lower_left_corner += how_much * w;
        }

        void rotate(const double yaw, const double pitch, const double roll) {

            // https://en.wikipedia.org/wiki/Rotation_matrix#General_rotations

            const double a = yaw, b = pitch, c = roll;

            // rows of the rotation matrix
            const vec3 R_1 (
                cos(a)*cos(b),
                sin(a)*sin(b)*sin(c)-sin(a)*cos(c),
                cos(a)*sin(b)*cos(c)+sin(a)*sin(c));

            const vec3 R_2 (
                sin(a)*cos(b),
                sin(a)*sin(b)*sin(c)+cos(a)*cos(c),
                sin(a)*sin(b)*cos(c)-cos(a)*sin(c));

            const vec3 R_3 (
                -sin(b),
                sin(b)*cos(c),
                cos(b)*cos(c));

            u = unitVector(vec3(
                dot(R_1, u),
                dot(R_2, u),
                dot(R_3, u)));

            v = unitVector(vec3(
                dot(R_1, v),
                dot(R_2, v),
                dot(R_3, v)));

            w = unitVector(vec3(
                dot(R_1, w),
                dot(R_2, w),
                dot(R_3, w)));

            horizontal = horizontal.length() * u;
            vertical = vertical.length() * v;
            lower_left_corner = origin - horizontal/2 - vertical/2 - focus_dist * w;
        }

    public:
        point3 origin;
        point3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
        vec3 u,v,w;
        double lens_radius;
        double focus_dist;
        double vfov;
        double aspect_ratio;
};


#endif
