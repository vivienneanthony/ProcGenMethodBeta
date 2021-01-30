#include "MarchingCubeCell.h"

// Reset Configuration
void MarchingCubeCell::ResetConfiguration()
{
    // set configuration to 0
    configuration = 0;          // configuration is the polygon

    // Reset and make sure all data is 0
    for(int i=0;i<8;i++)
    {
        v[i]=FVector(0.0f,0.0f,0.0f);
        val[i]=0.0f;
    }
}

// Change Configuration
void MarchingCubeCell::ChangeConfiguration(float v0, float v1,float v2, float v3, float v4, float v5, float v6, float v7, float isolevel)
{
    int cubeindex = 0;

    if (v0 < isolevel)
        cubeindex |= 1;
    if (v1 < isolevel)
        cubeindex |= 2;
    if (v2 < isolevel)
        cubeindex |= 4;
    if (v3 < isolevel)
        cubeindex |= 8;
    if (v4 < isolevel)
        cubeindex |= 16;
    if (v5 < isolevel)
        cubeindex |= 32;
    if (v6 < isolevel)
        cubeindex |= 64;
    if (v7 < isolevel)
        cubeindex |= 128;

    // set configuration using cube index
    configuration = cubeindex;

    return;
};


// Change Configuration
void MarchingCubeCell::QuickChangeConfiguration(float isolevel)
{
    int cubeindex = 0;

    if (val[0] < isolevel)
        cubeindex |= 1;
    if (val[1] < isolevel)
        cubeindex |= 2;
    if (val[2] < isolevel)
        cubeindex |= 4;
    if (val[3] < isolevel)
        cubeindex |= 8;
    if (val[4] < isolevel)
        cubeindex |= 16;
    if (val[5] < isolevel)
        cubeindex |= 32;
    if (val[6] < isolevel)
        cubeindex |= 64;
    if (val[7] < isolevel)
        cubeindex |= 128;

    // set configuration using cube index
    configuration = cubeindex;

    return;
};