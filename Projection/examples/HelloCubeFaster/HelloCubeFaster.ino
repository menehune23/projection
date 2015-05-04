///////////////////////////////////////////////
// HelloCubeFaster
// by Andrew Meyer, Mar 2015
// 
// Demonstrates drawing a simple cube on a 
// Microview ( http://sfe.io/p12923 ).
// 
// Removes redundancy by applying cube's
// transform to vertices before forming lines.
///////////////////////////////////////////////

#include <MicroView.h>
#include <Projection.h>

#define MIN_SCALE     0.1
#define MAX_SCALE     1.5
#define UPDATE_DELAY  0

// Cube properties to keep track of
float roll, pitch, yaw;
float scale = MIN_SCALE;
float scaleStep = 0.015;
Transform cubeTrans;

// Cube vertices
point3 cubeVerts[] = {
    {  1,  1,  1 },
    {  1, -1,  1 },
    { -1, -1,  1 },
    { -1,  1,  1 },
    {  1,  1, -1 },
    {  1, -1, -1 },
    { -1, -1, -1 },
    { -1,  1, -1 }
};
    
// Each pair of indices denotes endpoints
// from cubeVerts above
byte cubeLineIndices[] = {
    0, 1,  // Top square
    1, 2,
    2, 3,
    3, 0,
    
    4, 5,  // Bottom square
    5, 6,
    6, 7,
    7, 4,
    
    0, 4,  // Sides
    1, 5,
    2, 6,
    3, 7
};

// Create a camera
Camera cam(LCDWIDTH, LCDHEIGHT);

void setup()
{
    // Start MicroView
    uView.begin();
    
    // Camera starts out at origin, looking along world +Y axis (its own +Z axis).
    // Set camera back a few units so cube will be in view.
    cam.transform.y = -5;
    
    // Uncomment these two lines for orthographic projection
    //cam.projMode = PROJ_ORTHO;
    //cam.orthoViewWidth = 3.0;
}

void loop()
{
    uView.clear(PAGE);
    
    updateCube();
    drawCube();
    
    uView.display();
    delay(UPDATE_DELAY);
}

void updateCube()
{
    // Update scale
    
    scale += scaleStep;
    
    if (scale < MIN_SCALE)
    {
        scale = MIN_SCALE;
        scaleStep *= -1;
    }
    else if (scale > MAX_SCALE)
    {
        scale = MAX_SCALE;
        scaleStep *= -1;
    }
    
    // Update cube transform
    cubeTrans = Transform(roll += 2, pitch, yaw += 2, scale, scale, scale, 0, 0, 0);
}

void drawCube()
{
    point3 verts[8];
    
    // Apply cube transform to vertices
    for (byte i = 0; i < 8; i++)
    {
        verts[i] = cubeTrans * cubeVerts[i];
    }
    
    // Draw projected cube lines
    for (byte i = 0; i < 24; i += 2)
    {
        // Get line to project
        line3 cubeLine = {
            verts[cubeLineIndices[i]],
            verts[cubeLineIndices[i + 1]]
        };
        
        // Project line to screen
        line2 line = cam.project(cubeLine);
        
        // Draw if not clipped completely
        if (!isnan(line.p0.x))
        {
            uView.line(line.p0.x, line.p0.y, line.p1.x, line.p1.y);
        }
    }
}

