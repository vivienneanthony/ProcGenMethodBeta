// Vivienne Anthony
// 2021

#include "Vect3.h"


#include <iostream>


Vect3 Vect3::cubizePoint()
{
    float copyx = x;
    float copyy = y;
    float copyz = z;

    float absX = fabs(copyx);
    float absY = fabs(copyy);
    float absZ = fabs(copyz);

    int isXPositive = copyx > 0 ? 1 : 0;
    int isYPositive = copyy > 0 ? 1 : 0;
    int isZPositive = copyz > 0 ? 1 : 0;

    float maxAxis, uc, vc;

    Vect3 cubespace;

    // POSITIVE X
    if (isXPositive && absX >= absY && absX >= absZ)
    {
        // u (0 to 1) goes from +z to -z
        // v (0 to 1) goes from -y to +y
        maxAxis = absX;
        uc = -copyz;
        vc = copyy;
        cubespace.x = 0;
    }
    // NEGATIVE X
    if (!isXPositive && absX >= absY && absX >= absZ)
    {
        // u (0 to 1) goes from -z to +z
        // v (0 to 1) goes from -y to +y
        maxAxis = absX;
        uc = copyz;
        vc = copyy;
        cubespace.x = 1;
    }
    // POSITIVE Y
    if (isYPositive && absY >= absX && absY >= absZ)
    {
        // u (0 to 1) goes from -x to +x
        // v (0 to 1) goes from +z to -z
        maxAxis = absY;
        uc = copyx;
        vc = -copyz;
        cubespace.x = 2;
    }
    // NEGATIVE Y
    if (!isYPositive && absY >= absX && absY >= absZ)
    {
        // u (0 to 1) goes from -x to +x
        // v (0 to 1) goes from -z to +z
        maxAxis = absY;
        uc = copyx;
        vc = copyz;
        cubespace.x = 3;
    }
    // POSITIVE Z
    if (isZPositive && absZ >= absX && absZ >= absY)
    {
        // u (0 to 1) goes from -x to +x
        // v (0 to 1) goes from -y to +y
        maxAxis = absZ;
        uc = copyx;
        vc = copyy;
        cubespace.x= 4;
    }
    // NEGATIVE Z
    if (!isZPositive && absZ >= absX && absZ >= absY)
    {
        // u (0 to 1) goes from +x to -x
        // v (0 to 1) goes from -y to +y
        maxAxis = absZ;
        uc = -copyx;
        vc = copyy;
        cubespace.x = 5;
    }

    // Convert range from -1 to 1 to 0 to 1
    cubespace.y = 0.5f * (uc / maxAxis + 1.0f);
    cubespace.z = 0.5f * (vc / maxAxis + 1.0f);

    return cubespace;
}


void Vect3::ProjectCubizeToCubeXYZ()
{
    // convert range 0 to 1 to -1 to 1
    float uc = 2.0f * y - 1.0f;
    float vc = 2.0f * z - 1.0f;
    switch ((int)x)
    {
    case 0:
        x =  1.0f;
        y =    vc;
        z =   -uc;
        break;	// POSITIVE X
    case 1:
        x = -1.0f;
        y =    vc;
        z =    uc;
        break;	// NEGATIVE X
    case 2:
        x =    uc;
        y =  1.0f;
        z =   -vc;
        break;	// POSITIVE Y
    case 3:
        x =    uc;
        y = -1.0f;
        z =    vc;
        break;	// NEGATIVE Y
    case 4:
        x =    uc;
        y =    vc;
        z =  1.0f;
        break;	// POSITIVE Z
    case 5:
        x =   -uc;
        y =    vc;
        z = -1.0f;
        break;	// NEGATIVE Z
    }
}