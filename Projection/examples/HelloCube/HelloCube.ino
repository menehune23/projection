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

// Cube lines
line3 cubeLines[] = {
    { cubeVerts[0], cubeVerts[1] },
    { cubeVerts[1], cubeVerts[2] },
    { cubeVerts[2], cubeVerts[3] },
    { cubeVerts[3], cubeVerts[0] },
    { cubeVerts[4], cubeVerts[5] },
    { cubeVerts[5], cubeVerts[6] },
    { cubeVerts[6], cubeVerts[7] },
    { cubeVerts[7], cubeVerts[4] },
    { cubeVerts[0], cubeVerts[4] },
    { cubeVerts[1], cubeVerts[5] },
    { cubeVerts[2], cubeVerts[6] },
    { cubeVerts[3], cubeVerts[7] },
};

// Create a camera
Camera cam(LCDWIDTH, LCDHEIGHT);

void setup()
{
    // Start MicroView
    uView.begin();
    
    // Camera starts out at origin, looking along world +X axis (its own Z+ axis).
    // Set camera back a few units so cube will be in view.
    cam.transform.x = -5;
    
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
    for (int i = 0; i < 12; i++)
    {
        // Project line to screen after applying cube transform
        line2 line = cam.project(cubeTrans * cubeLines[i]);
        
        // Draw if not clipped completely
        if (!isnan(line.p0.x))
        {
            uView.line(line.p0.x, line.p0.y, line.p1.x, line.p1.y);
        }
    }
}

