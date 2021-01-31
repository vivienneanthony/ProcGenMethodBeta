// Fill out your copyright notice in the Description page of Project Settings.

#include "RuntimeProviderSphereTerrain.h"

URuntimeMeshProviderSphere::URuntimeMeshProviderSphere()
    : MaxLOD(0), SphereRadius(20000.0f)
{
    MaxLOD = GetMaxNumberOfLODs() - 1;

    // Set Octree to half the radius or full depending on how to set the bounds
    // Maxlod of octree can be set to MaxLOD
}

void URuntimeProviderSphereTerrain::Initialize()
{
    // Not adding a new object
    FWriteScopeLock Lock(ModifierRWLock);
    CurrentMeshModifiers.Add(NewObject<URuntimeMeshModifierNormals>(this, "RuntimeMeshModifierNormals"));

    // Create Runtime Mesh LOD Properties
    FRuntimeMeshLODProperties LODProperties;

    // Set screen size
    LODProperties.ScreenSize = 0.0f;

    // This configure LOD properties
    ConfigureLODs({LODProperties});

    // Set Material
    SetupMaterialSlot(0, FName("Auto Terrain Material"), AutoTerrainMaterial);

    // Setup regular properties
    FRuntimeMeshSectionProperties Properties;
    Properties.bCastsShadow = true;
    Properties.bIsVisible = true;
    Properties.MaterialSlot = 0;
    Properties.UpdateFrequency = ERuntimeMeshUpdateFrequency::Infrequent;

    // CreateDefault Section
    CreateSection(0, 0, Properties);

    // Mark collision dirty not used
    MarkCollisionDirty();
}

// Get section would change for getsection based on some value
bool URuntimeProviderSphereTerrain::GetSectionMeshForLOD(int32 LODIndex, int32 SectionId, FRuntimeMeshRenderableMeshData &MeshData)
{
    // We should only ever be queried for section 0 and lod 0
    check(SectionId == 0 && LODIndex == 0);

    // Core
    ptrMarchingCube = NewObject<UMeshMarchingCube>(this, TEXT("MeshMarchingCube"));

    // Write Long if marching cube can't be created
    if (!ptrMarchingCube)
    {
        UE_LOG(LogTemp, Warning, TEXT("Get Section Mesh For LOD FaiL - Could not create Marching Cube Object"));

        return false;
    }

    UE_LOG(LogTemp, Warning, TEXT("Get Section Mesh Lod - Set Parameters"));
    ptrMarchingCube->SetParameters(configMeshMarchingCubeParameters);

    // Write Log
    UE_LOG(LogTemp, Warning, TEXT("Get Section Mesh Lod - Initialize Noise Grid Data"));

    // Initialize
    ptrMarchingCube->InitializeNoiseGridData();

    // Write Log
    UE_LOG(LogTemp, Warning, TEXT("Get Section Mesh Lod - Polygonization"));

    // create polygons
    ptrMarchingCube->Polygonization();

    // Write Log
    UE_LOG(LogTemp, Warning, TEXT("Get Section Mesh Lod - Copy to MeshData (Could be a memory move)"));

    // Get data from polygonization
    TArray<FVector> positions = ptrMarchingCube->GetVerticesData();
    TArray<int32> triangles = ptrMarchingCube->GetTrianglesData();

    for (uint32 i = 0; i < positions.Num(); i++)
    {
        MeshData.Positions.Add(positions[i]);
    }

    for (uint32 i = 0; i < triangles.Num(); i += 3)
    {
        MeshData.Triangles.AddTriangle(triangles[i], triangles[i + 1], triangles[i + 2]);
    }

    // Write Log
    UE_LOG(LogTemp, Warning, TEXT("Provider Vertices %d  Triangles %d"), MeshData.Positions.Num(), MeshData.Triangles.Num());

    FReadScopeLock Lock(ModifierRWLock);

    for (URuntimeMeshModifier* Modifier : CurrentMeshModifiers)
	{
		
			Modifier->ApplyToMesh(MeshData);
	}

    return true;
}

FBoxSphereBounds URuntimeProviderSphereTerrain::GetBounds()
{
    return FBoxSphereBounds(FSphere(FVector::ZeroVector, SphereRadius));
}

float URuntimeProviderSphereTerrain::GetSphereRadius() const
{
    FScopeLock Lock(&PropertySyncRoot);
    return SphereRadius;
}

void URuntimeProviderSphereTerrain::SetSphereRadius(float InSphereRadius)
{
    FScopeLock Lock(&PropertySyncRoot);
    SphereRadius = InSphereRadius;
}

bool URuntimeProviderSphereTerrain::IsThreadSafe()
{
    return true;
}

void URuntimeProviderSphereTerrain::SetMarchingCubeParameters(FMeshMarchingCubeParameters inParameters)
{
    FScopeLock Lock(&PropertySyncRoot);
    configMeshMarchingCubeParameters = inParameters;
}

void URuntimeProviderSphereTerrain::SetSphereMaterial(UMaterialInterface *InSphereMaterial)
{
    FScopeLock Lock(&PropertySyncRoot);
    AutoTerrainMaterial = InSphereMaterial;
    this->SetupMaterialSlot(0, FName("Auto Terrain Material"), AutoTerrainMaterial);
}