///////////////////////////////////////////////
// GaussianPlot
// by Andrew Meyer, May 2015
// 
// Demonstrates drawing a 2D Gaussian function
// on a MicroView ( http://sfe.io/p12923 ).
///////////////////////////////////////////////

#include <MicroView.h>
#include <Projection.h>

#define UPDATE_DELAY  0

// Transform for plot (note Z scale)
Transform plotTrans = Transform(0, 0, 45, 1, 1, 3, 0, 0, 0);

// Create a camera
Camera cam(LCDWIDTH, LCDHEIGHT);

void setup()
{
    // Start MicroView
    uView.begin();
    
    // Set camera back and up a few units and angle downward.
    // 
    // Note that camera's projection axis is its own Z+ axis,
    // so an x-angle of -90 deg aligns it with the world Y+
    // axis, and less than -90 will angle the camera's
    // projection axis further downward.
    cam.transform = Transform(-90 - 10, 0, 0, 1, 1, 1, 0, -7, 2);
    
    // Uncomment these two lines for orthographic projection
    //cam.projMode = PROJ_ORTHO;
    //cam.orthoViewWidth = 3.0;
}

void loop()
{
    uView.clear(PAGE);
    
    // Draw Gaussian plot
    drawGaussPlot();
    
    uView.display();
    delay(UPDATE_DELAY);
}

void drawGaussPlot()
{
    float ux = 0, uy = 0;      // Means
    float sx = 0.5, sy = 0.5;  // Standard deviations
    
    // Plotting options
    float minX = -3, maxX = 3;
    float minY = -3, maxY = 3;
    float xStep = 0.25, yStep = 0.25;
    
    byte numPtsX = (maxX - minX) / xStep;
    byte numPtsY = (maxY - minY) / yStep;
    
    for (byte n = 0; n < numPtsX; n++)
    {
        for (byte m = 0; m < numPtsY; m++)
        {
            // Compute Gaussian point
            float x = (n * xStep) + minX;
            float y = (m * yStep) + minY;
            float z = gauss(x, y, ux, uy, sx, sy);
            point3 pt3 = { x, y, z };
            
            // Transform and project 3D point
            point2 pt2 = cam.project(plotTrans * pt3);
            
            // Draw point if not clipped
            if (!isnan(pt2.x))
            {
                uView.pixel(pt2.x, pt2.y);
            }
        }
    }
}

float gauss(float x, float y, float ux, float uy, float sx, float sy)
{
    return exp(-(
        ((x - ux) * (x - ux) / (2 * sx * sx)) +
        ((y - uy) * (y - uy) / (2 * sy * sy))))
        / (2 * PI * sx * sy);
}
