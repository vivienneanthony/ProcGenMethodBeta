#pragma once
 
#include "CoreMinimal.h"

#include "GenericPlatform/GenericPlatformMath.h"
#include "Math/UnrealMathVectorConstants.h"

#include "Math/RandomStream.h"

struct PROCGENMETHODBETA_API PoissonSettings
        {
            public:
                FVector TopLeft;            // Left
                FVector LowerRight;          // Right
                FVector Center;             // Center

                FVector Dimensions;         // Dimensions
                
                float RejectionSqDistance;
                float MinimumDistance;

                float CellSize;             // Cell Size

                int32 GridWidth;              // Grid width
                int32 GridHeight;             // Grid height
                int32 GridLength;             // Grid Length
        };


struct PROCGENMETHODBETA_API State
{
    public:
         TArray<FVector> Grid;                      // Grid points
         TArray<FVector> ActivePoints;              // Active points
         TArray<FVector> Points;                    // Points
    
};


class PROCGENMETHODBETA_API PoissonSampler
{
    
    public:
    // Get sample sphere but wrong    
    TArray<FVector> SampleSphere(int32 inSeed,FVector inCenter,float inRadius, float inMinimumDistance);

    // Get sample sphere but wrong
    TArray<FVector> SampleSphere(int32 inSeed,FVector inCenter, float inRadius, float inMinimumDistance, int32 inPointsPerIteration);

    // sample
    TArray<FVector> Sample(int seed, FVector inTopLeft, FVector inLowerRight, float inMinimumDistance, float inRejectionDistance,  int32 inPointsPerIteration);

    // Not sure what this does
    FVector Denormalize(FVector inPoint, FVector inOrigin, double inCellSize);

    // Add first point
    void AddFirstPoint(PoissonSettings inSettings, State &inState);

    // I think match
    bool AddNextPoint(FVector inPoint, PoissonSettings inSettings, State & state);

    // Need to return a float
    float RandomFloatFromRandomStream();

    // Calculate Index
    int32 CalculateIndex(PoissonSettings inSettings,  int32 i, int32 j, int32 k);
    
    // Generate random number
    FVector GenerateRandomAround(FVector inCenter, float inMinimumDistance);
            
    // random stream
    FRandomStream randomStream;
};
