// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Kismet/KismetMathLibrary.h"

/**
 * 
 */
class PROCGENMETHODBETA_API SpawnPointsSphere
{
public:
	SpawnPointsSphere();
	~SpawnPointsSphere();

	// Initialize
	bool Initialize(uint32 inSeed, float inRadius, FVector inVectorOrigin, FVector inActionLocation, int32 inCountMax);

	// Get the next point
	FVector4 GetNextPoint();

	// Setters;
	bool SetActorLocation(FVector inActorLocation);

	// Get Point
	FVector4 GetPoint();

	// Check In Bound
	bool CheckInBound(FVector inVector);

	// Reset seed
	bool ResetSeed();

	// bInitialized
	bool bInitialized = false;

	 // Set Origin
    FVector currentOrigin;

    // actor location
    FVector currentActorLocation;

	FVector lastActorLocation;

    // Set Count
    uint currentCount;

    // Count Max
    int32 currentCountMax;

	//radious
    float currentRadius;

	
private:
	// current stream
	FRandomStream currentStream;

	// current seed
	uint32 currentSeed;

    float u;
    float v;

    float theta;
    float phi;
    
    float sinTheta;
    float cosTheta;
    float sinPhi;
    float cosPhi;

    float x1;
    float y1;
    float z1;

	FVector outVector;

};
