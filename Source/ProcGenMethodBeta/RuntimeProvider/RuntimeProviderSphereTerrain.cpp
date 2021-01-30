// Fill out your copyright notice in the Description page of Project Settings.

#include "RuntimeProviderSphereTerrain.h"

URuntimeMeshProviderSphere::URuntimeMeshProviderSphere()
    : MaxLOD(0), SphereRadius(20000.0f)
{
    MaxLOD = GetMaxNumberOfLODs() - 1;
}

void URuntimeProviderSphereTerrain::Initialize()
{
    // Create Runtime Mesh LOD Properties
    FRuntimeMeshLODProperties LODProperties;

    // Set screen size
    LODProperties.ScreenSize = 0.0f;

    // This configure LOD properties
    ConfigureLODs({LODProperties});

    //SetupMaterialSlot(0, FName("AutoTerrain Material"), GetMaterial());

    // Setup regular properties
    FRuntimeMeshSectionProperties Properties;
    Properties.bCastsShadow = true;
    Properties.bIsVisible = true;
    Properties.MaterialSlot = 0;
    Properties.UpdateFrequency = ERuntimeMeshUpdateFrequency::Infrequent;

    // CreateDefault Section
    CreateSection(0, 0, Properties);

    MarkCollisionDirty();
}

// get sction
bool URuntimeProviderSphereTerrain::GetSectionMeshForLOD(int32 LODIndex, int32 SectionId, FRuntimeMeshRenderableMeshData &MeshData)
{
    // We should only ever be queried for section 0 and lod 0
    check(SectionId == 0 && LODIndex == 0);

    if (!ptrMarchingCube)
    {
        UE_LOG(LogTemp, Warning, TEXT("Get Section Mesh For LOD FAIL - no marching cube set"));

        return false;
    }

    // Core
    ptrMarchingCube = NewObject<UMeshMarchingCube>(this, TEXT("MeshMarchingCube"));

    // Initialize
    ptrMarchingCube->InitializeNoiseGridData();

    // create polygons
    ptrMarchingCube->Polygonization();

    // Get datafrom polygonization
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
    //UpdateMeshParameters(true); - Not used
}

bool URuntimeProviderSphereTerrain::IsThreadSafe()
{
    return true;
}