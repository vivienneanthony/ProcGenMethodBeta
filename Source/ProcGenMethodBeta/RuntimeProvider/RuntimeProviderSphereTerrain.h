// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

// RuntimeMeshComponent Headers
#include "RuntimeMeshProvider.h"
#include "RuntimeMeshModifier.h"
#include "RuntimeMeshComponentPlugin.h"

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

	UFUNCTION(Category = "RuntimeMesh|Providers|Sphere", BlueprintCallable)
	void SetSphereMaterial(UMaterialInterface *InSphereMaterial);

	// Set Parameters
	virtual void SetMarchingCubeParameters(FMeshMarchingCubeParameters inParameters);

protected:
	// Initialize
	virtual void Initialize() override;

	// Get Bounds
	virtual FBoxSphereBounds GetBounds() override;

	// Is safe
	virtual bool IsThreadSafe() override;

	// Collision - Has to tell Unreal theres a collision
	bool HasCollisionMesh() override;

	// Get collision mesh
	bool GetCollisionMesh(FRuntimeMeshCollisionData &CollisionData) override;

	// Override standard create section
	virtual void CreateSection(int32 LODIndex, int32 SectionId, const FRuntimeMeshSectionProperties &SectionProperties) override;

	// Get section Mesh
	virtual bool GetSectionMeshForLOD(int32 LODIndex, int32 SectionId, FRuntimeMeshRenderableMeshData &MeshData) override;

	bool GenerateSectionData(int32 LODIndex, int32 SectionId, FRuntimeMeshRenderableMeshData &SectionData);

	// Not used
	void SetRenderableSectionAffectsCollision(int32 SectionId, bool bCollisionEnabled);

	// Get collision settings
	FRuntimeMeshCollisionSettings GetCollisionSettings();

	// Marching Cube Parameters
	FMeshMarchingCubeParameters configMeshMarchingCubeParameters;

	// Renderable Collision Data
	TMap<int32, FRuntimeMeshRenderableCollisionData> RenderableCollisionData;

	// Sections Affecting Collision
	TSet<int32> SectionsAffectingCollision;

	// Collision Mesh
	FRuntimeMeshCollisionData CollisionMesh;

	// Null pointer marching cube
	//UMeshMarchingCube *ptrMarchingCube = nullptr;

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

	// Section Map Data - Map Section and Data
	TMap<int32, FSectionDataMapEntry> SectionDataMap;

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
};
