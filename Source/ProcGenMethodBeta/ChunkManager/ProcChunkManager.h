// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

// RuntimeMeshComponent Headers
#include "RuntimeMeshProvider.h"
#include "RuntimeMeshComponentPlugin.h"

#include "RuntimeMeshModifier.h"

#include "../Marker/TerrainMarker.h"

#include "../RuntimeProvider/RuntimeProviderSphereTerrain.h"

#include "../Octree/OctreeNode.h"

#include "ProcChunkManager.generated.h"

class URuntimeProviderSphereTerrain;

UCLASS()
class PROCGENMETHODBETA_API UProcChunkManager : public UObject
{
	GENERATED_BODY()
	
public:			
	UPROPERTY()
	bool isInitialized = false;
	
	UPROPERTY(Instanced)			
	URuntimeProviderSphereTerrain * parentRuntimeProvider;
	
	UPROPERTY()
	AActor * parentActor;

public:
	// Initialize
	void Initialize();

	// Set References
	bool SetReferences(URuntimeProviderSphereTerrain * inRuntimeProvider, AActor * inActor);

	// Get region by using terrain marker
	void GenerateTerrainByRegion(TArray<ATerrainMarker *> inTerrainMarkers);
	
protected:	
		
	// All generated ndoes
	OctreeNode * rootOctreeNode = nullptr;

	// Octree Node Sections	
	TArray<OctreeNode *> OctreeNodeSections;	

};
