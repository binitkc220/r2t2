#ifndef RT_H
#define RT_H

#include<cmath>
#include<limits>
#include<memory>
#include<random>

//Constants
const double INF = std::numeric_limits<double>::infinity();
const double PI = 3.1415926535897932385;

//Utility Functions
inline double degree_to_radian(double degree){
    return degree * PI / 180.0;
}


inline double random_double(double min, double max) {
    static std::uniform_real_distribution<double> distribution(min, max);
    static std::mt19937 generator;
    return distribution(generator);
}

inline double random_double() {
    return random_double(0.0, 1.0);
}

inline int random_int(int min, int max) {
    return static_cast<int>(random_double(min, max+1));
}

inline double clamp(double x, double min, double max) {
    if(x<min)return min;
    if(x>max)return max;
    return x;
}

inline double dmin3(double x, double y, double z) {
    return fmin(x,(fmin(y,z)));
}

inline double dmax3(double x, double y, double z) {
    return fmax(x,(fmax(y,z)));
}

inline double dmin4(double x, double y, double z, double w) {
    return fmin(x,(fmin(y,fmin(z,w))));
}

inline double dmax4(double x, double y, double z, double w) {
    return fmax(x,(fmax(y,fmax(z,w))));
}

//Common Headers

#endif
