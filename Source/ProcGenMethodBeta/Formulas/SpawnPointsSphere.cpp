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
FVector4 SpawnPointsSphere::GetNextPoint()
{    
    // test incement
    // if((currentCount+1)>currentCountMax)
    // { 
        // blank
    //    outVector = FVector(0.0f,0.0f,0.0f);
    // }else
    // { 
        // Get a point
       outVector = GetPoint();
       
        // outVector
        // outVector = point;

        // // Increment
        // currentCount++;           
    //    }

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


FVector4 SpawnPointsSphere::GetPoint() 
{  
    //float u = currentStream.GetFraction();
    //float v = currentStream.GetFraction();

    //float theta = 2 * PI * u;
    //float phi =  UKismetMathLibrary::Acos(1 - 2 * v);
    
    //float sinTheta =  UKismetMathLibrary::Sin(theta);
    //float cosTheta = UKismetMathLibrary::Cos(theta);
    //float sinPhi = UKismetMathLibrary::Sin(phi);
    //float cosPhi = UKismetMathLibrary::Cos(phi);

    //float x1 = sinPhi * cosTheta;
    //float y1 = sinPhi * sinTheta;
    //float z1 = cosPhi;


    //u = currentStream.GetFraction();
    //v = currentStream.GetFraction();

    u = currentStream.FRandRange(0.0f, 1.00f);
    v = currentStream.FRandRange(0.0f, 0.1f);

    theta = 2 * PI * u;
    phi =  UKismetMathLibrary::Acos(1 - 2 * v);
    
    sinTheta =  UKismetMathLibrary::Sin(theta);
    cosTheta = UKismetMathLibrary::Cos(theta);
    sinPhi = UKismetMathLibrary::Sin(phi);
    cosPhi = UKismetMathLibrary::Cos(phi);

    x1 = sinPhi * cosTheta;
    y1 = sinPhi * sinTheta;
    z1 = cosPhi;

    uint32 pick = currentStream.RandRange(0,16);

    return FVector4(x1*currentRadius, y1*currentRadius, z1*currentRadius, pick);
}

  