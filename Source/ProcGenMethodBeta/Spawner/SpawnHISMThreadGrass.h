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


class PROCGENMETHODBETA_API FSpawnHISMThreadGrass : public FRunnable
{
	public:
	FSpawnHISMThreadGrass(float inRadius, ASpawnHISMActor * callingActor);

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
	// Parson
	SpawnPointsSphere spawnPoints;

	// set spawnpoints to false
	bool spawningPoints;
	uint32 spawnCounter;

	float currentRadius;

	FVector currentVector;
};