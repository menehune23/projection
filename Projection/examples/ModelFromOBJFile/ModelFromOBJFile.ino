///////////////////////////////////////////////
// ModelFromOBJFile
// by Andrew Meyer, April 2015
// 
// Demonstrates drawing a simple OBJ model on a 
// Microview ( http://sfe.io/p12923 ).
// 
// Removes redundancy by applying model's
// transform to vertices before forming lines.
///////////////////////////////////////////////

#include <MicroView.h>
#include <Projection.h>

#define UPDATE_DELAY  0

// Model properties to keep track of
float angle = -90;
Transform modelTrans;

// vertices and lineIndices code was generated
// from the ObjConverter tool.
// Run ObjConverter.html in a browser -- no
// internet required!

// Model vertices
const byte NUM_VERTICES = 28;
point3 vertices[] = {
    { -1.036572, 1.036572, 0.303021 },
    { 0.466702, -0.357184, 0.258869 },
    { -1.036572, -0.466702, 0.303021 },
    { 0.533960, -1.182932, -0.246472 },
    { -0.585337, -1.182932, -0.246472 },
    { 0.533960, -1.182932, -0.028611 },
    { -0.585337, -1.182932, -0.028611 },
    { 1.033760, -1.851447, -0.434040 },
    { 0.605000, -1.851447, -0.434040 },
    { 1.296128, 0.574044, -0.421914 },
    { 1.296128, 0.574044, -0.188807 },
    { -1.036572, -0.466702, -0.303021 },
    { 0.466702, -0.357184, -0.258869 },
    { 0.466702, 0.927055, -0.258869 },
    { -1.036572, 1.036572, -0.303021 },
    { 0.466702, 0.927055, 0.258869 },
    { 0.533960, 1.752803, -0.246472 },
    { -0.585337, 1.752803, -0.246472 },
    { 0.533960, 1.752803, -0.028611 },
    { -0.585337, 1.752803, -0.028611 },
    { 1.033760, 2.421317, -0.434040 },
    { 0.605000, 2.421317, -0.434040 },
    { 1.296128, -0.004174, -0.421914 },
    { 1.296128, -0.004174, -0.188807 },
    { -1.296128, 0.926559, 0.205740 },
    { -1.296128, -0.356688, 0.205740 },
    { -1.296128, -0.356688, -0.205740 },
    { -1.296128, 0.926559, -0.205740 }
};

// Model line indices
// Each pair of indices defines a line
const byte NUM_INDICES = 108;
byte lineIndices[] = {
    0, 15,
    15, 13,
    13, 14,
    14, 0,
    4, 3,
    3, 7,
    7, 8,
    8, 4,
    1, 2,
    2, 6,
    6, 5,
    5, 1,
    1, 15,
    0, 2,
    12, 13,
    13, 9,
    9, 22,
    22, 12,
    11, 12,
    12, 3,
    4, 11,
    14, 17,
    17, 16,
    16, 13,
    2, 11,
    4, 6,
    11, 14,
    17, 21,
    21, 20,
    20, 16,
    16, 18,
    18, 15,
    12, 1,
    5, 3,
    10, 23,
    23, 22,
    9, 10,
    23, 1,
    0, 19,
    19, 17,
    10, 15,
    18, 19,
    14, 27,
    27, 24,
    24, 0,
    26, 25,
    25, 24,
    27, 26,
    2, 25,
    26, 11,
    8, 6,
    7, 5,
    19, 21,
    18, 20
};

///////////////////////////////////////////

// Create a camera
Camera cam(LCDWIDTH, LCDHEIGHT);

void setup()
{
    // Start MicroView
    uView.begin();
    
    // Camera starts out at origin, looking along world +Y axis (its own +Z axis).
    // Set camera back a few units so model will be in view.
    cam.transform.y = -5;
    
    // Uncomment these two lines for orthographic projection
    //cam.projMode = PROJ_ORTHO;
    //cam.orthoViewWidth = 3.0;
}

void loop()
{
    uView.clear(PAGE);
    
    updateModel();
    drawModel();
    
    uView.display();
    delay(UPDATE_DELAY);
}

void updateModel()
{   
    // Update model transform
    modelTrans = Transform(0, 5, angle += 5, 1, 1, 1, 0, 0, 0);
}

void drawModel()
{
    point3 transVerts[NUM_VERTICES];
    
    // Apply model transform to vertices
    for (byte i = 0; i < NUM_VERTICES; i++)
    {
        transVerts[i] = modelTrans * vertices[i];
    }
    
    // Draw projected model lines
    for (byte i = 0; i < NUM_INDICES; i += 2)
    {
        // Get line to project
        line3 modelLine = {
            transVerts[lineIndices[i]],
            transVerts[lineIndices[i + 1]]
        };
        
        // Project line to screen
        line2 line = cam.project(modelLine);
        
        // Draw if not clipped completely
        if (!isnan(line.p0.x))
        {
            uView.line(line.p0.x, line.p0.y, line.p1.x, line.p1.y);
        }
    }
}

