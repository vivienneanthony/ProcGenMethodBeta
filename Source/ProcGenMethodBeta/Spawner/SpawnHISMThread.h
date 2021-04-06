// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "../Structures/TraceCall.h"

#include "../Poisson/PoissonSampler.h"

#include "../Enums/WorldTrace.h"
#include "../Enums/PopulateTypes.h"

#include "Engine/Classes/Components/StaticMeshComponent.h"
#include "Engine/Classes/Components/InstancedStaticMeshComponent.h"
#include "Engine/Classes/Components/HierarchicalInstancedStaticMeshComponent.h"

#include "Core/Public/HAL/Runnable.h"

#include "UObject/Object.h"

// include draw debug helpers header file
#include "DrawDebugHelpers.h"

#include "../Formulas/SpawnPointsSphere.h"

// Forward
class FRunnableThread;
class ASpawnHISMActor;


class PROCGENMETHODBETA_API FSpawnHISMThread : public FRunnable
{
	public:
	FSpawnHISMThread(EPopulateTypes inPopulateType, int32 inSeed, bool inbEnablePoisson, EWorldTrace traceType, ASpawnHISMActor * callingActor);

	bool bStopThread;

	// Initialize
	virtual bool Init();

	// Stop
	virtual void Stop();

	// Run
	virtual uint32 Run();

	// Current Parent Actor	
	UPROPERTY()	
	ASpawnHISMActor *currentThreadActor;

private:
	// Static Mesh Component		
	EWorldTrace currentTraceType;

	// start trace
	bool startedtrace;

	// Collision Parameters
	FCollisionQueryParams collisionParams;

	// Delegate
	FTraceDelegate TraceDelegate;

	// Async function
	void AsyncTraceCollisionToPoint();

	// Trace Done
	void TraceDone(const FTraceHandle& TraceHandle, FTraceDatum & TraceData);

	// Choose a coordinate
	FVector ChooseACoordinate();	

	// Out Trace Call
	FTraceCall OutTraceCall;

	// Current PopulateType
	EPopulateTypes	currentPopulateType;

	// Parson
	PoissonSampler poissonSampler;

	// enable poisson calculation
	bool bEnabledPoisson;

	// Keep points in thread
	TQueue<FVector> VectorPoints;

	void PoissonBasedOutput();

	int32 idx;

	int32 currentSeed;

	bool bPoissonGenerated = false;

		
	bool bPoissonGenerating = false;

	float SetRadius = 15000.0f;
	
	float SetInterations = 30.0f;
	
	float SetMinimumDistance = 5000.0f;

	SpawnPointsSphere	spawnPoints;

	uint32 MaximumCount = 0.0f;


	// Get actor location
	FVector currentLocation;
	FVector4 newPoint;
	FVector Start;
	FVector End;

	
};