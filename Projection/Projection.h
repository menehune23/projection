#ifndef PROJECTION_H
#define PROJECTION_H

#include <Arduino.h>

#define DEG_TO_RAD(deg)  deg * 0.017453
#define RAD_TO_DEG(rad)  rad * 57.29578

enum PROJ_MODE { PROJ_PERSPECTIVE, PROJ_ORTHO };

struct point2
{
    float x, y;
};

struct point3
{
    float x, y, z;
};

struct line2
{
    point2 p0, p1;
};

struct line3
{
    point3 p0, p1;
};

class Transform
{
public:
    Transform();
    
    Transform(float mat00, float mat01, float mat02,
		      float mat10, float mat11, float mat12,
		      float mat20, float mat21, float mat22,
		      float posX,  float posY,  float posZ);
    
    Transform(float angleX, float angleY, float angleZ,
		      float scaleX, float scaleY, float scaleZ,
		      float posX,   float posY,   float posZ);
    
    Transform inverse() const;
    
    Transform operator *(const Transform &rhs) const;
    point3 operator *(const point3 &point) const;
    line3 operator *(const line3 &line) const;
    
    float m00, m01, m02;
	float m10, m11, m12;
	float m20, m21, m22;
	float x, y, z;
    
};

class Camera
{
public:
    Camera();
    Camera(int displayWidth, int displayHeight);
    
    point2 project(const point3 &point, bool clip = true, bool round = true) const;    
    line2 project(const line3 &line, bool clip = true, bool round = true) const;
        
    PROJ_MODE projMode;
    Transform transform;
    float focalDistPx;
    float nearDist;
    float farDist;
    float orthoViewWidth;
    int screenWidth;
    int screenHeight;

private:
    static const float MIN_NEAR_DIST = 0.1;
    static float getLineT(float bound, float start, float end);
    
    point2 getImageCoords(const point3 &camPoint) const;
    byte outcode(const point3 &point, float near, float far) const;
    byte outcode(const point2 &point) const;
    void clipLine(line3 &line) const;
    void clipLine(line2 &line) const;
    
};

#endif // PROJECTION_H