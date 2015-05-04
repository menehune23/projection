//////////////////////////////////////////////////////////////////////
// Projection library for Arduino
// Created March 2105 by Andrew Meyer
// 
// NOTES:
//  - Coordinate system is a right-handed, Z-up system
//  - 3D rotations follow ZYX Euler angle convention (yaw-pitch-roll)
//  - Angles are assumed to be in degrees
//////////////////////////////////////////////////////////////////////

#include <Projection.h>

Transform::Transform()
    : m00(1), m01(0), m02(0),
      m10(0), m11(1), m12(0),
      m20(0), m21(0), m22(1),
      x(0),   y(0),   z(0)
{ }

Transform::Transform(float mat00, float mat01, float mat02,
                     float mat10, float mat11, float mat12,
                     float mat20, float mat21, float mat22,
                     float posX,  float posY,  float posZ)
    : m00(mat00), m01(mat01), m02(mat02),
      m10(mat10), m11(mat11), m12(mat12),
      m20(mat20), m21(mat21), m22(mat22),
      x(posX),    y(posY),    z(posZ)
{ }

Transform::Transform(float angleX, float angleY, float angleZ,
		             float scaleX, float scaleY, float scaleZ,
		             float posX,   float posY,   float posZ)
    : x(posX), y(posY), z(posZ)
{
    float sx = sin(DEG_TO_RAD(angleX));
    float sy = sin(DEG_TO_RAD(angleY));
    float sz = sin(DEG_TO_RAD(angleZ));
    float cx = cos(DEG_TO_RAD(angleX));
    float cy = cos(DEG_TO_RAD(angleY));
    float cz = cos(DEG_TO_RAD(angleZ));

    m00 = scaleX * cy * cz;
    m01 = scaleY * (sx * sy * cz - cx * sz);
    m02 = scaleZ * (sx * sz + cx * sy * cz);

    m10 = scaleX * cy * sz;
    m11 = scaleY * (sx * sy * sz + cx * cz);
    m12 = scaleZ * (cx * sy * sz - sx * cz);

    m20 = scaleX * -sy;
    m21 = scaleY * sx * cy;
    m22 = scaleZ * cx * cy;
}

Transform Transform::inverse() const
{
    // Compute N = inv(M)
    float n00 = (m11 * m22) - (m12 * m21);
    float n01 = (m02 * m21) - (m01 * m22);
    float n02 = (m01 * m12) - (m02 * m11);
    float n10 = (m12 * m20) - (m10 * m22);
    float n11 = (m00 * m22) - (m02 * m20);
    float n12 = (m02 * m10) - (m00 * m12);
    float n20 = (m10 * m21) - (m11 * m20);
    float n21 = (m01 * m20) - (m00 * m21);
    float n22 = (m00 * m11) - (m01 * m10);
    
    float det = (m02 * n20) + (m01 * n10) + (m00 * n00);
    
    n00 /= det; n01 /= det; n02 /= det;
    n10 /= det; n11 /= det; n12 /= det;
    n20 /= det; n21 /= det; n22 /= det;
    
    // Compute R = -N * B
    float r0 = -((n00 * x) + (n01 * y) + (n02 * z));
    float r1 = -((n10 * x) + (n11 * y) + (n12 * z));
    float r2 = -((n20 * x) + (n21 * y) + (n22 * z));

    // Return inverse as [ N R ]
    //                   [ 0 1 ]
    return Transform(n00, n01, n02,
                     n10, n11, n12,
                     n20, n21, n22,
                     r0,  r1,  r2);
}

Transform Transform::operator *(const Transform &rhs) const
{
    return Transform(m00 * rhs.m00 + m01 * rhs.m10 + m02 * rhs.m20,
                     m00 * rhs.m01 + m01 * rhs.m11 + m02 * rhs.m21,
                     m00 * rhs.m02 + m01 * rhs.m12 + m02 * rhs.m22,

                     m10 * rhs.m00 + m11 * rhs.m10 + m12 * rhs.m20,
                     m10 * rhs.m01 + m11 * rhs.m11 + m12 * rhs.m21,
                     m10 * rhs.m02 + m11 * rhs.m12 + m12 * rhs.m22,

                     m20 * rhs.m00 + m21 * rhs.m10 + m22 * rhs.m20,
                     m20 * rhs.m01 + m21 * rhs.m11 + m22 * rhs.m21,
                     m20 * rhs.m02 + m21 * rhs.m12 + m22 * rhs.m22,

                     m00 * rhs.x + m01 * rhs.y + m02 * rhs.z + x,
                     m10 * rhs.x + m11 * rhs.y + m12 * rhs.z + y,
                     m20 * rhs.x + m21 * rhs.y + m22 * rhs.z + z);
}

point3 Transform::operator *(const point3 &rhs) const
{
    return { m00 * rhs.x + m01 * rhs.y + m02 * rhs.z + x,
             m10 * rhs.x + m11 * rhs.y + m12 * rhs.z + y,
             m20 * rhs.x + m21 * rhs.y + m22 * rhs.z + z };
}

line3 Transform::operator *(const line3 &line) const
{
    return { (*this) * line.p0, (*this) * line.p1 };
}

Camera::Camera()
    : transform(Transform(-90, 0, 0, 1, 1, 1, 0, 0, 0))
{ }

Camera::Camera(int displayWidth, int displayHeight)
    : screenWidth(displayWidth), screenHeight(displayHeight), projMode(PROJ_PERSPECTIVE),
      focalDistPx(75), nearDist(MIN_NEAR_DIST), farDist(1000.0),
      transform(Transform(-90, 0, 0, 1, 1, 1, 0, 0, 0))
{ }

point2 Camera::project(const point3 &point, bool clip /* = true */, bool round /* = true */) const
{
    point2 ret;
    point3 camPoint = transform.inverse() * point; // Transform point to camera space
    
    if (clip)
    {
        // Clip to near/far 3D planes
        
        float near = max(nearDist, MIN_NEAR_DIST);
        float far = max(farDist, near);
        
        if (camPoint.z < near || camPoint.z > far)
        {
            return { NAN, NAN };
        }
    }
    
    // Project to screen
    ret = getImageCoords(camPoint);
    
    if (clip)
    {
        // Clip to 2D viewport

        if (ret.x < 0 || ret.x > (screenWidth - 1) ||
            ret.y < 0 || ret.y > (screenHeight - 1))
        {
            return { NAN, NAN };
        }
    }

    if (round)
    {
        ret = { round(ret.x), round(ret.y) };
    }
    
    return ret;
}

line2 Camera::project(const line3 &line, bool clip /* = true */, bool round /* = true */) const
{
    line2 ret;
    Transform worldToCam = transform.inverse();
    line3 camLine = { worldToCam * line.p0, worldToCam * line.p1 };  // Transform line to camera space
    
    if (clip)
    {
        // Clip line to near/far 3D planes
        
        clipLine(camLine);
        
        if (isnan(camLine.p0.x))
        {
            return { { NAN, NAN }, { NAN, NAN } };
        }
    }
    
    // Project to screen
    ret = { getImageCoords(camLine.p0), getImageCoords(camLine.p1) };
    
    if (clip)
    {
        // Clip line to 2D viewport
        
        clipLine(ret);
        
        if (isnan(ret.p0.x))
        {
            return { { NAN, NAN }, { NAN, NAN } };
        }
    }
    
    if (round)
    {
        ret = { { round(ret.p0.x), round(ret.p0.y) },
                { round(ret.p1.x), round(ret.p1.y) } };
    }
    
    return ret;
}

float Camera::getLineT(float bound, float start, float end)
{
    return (bound - start) / (end - start);
}

point2 Camera::getImageCoords(const point3 &camPoint) const
{
    if (projMode == PROJ_PERSPECTIVE)
    {
        return { (focalDistPx * camPoint.x) / camPoint.z + screenWidth / 2,
                 (focalDistPx * camPoint.y) / camPoint.z + screenHeight / 2 };
    }
    else
    {
        return { (screenWidth * camPoint.x) / orthoViewWidth + screenWidth / 2,
                 (screenWidth * camPoint.y) / orthoViewWidth + screenHeight / 2 };
    }
}

byte Camera::outcode(const point3 &point, float near, float far) const
{
    byte code = 0;
    
    bitWrite(code, 0, point.z < near);
    bitWrite(code, 1, point.z > far);
    
    return code;
}

byte Camera::outcode(const point2 &point) const
{
    byte code = 0;
    
    bitWrite(code, 0, point.x < 0);
    bitWrite(code, 1, point.x > (screenWidth - 1));
    bitWrite(code, 2, point.y < 0);
    bitWrite(code, 3, point.y > (screenHeight - 1));
    
    return code;
}

void Camera::clipLine(line3 &line) const
{
    bool done = false;
    float near = max(nearDist, MIN_NEAR_DIST);
    float far = max(farDist, near);
    
    do
    {
        byte code0 = outcode(line.p0, near, far);
        byte code1 = outcode(line.p1, near, far);
        
        if ((code0 | code1) == 0)
        {
            // Trivially accept
            done = true;
        }
        else if ((code0 & code1) != 0)
        {
            // Trivially reject
            line = { { NAN, NAN, NAN }, { NAN, NAN, NAN } };
            done = true;
        }
        else
        {
            // Clip one end
            
            byte code;
            point3 *point;
            
            if (code0)
            {
                code = code0;
                point = &line.p0;
            }
            else
            {
                code = code1;
                point = &line.p1;
            }
            
            // Compute point of intersection with clipping plane
            
            float bound = bitRead(code, 0) ? near : far;
            float t = getLineT(bound, line.p0.z, line.p1.z);
            
            point->x = line.p0.x + t * (line.p1.x - line.p0.x);
            point->y = line.p0.y + t * (line.p1.y - line.p0.y);
            point->z = bound;
        }
    } while (!done);
}

void Camera::clipLine(line2 &line) const
{
    bool done = false;
    
    do
    {
        byte code0 = outcode(line.p0);
        byte code1 = outcode(line.p1);
        
        if ((code0 | code1) == 0)
        {
            // Trivially accept
            done = true;
        }
        else if ((code0 & code1) != 0)
        {
            // Trivially reject
            line = { { NAN, NAN }, { NAN, NAN } };
            done = true;
        }
        else
        {
            // Clip one end
            
            byte code;
            point2 *point;
            
            if (code0)
            {
                code = code0;
                point = &line.p0;
            }
            else
            {
                code = code1;
                point = &line.p1;
            }
            
            // Compute point of intersection with clipping edge
            
            if (bitRead(code, 0))       // x < 0
            {
                float t = getLineT(0, line.p0.x, line.p1.x);
                
                point->x = 0;
                point->y = line.p0.y + t * (line.p1.y - line.p0.y);
            }
            else if (bitRead(code, 1))  // x > (screenWidth - 1)
            {
                float t = getLineT(screenWidth - 1, line.p0.x, line.p1.x);
                
                point->x = screenWidth - 1;
                point->y = line.p0.y + t * (line.p1.y - line.p0.y);
            }
            else if (bitRead(code, 2))  // y < 0
            {
                float t = getLineT(0, line.p0.y, line.p1.y);
                
                point->x = line.p0.x + t * (line.p1.x - line.p0.x);
                point->y = 0;
            }
            else                        // y > (screenHeight - 1)
            {
                float t = getLineT(screenHeight - 1, line.p0.y, line.p1.y);
                
                point->x = line.p0.x + t * (line.p1.x - line.p0.x);
                point->y = screenHeight - 1;
            }
        }
    } while (!done);
}
