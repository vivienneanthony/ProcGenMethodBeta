// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

// RuntimeMeshComponent Headers
#include "RuntimeMeshProvider.h"
#include "RuntimeMeshComponentPlugin.h"

#include "RuntimeMeshModifier.h"
#include "Modifiers/RuntimeMeshModifierNormals.h"

// Add Marching cube
#include "../MarchingCube/MeshMarchingCube.h"
#include "../Structures/MeshMarchingCubeParameters.h"

// Custom clas
#include "../Octree/OctreeNode.h"

// Generated file
#include "RuntimeProviderSphereTerrain.generated.h"

class URuntimeMeshModifier;
class URuntimeMeshModifierNormals;

// Class
UCLASS()
class PROCGENMETHODBETA_API URuntimeProviderSphereTerrain : public URuntimeMeshProvider
{
	// Generate Body
	GENERATED_BODY()

public:
	UPROPERTY()
	float SphereRadius;

	UFUNCTION()
	float GetSphereRadius() const;

	UFUNCTION()
	void SetSphereRadius(float InSphereRadius);

	UFUNCTION()
	void SetSphereMaterial(UMaterialInterface *InSphereMaterial);

	UFUNCTION()
	virtual void SetMarchingCubeParameters(FMeshMarchingCubeParameters inParameters);

	UPROPERTY()
	int32 MaxLOD;

	// Marching Cube Parameters
	UPROPERTY()
	FMeshMarchingCubeParameters configMeshMarchingCubeParameters;

	UPROPERTY()
	TArray<UMeshMarchingCube *> MarchingCubePool;

	// Renderable Collision Data
	UPROPERTY()
	TMap<int32, FRuntimeMeshRenderableCollisionData> RenderableCollisionData;

	// Sections Affecting Collision
	UPROPERTY()
	TSet<int32> SectionsAffectingCollision;

	// Collision Mesh
	UPROPERTY()
	FRuntimeMeshCollisionData CollisionMesh;

	// Sections For Collision
	UPROPERTY()
	TSet<int32> SectionsForMeshCollision;

	// Material
	UPROPERTY()
	UMaterialInterface *AutoTerrainMaterial;

	// Array
	UPROPERTY()
	TArray<URuntimeMeshModifier *> CurrentMeshModifiers;

protected:
	// Initialize
	virtual void Initialize() override;

	// Get Bounds
	virtual FBoxSphereBounds GetBounds() override;

	// Is safe
	virtual bool IsThreadSafe() override;

	// Collision - Has to tell Unreal theres a collision
	virtual bool HasCollisionMesh() override;

	// Get collision mesh
	virtual bool GetCollisionMesh(FRuntimeMeshCollisionData &CollisionData) override;

	// Override standard create section
	virtual void CreateSection(int32 LODIndex, int32 SectionId, const FRuntimeMeshSectionProperties &SectionProperties) override;

	// Get section Mesh
	virtual bool GetSectionMeshForLOD(int32 LODIndex, int32 SectionId, FRuntimeMeshRenderableMeshData &MeshData) override;

	virtual bool GenerateSectionData(int32 LODIndex, int32 SectionId, FRuntimeMeshRenderableMeshData &SectionData);

	// Not used
	virtual void SetRenderableSectionAffectsCollision(int32 SectionId, bool bCollisionEnabled);

	// Get collision settings
	virtual FRuntimeMeshCollisionSettings GetCollisionSettings();

	// SectionDataEntry
	using FSectionDataMapEntry = TTuple<FRuntimeMeshSectionProperties, FRuntimeMeshRenderableMeshData, FBoxSphereBounds>;

	// Section Map Data - Map Section and Data
	TMap<int32, FSectionDataMapEntry> SectionDataMap;

	// Root Octree
	OctreeNode rootOctreeNode;

	// Octree Node Sections
	TArray<OctreeNode *> OctreeNodeSections;

private:
	// mutable critical
	mutable FCriticalSection PropertySyncRoot;
	mutable FCriticalSection MeshSyncRoot;
	mutable FCriticalSection CollisionSyncRoot;
	mutable FCriticalSection SyncRoot;

	// mutable locks
	mutable FRWLock ModifierRWLock;
};
