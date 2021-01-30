#pragma once

class MarchingCubeCell
{
public:
    int configuration; // marching cube can only have 256 possibilies

    // Reset Configuration and clear all values
    void ResetConfiguration();

    // Change Configuration and set
    void ChangeConfiguration(float v0, float v1, float v2, float v3, float v4, float v5, float v6, float v7, float isolevel);

    // Change Configuration and set
    void QuickChangeConfiguration(float isolevel);

    // Get configuration of the triangles
    int GetConfiguration() { return configuration; };

    // vertex value
    FVector v[8];

    // noise value
    float val[8];
};
