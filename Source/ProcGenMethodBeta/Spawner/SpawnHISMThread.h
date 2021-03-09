// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "../Structures/TraceCall.h"

#include "../Enums/WorldTrace.h"
#include "../Enums/PopulateTypes.h"

#include "Engine/Classes/Components/StaticMeshComponent.h"
#include "Engine/Classes/Components/InstancedStaticMeshComponent.h"
#include "Engine/Classes/Components/HierarchicalInstancedStaticMeshComponent.h"

#include "Core/Public/HAL/Runnable.h"

#include "UObject/Object.h"

// include draw debug helpers header file
#include "DrawDebugHelpers.h"

// Forward
class FRunnableThread;
class ASpawnHISMActor;


class PROCGENMETHODBETA_API FSpawnHISMThread : public FRunnable
{
	public:
	FSpawnHISMThread(EPopulateTypes inPopulateType, EWorldTrace traceType, ASpawnHISMActor * callingActor);

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

	// Save Start Trace
	FVector Start;

	// Save End Trace
	FVector End;

	// Out Trace Call
	FTraceCall OutTraceCall;

	EPopulateTypes	currentPopulateType;
};