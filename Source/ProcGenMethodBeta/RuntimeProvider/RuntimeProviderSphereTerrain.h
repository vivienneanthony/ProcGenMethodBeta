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

private:
	mutable FCriticalSection PropertySyncRoot;
	mutable FRWLock ModifierRWLock;

	// Array
	TArray<URuntimeMeshModifier *> CurrentMeshModifiers;

	// Root Octree
	OctreeNode rootOctreeNode;

	// Octree Node Sections
	TArray<OctreeNode *> OctreeNodeSections;

	// Material
	UMaterialInterface *AutoTerrainMaterial;
};
