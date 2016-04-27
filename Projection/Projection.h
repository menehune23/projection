//////////////////////////////////////////////////////////////////////
// Projection library for Arduino
// Created March 2105 by Andrew Meyer
// 
// NOTES:
//  - Coordinate system is a right-handed, Z-up system
//  - 3D rotations follow ZYX Euler angle convention (yaw-pitch-roll)
//  - Angles are assumed to be in degrees
//////////////////////////////////////////////////////////////////////

#ifndef PROJECTION_H
#define PROJECTION_H

#include <Arduino.h>

// Macro to convert an angle from degrees to radians
#define DEG_TO_RAD(deg)  deg * 0.017453

// Macro to convert an angle from radians to degrees
#define RAD_TO_DEG(rad)  rad * 57.29578

// Camera projection mode
enum PROJ_MODE {
    PROJ_PERSPECTIVE,  // Perspective projection
    PROJ_ORTHO         // Orthographic projection
};

// A simple 2D point structure
struct point2
{
    float x, y;
};

// A simple 3D point structure
struct point3
{
    float x, y, z;
};

// A simple 2D line structure
struct line2
{
    point2 p0, p1;
};

// A simple 3D line structure
struct line3
{
    point3 p0, p1;
};

// A 4x4 homogeneous transform of block-matrix form:
// 
//     H = [ M B ]
//         [ 0 1 ]
// 
// where M is a 3x3 transformation matrix, usually representing
// rotation and scaling, and B is a 3x1 translation vector
class Transform
{
public:
    // Constructs a default Transform equivalent to the 4x4
    // identity matrix
    Transform();
    
    // Constructs a Transfrom from given matrix and 
    // translation values
    Transform(float mat00, float mat01, float mat02,
		      float mat10, float mat11, float mat12,
		      float mat20, float mat21, float mat22,
		      float posX,  float posY,  float posZ);
    
    // Constructs a Transform with a specified rotation,
    // scale, and translation
    Transform(float angleX, float angleY, float angleZ,
		      float scaleX, float scaleY, float scaleZ,
		      float posX,   float posY,   float posZ);
    
    // Computes the inverse of a Transform
    Transform inverse() const;
    
    // Multiplies two Transforms
    Transform operator *(const Transform &rhs) const;
    
    // Transforms a 3D point
    point3 operator *(const point3 &point) const;
    
    // Transforms a 3D line
    line3 operator *(const line3 &line) const;
    
    // Elements of Transform's matrix
    float m00, m01, m02;
	float m10, m11, m12;
	float m20, m21, m22;
    
    // Translation components of Transform
	float x, y, z;
};

// A projection camera for projecting points and lines
// from 3D to 2D
class Camera
{
public:
    // Constructs a default camera at world origin, with its
    // +Z axis looking along the world +Y axis
    Camera();
    
    // Constructs a camera given display dimensions, at world origin,
    //  with its +Z axis looking along the world +Y axis
    Camera(int displayWidth, int displayHeight);
    
    // Projects a 3D point onto 2D screen, with optional clipping to
    // screen bounds and rounding to nearest pixel
    point2 project(const point3 &point, bool clip = true, bool round = true) const;
    
    // Projects a 3D line onto 2D screen, with optional clipping to
    // screen bounds and rounding to nearest pixel
    line2  project(const line3 &line, bool clip = true, bool round = true) const;
        
    PROJ_MODE projMode;    // Projection mode
    Transform transform;   // Transform of camera relative to world
    float focalDistPx;     // Focal distance, in pixels
    float nearDist;        // Near clipping distance
    float farDist;         // Far clipping distance
    float orthoViewWidth;  // Viewing width for orthographic mode
    int screenWidth;       // Width of display
    int screenHeight;      // Height of display
    
private:
    static constexpr const float MIN_NEAR_DIST = 0.1;
    static float getLineT(float bound, float start, float end);
    
    point2 getImageCoords(const point3 &camPoint) const;
    byte outcode(const point3 &point, float near, float far) const;
    byte outcode(const point2 &point) const;
    void clipLine(line3 &line) const;
    void clipLine(line2 &line) const;
};

#endif // PROJECTION_H