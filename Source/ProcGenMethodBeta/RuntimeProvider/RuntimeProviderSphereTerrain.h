// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

// RuntimeMeshComponent Headers
#include "RuntimeMeshProvider.h"
#include "RuntimeMeshModifier.h"

#include "Modifiers/RuntimeMeshModifierNormals.h"

// Add Marching cube
#include "../MarchingCube/MeshMarchingCube.h"
#include "../Structures/MeshMarchingCubeParameters.h"

#include "../Octree/OctreeNode.h"

// Generated file
#include "RuntimeProviderSphereTerrain.generated.h"

class URuntimeMeshModifier;

// Class
UCLASS()
class PROCGENMETHODBETA_API URuntimeProviderSphereTerrain : public URuntimeMeshProvider
{
	// Generate Body
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 MaxLOD;

	UPROPERTY(Category = "RuntimeMesh|Providers|Sphere", VisibleAnywhere, BlueprintGetter = GetSphereRadius, BlueprintSetter = SetSphereRadius)
	float SphereRadius;

	UFUNCTION(Category = "RuntimeMesh|Providers|Sphere", BlueprintCallable)
	float GetSphereRadius() const;

	UFUNCTION(Category = "RuntimeMesh|Providers|Sphere", BlueprintCallable)
	void SetSphereRadius(float InSphereRadius);

	// Set Parameters
	virtual void SetMarchingCubeParameters(FMeshMarchingCubeParameters inParameters);

	UFUNCTION(Category = "RuntimeMesh|Providers|Sphere", BlueprintCallable)
	void SetSphereMaterial(UMaterialInterface *InSphereMaterial);

	UPROPERTY()
	UMeshMarchingCube *ptrMarchingCube = nullptr;

	// Override standard create section
	virtual void CreateSection(int32 LODIndex, int32 SectionId, const FRuntimeMeshSectionProperties &SectionProperties) override;

	UFUNCTION(Category = "RuntimeMesh|Providers|Collision", BlueprintCallable)
	void SetRenderableSectionAffectsCollision(int32 SectionId, bool bCollisionEnabled);

protected:
	// Initialize
	virtual void Initialize() override;

	// Get Bounds
	virtual FBoxSphereBounds GetBounds() override;

	// Is safe
	virtual bool IsThreadSafe() override;

	// Get section Mesh
	virtual bool GetSectionMeshForLOD(int32 LODIndex, int32 SectionId, FRuntimeMeshRenderableMeshData &MeshData) override;

	// Marching Cube Parameters
	FMeshMarchingCubeParameters configMeshMarchingCubeParameters;

	// Collision - Has to tell Unreal theres a collision
	bool HasCollisionMesh() override;

	// Get collision mesh
	bool GetCollisionMesh(FRuntimeMeshCollisionData &CollisionData) override;

	// Get collision settings
	FRuntimeMeshCollisionSettings GetCollisionSettings();

	TMap<int32, FRuntimeMeshRenderableCollisionData> RenderableCollisionData;

	TSet<int32> SectionsAffectingCollision;

	FRuntimeMeshCollisionData CollisionMesh;

private:
	// mutable critical
	mutable FCriticalSection PropertySyncRoot;

	mutable FCriticalSection MeshSyncRoot;
	mutable FCriticalSection CollisionSyncRoot;
	mutable FCriticalSection SyncRoot;

	// mutable locks
	mutable FRWLock ModifierRWLock;

	// SectionDataEntry
	using FSectionDataMapEntry = TTuple<FRuntimeMeshSectionProperties, FRuntimeMeshRenderableMeshData, FBoxSphereBounds>;

	// Section Map Data
	TMap<int32, TMap<int32, FSectionDataMapEntry>> SectionDataMap;

	// Array
	TArray<URuntimeMeshModifier *> CurrentMeshModifiers;

	// Sections For Collision
	TSet<int32> SectionsForMeshCollision;

	// Root Octree
	OctreeNode rootOctreeNode;

	// Octree Node Sections
	TArray<OctreeNode *> OctreeNodeSections;

	// Material
	UMaterialInterface *AutoTerrainMaterial;

	// Create Section
	void CreateSection(int32 LODIndex, int32 SectionId, const FRuntimeMeshSectionProperties &SectionProperties, FRuntimeMeshRenderableMeshData &&SectionData, bool bCreateCollision = true)
	{
		// This creates a section
		CreateSection(LODIndex, SectionId, SectionProperties);

		// not sure
		//UpdateSectionInternal(LODIndex, SectionId, MoveTemp(SectionData), GetBoundsFromMeshData(SectionData));

		//UpdateSectionAffectsCollision(LODIndex, SectionId, bCreateCollision);
	}

	// Update Section Internal
	void UpdateSectionInternal(int32 LODIndex, int32 SectionId, FRuntimeMeshRenderableMeshData &&SectionData, FBoxSphereBounds KnownBounds);
};
