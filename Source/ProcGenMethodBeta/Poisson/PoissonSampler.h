#include "CoreMininal.h"

USTRUCT() 
struct UPoissonSettings
        {
            public:
                FVector3 TopLeft, LowerRight, Center;
                FVector Dimensions;
                float RejectionSqDistance;
                float MinimumDistance;
                float CellSize;
                int GridWidth, GridHeight, GridLength;
        }

USTRUCT()
struct State
{
    public:
    
}


class UPoisson
{
    GENERATE_BODY()

    public:
List<FVector> PoissonSampler::SampleSphere(FVector inCenter,float inRadius, float inMinimumDistance)

List<FVector> PoissonSampler::SampleSphere(FVector inCenter, float inRadius, float inMinimumDistance, int32 inPointsPerIteration)

void PoissonSampler::Sample(int seed, FVector inTopLeft, FVector inTopRight, FVector Dimensions, float inMinimumDistance, float inRejectionDistance)

FVector PoissonSampler::Denormalize(FVector inPoint, FVector inOrigin, double inCellSize)

float PoissonSampler::RandomFloat()
}
