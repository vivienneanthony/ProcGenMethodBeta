// Fill out your copyright notice in the Description page of Project Settings.

#include "RuntimeProviderSphereTerrain.h"
#include <algorithm>

// Maximum Depth Octree

// Using LODDOF FOR DETAIL LEVEL PER NODE

#define MAXOCTREENODEDEPTH 2

URuntimeMeshProviderSphere::URuntimeMeshProviderSphere()
    : MaxLOD(0), SphereRadius(20000.0f)
{
    MaxLOD = GetMaxNumberOfLODs() - 1;

    // Set Octree to half the radius or full depending on how to set the bounds
    // Maxlod of octree can be set to MaxLOD
    MaxLOD = 10;
}

void URuntimeProviderSphereTerrain::Initialize()
{

    // Core
    ptrMarchingCube = NewObject<UMeshMarchingCube>(this, TEXT("MeshMarchingCube"));

    // Write Long if marching cube can't be created
    if (!ptrMarchingCube)
    {
        UE_LOG(LogTemp, Warning, TEXT("Initialize - Generate Marching Cube"));

        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("Initialize - Set Parameters"));
    ptrMarchingCube->SetParameters(configMeshMarchingCubeParameters);

    // Write Log
    UE_LOG(LogTemp, Warning, TEXT("Initialize - Initialize Noise Grid Data"));

    // Initialize
    ptrMarchingCube->InitializeNoiseGridData();

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

    // Generate Octree
    rootOctreeNode.BoundRegion.Min = Vect3(-20000.0f, -20000.0f, -20000.0f);
    rootOctreeNode.BoundRegion.Max = Vect3(20000.0f, 20000.0f, 20000.0f);

    // Generate Tree Base On Max Lod
    rootOctreeNode.BuildTree(MAXOCTREENODEDEPTH);

    // Return list of nodes
    rootOctreeNode.GetAllNodesAtDepth(MAXOCTREENODEDEPTH, OctreeNodeSections);

    // Log
    UE_LOG(LogTemp, Warning, TEXT("Sections %d"), OctreeNodeSections.Num());

    // create all sections  // This should get all sections
    for (unsigned int section = 0; section < OctreeNodeSections.Num(); section++)
    {
        // CreateDefault Section
        CreateSection(0, section, Properties);
    }

    // Mark collision dirty not used
    MarkCollisionDirty();
}

// Get section would change for getsection based on some value
bool URuntimeProviderSphereTerrain::GetSectionMeshForLOD(int32 LODIndex, int32 SectionId, FRuntimeMeshRenderableMeshData &MeshData)
{
    // Write Long if marching cube can't be created
    if (!ptrMarchingCube)
    {
        UE_LOG(LogTemp, Warning, TEXT("Get Section Mesh For LOD FaiL - Could not create Marching Cube Object"));

        return false;
    }

    // Write Log
    UE_LOG(LogTemp, Warning, TEXT("Get Section Mesh Lod - Polygonization"));

    // Copy region
    FVector polygonizationBoundRegionMin = FVector(OctreeNodeSections[SectionId]->BoundRegion.Min.x, OctreeNodeSections[SectionId]->BoundRegion.Min.y, OctreeNodeSections[SectionId]->BoundRegion.Min.z);
    FVector polygonizationBoundRegionMax = FVector(OctreeNodeSections[SectionId]->BoundRegion.Max.x, OctreeNodeSections[SectionId]->BoundRegion.Max.y, OctreeNodeSections[SectionId]->BoundRegion.Max.z);

    // Get data from polygonization
    TArray<FVector> positions;
    TArray<int32> triangles;

    // create polygons
    UE_LOG(LogTemp, Warning, TEXT("Get Section Mesh Lod - Generating for bound  Min %s  Max %s"), *polygonizationBoundRegionMin.ToString(), *polygonizationBoundRegionMax.ToString());

    ptrMarchingCube->PolygonizationV2(polygonizationBoundRegionMin, polygonizationBoundRegionMax, positions, triangles);

    // Write Log
    UE_LOG(LogTemp, Warning, TEXT("Get Section Mesh Lod - Copy to MeshData (Could be a memory move)"));

    auto AddVertex = [&](const FVector &InPosition, const FVector &InTangentX, const FVector &InTangentZ, const FVector2D &InTexCoord) {
        MeshData.Positions.Add(InPosition);
        MeshData.Tangents.Add(InTangentZ, InTangentX);
        MeshData.Colors.Add(FColor::White);
        MeshData.TexCoords.Add(InTexCoord);
    };

    FVector Tangent;
    FVector2D TextCoord;

    for (uint32 i = 0; i < positions.Num(); i++)
    {
        AddVertex(positions[i], Tangent, Tangent, TextCoord);
    }

    for (uint32 i = 0; i < triangles.Num(); i += 3)
    {
        MeshData.Triangles.AddTriangle(triangles[i], triangles[i + 1], triangles[i + 2]);
    }

    // Write Log
    UE_LOG(LogTemp, Warning, TEXT("Get Section Mesh Lod - Copied  Vertices %d  Triangles %d"), positions.Num(), triangles.Num());

    bool testMesh = MeshData.HasValidMeshData();

    FString valid = "Valid";
    FString invalid = "Invalid";

    // Check mesh data validty
    if (testMesh == true)
    {
        UE_LOG(LogTemp, Warning, TEXT("Mesh Data %s"), *valid);

        // FReadScopeLock Lock(ModifierRWLock);

        // for (int i = 0; i < CurrentMeshModifiers.Num(); i++)
        //{
        //    if (CurrentMeshModifiers[i])
        //   {
        //       CurrentMeshModifiers[i]->ApplyToMesh(MeshData);
        //     }
        //}//
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Mesh Data %s"), *invalid);
        return false;
    }

    return true;
}

FBoxSphereBounds URuntimeProviderSphereTerrain::GetBounds()
{
    return FBoxSphereBounds(FBox(FVector(-SphereRadius,-SphereRadius,-SphereRadius), FVector(SphereRadius,SphereRadius,SphereRadius)));
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