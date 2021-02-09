// Fill out your copyright notice in the Description page of Project Settings.

#include "RuntimeProviderSphereTerrain.h"
#include <algorithm>

// Maximum Depth Octree

// Using LODDOF FOR DETAIL LEVEL PER NODE

#define MAXOCTREENODEDEPTH 3

URuntimeMeshProviderSphere::URuntimeMeshProviderSphere()
    : MaxLOD(0), SphereRadius(20000.0f)
{
    // Set MaxLod to 0
    //MaxLOD = GetMaxNumberOfLODs() - 1;

    //MaxLOD = 10;
}

// Initialize
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
    Properties.bWants32BitIndices = true;
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
        this->CreateSection(0, section, Properties);
    }

    // Mark Collision Dirty
    MarkCollisionDirty();

    
}

// Get section for a specific LOD
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

    bool bTriangleGenerated = false;

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

        if (bTriangleGenerated == false)
        {
            bTriangleGenerated = true;
        }
    }

    // Check mesh data validty - Quick change
    if (bTriangleGenerated == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("Mesh Data %d - No triangle generated "), SectionId);

        // Remove from renderable
        SetRenderableSectionAffectsCollision(SectionId, false);

        return false;
    }

    // Write Log
    UE_LOG(LogTemp, Warning, TEXT("Get Section Mesh Lod - Copied  Vertices %d  Triangles %d"), positions.Num(), triangles.Num());

    bool testMesh = MeshData.HasValidMeshData();

    FString valid = "Valid";
    FString invalid = "Invalid";

    // Check mesh data validty
    if (testMesh == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("Mesh Data %s - Removing from renderable"), *invalid);

        // Remove from renderable
        SetRenderableSectionAffectsCollision(SectionId, false);

        return false;
    }

    // Set this to affect collision
    SetRenderableSectionAffectsCollision(SectionId, true);

    // add data
    if (SectionsAffectingCollision.Contains(SectionId))
    {
        UE_LOG(LogTemp, Warning, TEXT("Mesh Data %d - Section found FruntimeRenderableCollisionData"), SectionId);

        FRuntimeMeshRenderableCollisionData &SectionCacheData = RenderableCollisionData.FindOrAdd(SectionId);
        SectionCacheData = FRuntimeMeshRenderableCollisionData(MeshData);
        MarkCollisionDirty();
    }

    // If Data is valid
   /* if (MeshData.HasValidMeshData())
    {
        for (int32 Index = 0; Index < CurrentMeshModifiers.Num(); Index++)
        {
            if (CurrentMeshModifiers[Index])
            {
                CurrentMeshModifiers[Index]->ApplyToMesh(MeshData);
            }
        }
    }*/

    return true;
}

// Get Bounds
FBoxSphereBounds URuntimeProviderSphereTerrain::GetBounds()
{
    return FBoxSphereBounds(FBox(FVector(-SphereRadius, -SphereRadius, -SphereRadius), FVector(SphereRadius, SphereRadius, SphereRadius)));
}

// Get radius
float URuntimeProviderSphereTerrain::GetSphereRadius() const
{
    FScopeLock Lock(&PropertySyncRoot);
    return SphereRadius;
}

// Set Radius
void URuntimeProviderSphereTerrain::SetSphereRadius(float InSphereRadius)
{
    FScopeLock Lock(&PropertySyncRoot);
    SphereRadius = InSphereRadius;
}

// Check if thread safe
bool URuntimeProviderSphereTerrain::IsThreadSafe()
{
    return true;
}

// Set Marching Cube Parameters
void URuntimeProviderSphereTerrain::SetMarchingCubeParameters(FMeshMarchingCubeParameters inParameters)
{
    FScopeLock Lock(&PropertySyncRoot);
    configMeshMarchingCubeParameters = inParameters;
}

// Set Sphere Material
void URuntimeProviderSphereTerrain::SetSphereMaterial(UMaterialInterface *InSphereMaterial)
{
    FScopeLock Lock(&PropertySyncRoot);
    AutoTerrainMaterial = InSphereMaterial;
    this->SetupMaterialSlot(0, FName("Auto Terrain Material"), AutoTerrainMaterial);
}

// Check has collision mesh
bool URuntimeProviderSphereTerrain::HasCollisionMesh()
{
    FScopeLock Lock(&SyncRoot);
    return (CollisionMesh.Vertices.Num() > 0 && CollisionMesh.Triangles.Num() > 0) ||
           RenderableCollisionData.Num() > 0;

    return true;
}

// Get collision settings
FRuntimeMeshCollisionSettings URuntimeProviderSphereTerrain::GetCollisionSettings()
{
    // Create a colision settings
    FRuntimeMeshCollisionSettings Settings;
    Settings.bUseAsyncCooking = true;
    Settings.bUseComplexAsSimple = true;

    return Settings;
}

// Get Collisoin Mesh
bool URuntimeProviderSphereTerrain::GetCollisionMesh(FRuntimeMeshCollisionData &CollisionData)
{
    FScopeLock Lock(&SyncRoot);

    //If the given collision mesh is valid, use it
    if (CollisionMesh.Vertices.Num() > 0 && CollisionMesh.Triangles.Num() > 0)
    {
        CollisionData = CollisionMesh;
        return true;
    }

    for (const auto &CachedSectionEntry : RenderableCollisionData)
    {
        int32 SectionId = CachedSectionEntry.Key;
        const FRuntimeMeshRenderableCollisionData &CachedSection = CachedSectionEntry.Value;

        // Log
        UE_LOG(LogTemp, Warning, TEXT("GetCollisionMesh - Add section %d"), SectionId);

        int32 FirstVertex = CollisionData.Vertices.Num();

        // Copy the vertices
        int32 NumVertices = CachedSection.Vertices.Num();
        CollisionData.Vertices.SetNum(FirstVertex + NumVertices);
        for (int32 Index = 0; Index < NumVertices; Index++)
        {
            CollisionData.Vertices.SetPosition(FirstVertex + Index, CachedSection.Vertices.GetPosition(Index));
        }

        // Copy tex coords
        int32 MaxTexCoordChannels = FMath::Max(CollisionData.TexCoords.NumChannels(), CachedSection.TexCoords.NumChannels());
        CollisionData.TexCoords.SetNum(MaxTexCoordChannels, FirstVertex + NumVertices);
        for (int32 Index = 0; Index < NumVertices; Index++)
        {
            for (int32 ChannelId = 0; ChannelId < MaxTexCoordChannels; ChannelId++)
            {
                if (ChannelId < CachedSection.TexCoords.NumChannels() && CachedSection.TexCoords.NumTexCoords(ChannelId) > Index)
                {
                    FVector2D TexCoord = CachedSection.TexCoords.GetTexCoord(ChannelId, Index);
                    CollisionData.TexCoords.SetTexCoord(ChannelId, Index, TexCoord);
                }
                else
                {
                    CollisionData.TexCoords.SetTexCoord(ChannelId, Index, FVector2D::ZeroVector);
                }
            }
        }

        // Copy triangles and fill in material indices
        int32 StartTriangle = CollisionData.Triangles.Num();
        int32 NumTriangles = CachedSection.Triangles.Num();
        CollisionData.Triangles.SetNum(StartTriangle + NumTriangles);
        CollisionData.MaterialIndices.SetNum(StartTriangle + NumTriangles);
        for (int32 Index = 0; Index < NumTriangles; Index++)
        {
            int32 IdA, IdB, IdC;
            CachedSection.Triangles.GetTriangleIndices(Index, IdA, IdB, IdC);

            CollisionData.Triangles.SetTriangleIndices(StartTriangle + Index, IdA + FirstVertex, IdB + FirstVertex, IdC + FirstVertex);
            CollisionData.MaterialIndices.SetMaterialIndex(StartTriangle + Index, SectionId);
        }

        CollisionData.CollisionSources.Emplace(StartTriangle, StartTriangle + NumTriangles - 1, this, SectionId, ERuntimeMeshCollisionFaceSourceType::Renderable);

        // Log Emplace
        UE_LOG(LogTemp, Warning, TEXT("CollisionData.CollisionSources.Emplace - Triangle Start %d  NumTriangles %d SectionId %d"), StartTriangle, NumTriangles, SectionId);
    }
    return true;
}

// Set RenderableSectionAffectCollision
void URuntimeProviderSphereTerrain::SetRenderableSectionAffectsCollision(int32 SectionId, bool bCollisionEnabled)
{
    bool bShouldMarkCollisionDirty = false;
    {
        FScopeLock Lock(&SyncRoot);
        if (bCollisionEnabled && !SectionsAffectingCollision.Contains(SectionId))
        {
            UE_LOG(LogTemp, Warning, TEXT("Mesh Data %d - Adding to sections renderable"), SectionId);

            SectionsAffectingCollision.Add(SectionId);
            bShouldMarkCollisionDirty = true;
        }
        else if (!bCollisionEnabled && SectionsAffectingCollision.Contains(SectionId))
        {

            UE_LOG(LogTemp, Warning, TEXT("Mesh Data %d - Removing from sections affecting and renderable"), SectionId);

            SectionsAffectingCollision.Remove(SectionId);
            RenderableCollisionData.Remove(SectionId);
            bShouldMarkCollisionDirty = true;
        }
    }

    if (bShouldMarkCollisionDirty)
    {
        MarkCollisionDirty();
    }
}

// Replacement Create Section Is Called
void URuntimeProviderSphereTerrain::CreateSection(int32 LODIndex, int32 SectionId, const FRuntimeMeshSectionProperties &SectionProperties)
{
    // Call super create section
    Super::CreateSection(LODIndex, SectionId, SectionProperties);
}