// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnPointsSphere.h"
#include <cstdlib>

// Main
SpawnPointsSphere::SpawnPointsSphere()
{
}

// Close
SpawnPointsSphere::~SpawnPointsSphere()
{
}

// Initialize
bool SpawnPointsSphere::Initialize(uint32 inSeed, float inRadius, FVector inVectorOrigin, FVector inActionLocation, int32 inCountMax)
{
    // Set Origin
    currentOrigin = inVectorOrigin;

    // actor location
    currentActorLocation = inActionLocation;

    // Set Count
    currentCount = 0;

    // Count Max
    currentCountMax = inCountMax;

    // Change InRadius
    currentRadius = inRadius;

    // Change seed  
    currentSeed = inSeed;

    // Initialize instream
    currentStream.Initialize(inSeed);

    return true;
}

// Get next point point
FVector SpawnPointsSphere::GetNextPoint()
{
    // outVector
    FVector outVector;

    // test incement
    if((currentCount+1)>currentCountMax)
    {
        // blank
        outVector = FVector(0.0f,0.0f,0.0f);
    }else
    {
        // Get a point
        FVector point = GetPoint();
       
        // outVector
        outVector = point;

        // Increment
        currentCount++;           
    }
    return outVector;
}

// Reset Origin
bool SpawnPointsSphere::SetActorLocation(FVector inActorLocation)
{
    // last Actor position
   lastActorLocation =  currentActorLocation;

   // set Origin
   currentActorLocation = inActorLocation;

   return true;        
}

bool SpawnPointsSphere::ResetSeed()
{
   // set Count
   currentCount = 0; 

   // Reset
   currentStream.Initialize(currentSeed);   

   return true;
}


FVector SpawnPointsSphere::GetPoint() 
{  
    float u = currentStream.GetFraction();
    float v = currentStream.GetFraction();

    float theta = 2 * PI * u;
    float phi =  UKismetMathLibrary::Acos(1 - 2 * v);
    
    float sinTheta =  UKismetMathLibrary::Sin(theta);
    float cosTheta = UKismetMathLibrary::Cos(theta);
    float sinPhi = UKismetMathLibrary::Sin(phi);
    float cosPhi = UKismetMathLibrary::Cos(phi);

    float x1 = sinPhi * cosTheta;
    float y1 = sinPhi * sinTheta;
    float z1 = cosPhi;

    return FVector(x1*currentRadius, y1*currentRadius, z1*currentRadius);
}

  