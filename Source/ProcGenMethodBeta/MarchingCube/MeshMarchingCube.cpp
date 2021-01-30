// Fill out your copyright notice in the Description page of Project Settings.

#include "MeshMarchingCube.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#include "GenericPlatform/GenericPlatformMath.h"

#include "math.h"

#include <cmath>

// Constructor
UMeshMarchingCube::UMeshMarchingCube()
{
    // Create wrapper for caves
    //fastNoiseWrapper = CreateDefaultSubobject<UFastNoiseWrapper>(TEXT("FastNoiseWrapper"));

    // create wrapper for terrain
    //fastNoiseWrapperTerrain = CreateDefaultSubobject<UFastNoiseWrapper>(TEXT("FastNoiseWrapperTerrain"));
}

// Initialization
void UMeshMarchingCube::InitializeNoiseGridData()
{
    // Create Object
    if (fastNoiseWrapper == nullptr)
    {
        fastNoiseWrapper = NewObject<UFastNoiseWrapper>(this, TEXT("FastNoiseWrapper"));
    }

    if (fastNoiseWrapperTerrain == nullptr)
    {
        fastNoiseWrapperTerrain = NewObject<UFastNoiseWrapper>(this, TEXT("FastNoiseWrapperTerrain"));
    }

    // Create a fast noise and set values
    fastNoiseWrapper->SetupFastNoise();

    // Set up terrain noise
    fastNoiseWrapperTerrain->SetupFastNoise();

    // Set Noise Type
    fastNoiseWrapper->SetNoiseType(noiseType);
    fastNoiseWrapper->SetFractalType(fractalType);
    fastNoiseWrapper->SetFrequency(noiseFrequency);
    fastNoiseWrapper->SetOctaves(noiseOctaves);

    // Setup second noise
    fastNoiseWrapperTerrain->SetNoiseType(noiseTypeTerrain);
    fastNoiseWrapperTerrain->SetFractalType(fractalTypeTerrain);
    fastNoiseWrapperTerrain->SetFrequency(noiseFrequencyTerrain);
    fastNoiseWrapperTerrain->SetOctaves(noiseOctavesTerrain);

    // Empty data
    verticesData.Empty();
    trianglesData.Empty();
    verticesNormalData.Empty();
}

// This creates the vertex from data
void UMeshMarchingCube::Polygonization()
{
    // copy cutoff from valuable
    float isolevel = noiseCutoff;
    int ConfigurationValue = 0;

    // Used to create UV index
    int index0, index1, index2 = 0;
    int32 count = 0;

    // create a vertex list
    FVector vertlist[12];

    // Create a cell here so faster reset
    MarchingCubeCell cell;

    UE_LOG(LogTemp, Warning, TEXT("Marching Cube - Cube Size %i"), cubeSize);

    // get all eight points and determine the configuration
    for (int z = 0; z < cubeSize; z++)
    {
        for (int y = 0; y < cubeSize; y++)
        {
            for (int x = 0; x < cubeSize; x++)
            {
                // reset data;
                cell.ResetConfiguration();

                // Calculate Cell
                CalculateCellData(cell, x, y, z);

                // Get configuration - Using cutoff
                cell.QuickChangeConfiguration(isolevel);

                // Get configuration
                ConfigurationValue = (int)cell.GetConfiguration();

                ///*vertlist.Empty();
                memset(vertlist, 0, sizeof(vertlist));

                if (edgeTable[ConfigurationValue] == 0)
                    continue;
                if (edgeTable[ConfigurationValue] & 1)
                    vertlist[0] =
                        VertexInterp(isolevel, cell.v[0], cell.v[1], cell.val[0], cell.val[1]);
                if (edgeTable[ConfigurationValue] & 2)
                    vertlist[1] =
                        VertexInterp(isolevel, cell.v[1], cell.v[2], cell.val[1], cell.val[2]);
                if (edgeTable[ConfigurationValue] & 4)
                    vertlist[2] =
                        VertexInterp(isolevel, cell.v[2], cell.v[3], cell.val[2], cell.val[3]);
                if (edgeTable[ConfigurationValue] & 8)
                    vertlist[3] =
                        VertexInterp(isolevel, cell.v[3], cell.v[0], cell.val[3], cell.val[0]);
                if (edgeTable[ConfigurationValue] & 16)
                    vertlist[4] =
                        VertexInterp(isolevel, cell.v[4], cell.v[5], cell.val[4], cell.val[5]);
                if (edgeTable[ConfigurationValue] & 32)
                    vertlist[5] =
                        VertexInterp(isolevel, cell.v[5], cell.v[6], cell.val[5], cell.val[6]);
                if (edgeTable[ConfigurationValue] & 64)
                    vertlist[6] =
                        VertexInterp(isolevel, cell.v[6], cell.v[7], cell.val[6], cell.val[7]);
                if (edgeTable[ConfigurationValue] & 128)
                    vertlist[7] =
                        VertexInterp(isolevel, cell.v[7], cell.v[4], cell.val[7], cell.val[4]);
                if (edgeTable[ConfigurationValue] & 256)
                    vertlist[8] =
                        VertexInterp(isolevel, cell.v[0], cell.v[4], cell.val[0], cell.val[4]);
                if (edgeTable[ConfigurationValue] & 512)
                    vertlist[9] =
                        VertexInterp(isolevel, cell.v[1], cell.v[5], cell.val[1], cell.val[5]);
                if (edgeTable[ConfigurationValue] & 1024)
                    vertlist[10] =
                        VertexInterp(isolevel, cell.v[2], cell.v[6], cell.val[2], cell.val[6]);
                if (edgeTable[ConfigurationValue] & 2048)
                    vertlist[11] =
                        VertexInterp(isolevel, cell.v[3], cell.v[7], cell.val[3], cell.val[7]);

                /* Create the triangle */
                for (int i = 0; triTable[ConfigurationValue][i] != -1; i += 3)
                {
                    FVector trianglepoint0 = vertlist[triTable[ConfigurationValue][i]];
                    FVector trianglepoint1 = vertlist[triTable[ConfigurationValue][i + 1]];
                    FVector trianglepoint2 = vertlist[triTable[ConfigurationValue][i + 2]];

                    // Append Data
                    verticesData.Add(trianglepoint0);
                    //index0 = verticesData.Num() - 1;
                    index0 = count;
                    count++;

                    verticesData.Add(trianglepoint1);
                    //index1 = verticesData.Num() - 1;
                    index1 = count;
                    count++;

                    verticesData.Add(trianglepoint2);
                    //index2 = verticesData.Num() - 1;
                    index2 = count;
                    count++;

                    // Add triangles
                    trianglesData.Add(index0);
                    trianglesData.Add(index1);
                    trianglesData.Add(index2);
                }

                for (int i = 0; verticesData.Num() != -1; i += 3)
                {
                    // Create normal
                    FVector crossResult = FVector::CrossProduct(verticesData[i] - verticesData[i + 2], verticesData[i + 1] - verticesData[i + 2]);

                    // Normalize
                    crossResult.Normalize();

                    verticesNormalData.Add(crossResult);
                    verticesNormalData.Add(crossResult);
                    verticesNormalData.Add(crossResult);
                }
            }
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Generated Vertices %d  Triangles %d  Vertices %d"), verticesData.Num(), trianglesData.Num(), verticesNormalData.Num());
}

FVector UMeshMarchingCube::VertexInterp(float isolevel, FVector p1, FVector p2, double valp1, double valp2)
{
    // Needed Valuables

    double mu;
    FVector p;

    if (UKismetMathLibrary::Abs(isolevel - valp1) < 0.00001)
        return (p1);
    if (UKismetMathLibrary::Abs(isolevel - valp2) < 0.00001)
        return (p2);
    if (UKismetMathLibrary::Abs(valp1 - valp2) < 0.00001)
        return (p1);
    mu = (isolevel - valp1) / (valp2 - valp1);
    p.X = p1.X + mu * (p2.X - p1.X);
    p.Y = p1.Y + mu * (p2.Y - p1.Y);
    p.Z = p1.Z + mu * (p2.Z - p1.Z);

    return (p);
}

// Getter
TArray<FVector> UMeshMarchingCube::GetVerticesData()
{
    return verticesData;
}

// Getter
TArray<int32> UMeshMarchingCube::GetTrianglesData()
{
    return trianglesData;
}

// Getter
TArray<FVector> UMeshMarchingCube::GetNormalData()
{
    return verticesNormalData;
}

float UMeshMarchingCube::distanceSquare(FVector v1, FVector v2)
{
    float distance1 = (v2.X - v1.X) * (v2.X - v1.X);
    float distance2 = (v2.Y - v1.Y) * (v2.Y - v1.Y);
    float distance3 = (v2.Z - v1.Z) * (v2.Z - v1.Z);

    float combined = distance1 + distance2 + distance3;

    return (float)FGenericPlatformMath::Sqrt(combined);
}

// Initialization
void UMeshMarchingCube::CalculateCellData(MarchingCubeCell &cell, uint32 x, uint32 y, uint32 z)
{
    // Declared once
    float scale = cubeCellSize, noiseValue = 0.0f;

    // Use VPosition - Declare once to increase speed
    FVector vPosition = FVector(0.0f, 0.0f, 0.0f);

    // Calculate position
    float midpoint = (cubeSize * scale) / 2;
    FVector midpointVector = FVector(midpoint, midpoint, midpoint);

    float distancefromcenter = 0;

    // temporary to calculate terrain
    Vect3 newPosition;
    Vect3 Results;

    // check noise height
    float testNoise = 0;

    // Go through each point
    for (int cubepoint = 0; cubepoint < CellPointsLength; cubepoint++)
    {

        // cakculate pixel location
        vPosition.X = (x + VPoints[cubepoint].X) * scale - midpoint;
        vPosition.Y = (y + VPoints[cubepoint].Y) * scale - midpoint;
        vPosition.Z = (z + VPoints[cubepoint].Z) * scale - midpoint;

        // set value
        cell.v[cubepoint] = vPosition;

        // distance from center
        distancefromcenter = FGenericPlatformMath::Abs(distanceSquare(vPosition + midpoint, midpointVector));

        // convert point to space
        newPosition.x = vPosition.X;
        newPosition.y = vPosition.Y;
        newPosition.z = vPosition.Z;

        // Calculate xyz to a cube space
        Results = newPosition.cubizePoint();

        // Calculate back to a xyz so perlin can be used
        Results.ProjectCubizeToCubeXYZ();

        // Create terrain increase height - can be a valuable
        testNoise = FMath::Clamp((float)fastNoiseWrapperTerrain->GetNoise3D(Results.x, Results.y, Results.z), 0.0f, 1.0f) * 1000.0;

        // Use terrain noise to create meight height
        testNoise = testNoise + coreLevel;

        // make sure height is not used
        if (distancefromcenter < testNoise)
        {
            // Getnoisewarapper
            noiseValue = fastNoiseWrapper->GetNoise3D(vPosition.X, vPosition.Y, vPosition.Z);

            // make sure distance above core level
            if ((distancefromcenter > coreLevel - 600) && (distancefromcenter < (coreLevel + 300)))
            {
                // cut off based on noise
                if (noiseValue > noiseCutoff)
                {
                    noiseValue = 1.0f;
                }
                else
                {
                    // use cutoff
                    noiseValue = 0.0f;
                }
            }
            else
            {
                // hide core level
                noiseValue = 1.0f;
            }
        }
        else
        {
            // mustbe external
            noiseValue = 0;
        }

        // copy noise value - should be more effiecent
        cell.val[cubepoint] = noiseValue;
    }
}
