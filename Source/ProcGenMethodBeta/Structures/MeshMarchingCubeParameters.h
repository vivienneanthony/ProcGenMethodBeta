#pragma once

// ProcGenMethodBeta Plugin
// Developer Vivienne Anthony
// Copyright (c) 2021 
#include "CoreMinimal.h"

#include "FastNoiseWrapper.h"

#include "MeshMarchingCubeParameters.generated.h"

USTRUCT()
struct PROCGENMETHODBETA_API FMeshMarchingCubeParameters
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "Noise Settings")
    EFastNoise_NoiseType noiseType;

    UPROPERTY(EditAnywhere, Category = "Noise Settings")
    EFastNoise_FractalType fractalType;

    UPROPERTY(EditAnywhere, Category = "Noise Settings")
    int32 noiseOctaves;

    UPROPERTY(EditAnywhere, Category = "Noise Settings")
    float noiseFrequency;

    UPROPERTY(EditAnywhere, Category = "Noise Settings")
    float noiseCutoff;

    UPROPERTY(EditAnywhere, Category = "Noise Settings")
    EFastNoise_NoiseType noiseTypeTerrain;

    UPROPERTY(EditAnywhere, Category = "Noise Settings")
    EFastNoise_FractalType fractalTypeTerrain;

    UPROPERTY(EditAnywhere, Category = "Noise Settings")
    int32 noiseOctavesTerrain;

    UPROPERTY(EditAnywhere, Category = "Noise Settings")
    float noiseFrequencyTerrain;

    UPROPERTY(EditAnywhere, Category = "Noise Settings")
    float noiseCutoffTerrain;

    UPROPERTY(EditAnywhere, Category = "Noise Settings")
    int32 cubeSize;

    UPROPERTY(EditAnywhere, Category = "Levels")
    float surfaceLevel;

    UPROPERTY(EditAnywhere, Category = "Levels")
    float coreLevel;
};