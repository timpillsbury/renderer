#pragma once

#include <assimp/types.h>
#include <assimp/camera.h>
#include <assert.h>
#include <sstream>
#include <vector>
#include <array>


static constexpr float EPS = 0.00001f;
static constexpr float FLT_MAX = std::numeric_limits<float>::max();

float eps_zero(float a);


inline constexpr float min(float a, float b, float c) {
    return a < b ? (a < c ? a : c) : (b < c ? b : c);
}

inline constexpr float max(float a, float b, float c) {
    return a > b ? (a > c ? a : c) : (b > c ? b : c);
}


enum class Axis : char { X = 0, Y = 1, Z = 2};
static constexpr std::array<Axis, 3> AXES = {{Axis::X, Axis::Y, Axis::Z}};


//
// Extend vector by [] operator to access axis coordinate.
//
class Vec : public aiVector3D {
public:
    template <typename... Args>
    Vec(Args... args) : aiVector3D(args...) {}

    using aiVector3D::operator[];

    float operator[](const Axis ax) const {
        return *(this->v + static_cast<int>(ax));
    }

    float& operator[](const Axis ax) {
        return *(this->v + static_cast<int>(ax));
    }


    bool operator<(const Vec& v) const {
        return x < v.x && y < v.y && z < v.z;
    }

    bool operator<=(const Vec& v) const {
        return x <= v.x && y <= v.y && z <= v.z;
    }
};

Vec operator/(int a, const Vec& v);

struct Vec2 {

    float operator[](unsigned int i) const {
        return *(&x + i);
    }

    union {
        struct {
            float x, y;
        };
        float vec[2];
    };
};

using Color = aiColor4D;

float fmin(float x, float y, float z);
float fmax(float x, float y, float z);

// AABB
struct Box {
    float surface_area() const {
        auto e0 = max[Axis::X] - min[Axis::X];
        auto e1 = max[Axis::Y] - min[Axis::Y];
        auto e2 = max[Axis::Z] - min[Axis::Z];
        return 2.f * (e0 * e1 + e0 * e2 + e1 * e0);
    }

    Vec min, max;

    // Check if the box is planar in the plane with normal `ax`.
    bool is_planar(Axis ax) const {
        return !(std::abs((max - min)[static_cast<int>(ax)]) > EPS);
    }

    bool is_trivial() const {
        return
            eps_zero(min[Axis::X]) &&
            eps_zero(min[Axis::Y]) &&
            eps_zero(min[Axis::Z]) &&
            eps_zero(max[Axis::X]) &&
            eps_zero(max[Axis::Y]) &&
            eps_zero(max[Axis::Z]);
    }

    // Split `box` on the plane defined by the position `pos` at axis `ax`.
    std::pair<Box, Box> split(Axis plane_ax, float plane_pos) const
    {
        assert(this->min[plane_ax] - EPS <= plane_pos);
        assert(plane_pos <= this->max[plane_ax] + EPS);

        Vec lmax = this->max;
        lmax[plane_ax] = plane_pos;
        Vec rmin = this->min;
        rmin[plane_ax] = plane_pos;

        return {{this->min, lmax}, {rmin, this->max}};
    }
};
Box operator+(const Box& a, const Box& b);

// Light Source
struct Light {
    Vec position;
    Color color;
};

// Ray with precomputed inverse direction
struct Ray : public aiRay {
    Ray(const Vec& pos, const Vec& dir);
    Ray(const aiRay& ray);

    // pos, dir are in aiRay
    Vec invdir;
};


// Represents a camera including transformation
class Camera : public aiCamera {
public:
    // Represents a camera including transformation
    template<typename... Args>
    Camera(const aiMatrix4x4& trafo, Args... args)
        : aiCamera{args...}
        , trafo_(trafo)  // discard tranlation in trafo
    {
        assert(mPosition == Vec());
        assert(mUp == Vec(0, 1, 0));
        assert(mLookAt == Vec(0, 0, -1));
        assert(mAspect != 0);

        mPosition = trafo * mPosition;
        delta_x_ = tan(mHorizontalFOV);
        delta_y_ = delta_x_ / mAspect;
    }

    //
    // Convert 2d raster coodinates into 3d cameras coordinates.
    //
    // We assume that the camera is trivial:
    //   assert(cam.mPosition == (0, 0, 0))
    //   assert(cam.mUp == (0, 1, 0))
    //   assert(cam.mLookAt == (0, 0, -1))
    //   assert(cam.mAspect != 0)
    //
    // The positioning of the camera is done in its parent's node
    // transformation matrix.
    //
    aiVector3D raster2cam(const aiVector2D& p, const float w, const float h) const;

private:
    aiMatrix3x3 trafo_;
    float delta_x_, delta_y_;
};
