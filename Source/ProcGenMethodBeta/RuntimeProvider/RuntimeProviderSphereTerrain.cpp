// Fill out your copyright notice in the Description page of Project Settings.

#include "RuntimeProviderSphereTerrain.h"
#include <algorithm>

// Maximum Depth Octree

// Using LODDOF FOR DETAIL LEVEL PER NODE

#define MAXOCTREENODEDEPTH 3

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
        this->CreateSection(0, section, Properties);
    }

    // Mark collision dirty not used
    // If this is correct this should tell runtime to get the collision mesh
    MarkCollisionDirty();
}

// Get section would change for getsection based on some value
bool URuntimeProviderSphereTerrain::GetSectionMeshForLOD(int32 LODIndex, int32 SectionId, FRuntimeMeshRenderableMeshData &MeshData)
{
    // We should only ever be queried for section 0 and lod 0
    //check(SectionId == 0 && LODIndex == 0);

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

        //FReadScopeLock Lock(ModifierRWLock);
        // for (int i = 0; i < CurrentMeshModifiers.Num(); i++)
        //{
        //     if (CurrentMeshModifiers[i])
        //    {
        //        CurrentMeshModifiers[i]->ApplyToMesh(MeshData);
        //    }
        //}
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
    return FBoxSphereBounds(FBox(FVector(-SphereRadius, -SphereRadius, -SphereRadius), FVector(SphereRadius, SphereRadius, SphereRadius)));
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

bool URuntimeProviderSphereTerrain::HasCollisionMesh()
{

    int32 LODForMeshCollisionTemp = 0;
    TSet<int32> SectionsForMeshCollisionTemp;

    {
        FScopeLock Lock(&CollisionSyncRoot);

        // Temporary
        LODForMeshCollisionTemp = 0;
        SectionsForMeshCollisionTemp = SectionsForMeshCollision;
    }

    {
        FScopeLock Lock(&MeshSyncRoot);
        TMap<int32, FSectionDataMapEntry> *LODSections = SectionDataMap.Find(LODForMeshCollisionTemp);
        if (LODSections)
        {
            for (int32 SectionId : SectionsForMeshCollisionTemp)
            {
                FSectionDataMapEntry *Section = LODSections->Find(SectionId);
                if (Section)
                {
                    if (Section->Get<1>().HasValidMeshData())
                    {
                        return true;
                    }
                }
            }
        }
    }

    return true;
}

// true true
FRuntimeMeshCollisionSettings URuntimeProviderSphereTerrain::GetCollisionSettings()
{
    // Create a colision settings
    FRuntimeMeshCollisionSettings Settings;
    Settings.bUseAsyncCooking = false;
    Settings.bUseComplexAsSimple = false;

    return Settings;
}

bool URuntimeProviderSphereTerrain::GetCollisionMesh(FRuntimeMeshCollisionData &CollisionData)
{
    // Test Loaded on Initial
    //UE_LOG(LogTemp, Warning, TEXT("Get collision mesh called %d"), 1);

    int32 LODForMeshCollisionTemp = INDEX_NONE;
    TSet<int32> SectionsForMeshCollisionTemp;
    bool bHadMeshData = false;

    FScopeLock Lock(&MeshSyncRoot);
    TMap<int32, FSectionDataMapEntry> *LODSections = SectionDataMap.Find(LODForMeshCollisionTemp);
    if (LODSections)
    {
        for (int32 SectionId : SectionsForMeshCollisionTemp)
        {
            FSectionDataMapEntry *Section = LODSections->Find(SectionId);
            if (Section)
            {
                FRuntimeMeshRenderableMeshData &SectionData = Section->Get<1>();
                if (SectionData.HasValidMeshData())
                {
                    int32 FirstVertex = CollisionData.Vertices.Num();
                    int32 NumVertex = SectionData.Positions.Num();
                    int32 NumTexCoords = SectionData.TexCoords.Num();
                    int32 NumChannels = SectionData.TexCoords.NumChannels();
                    CollisionData.Vertices.SetNum(FirstVertex + NumVertex, false);
                    CollisionData.TexCoords.SetNum(NumChannels, FirstVertex + NumVertex, false);
                    for (int32 VertIdx = 0; VertIdx < NumVertex; VertIdx++)
                    {
                        CollisionData.Vertices.SetPosition(FirstVertex + VertIdx, SectionData.Positions.GetPosition(VertIdx));
                        if (VertIdx >= NumTexCoords)
                        {
                            continue;
                        }
                        for (int32 ChannelIdx = 0; ChannelIdx < NumChannels; ChannelIdx++)
                        {
                            CollisionData.TexCoords.SetTexCoord(ChannelIdx, FirstVertex + VertIdx, SectionData.TexCoords.GetTexCoord(VertIdx, ChannelIdx));
                        }
                    }

                    int32 FirstTris = CollisionData.Triangles.Num();
                    int32 NumTriangles = SectionData.Triangles.NumTriangles();
                    CollisionData.Triangles.SetNum(FirstTris + NumTriangles, false);
                    CollisionData.MaterialIndices.SetNum(FirstTris + NumTriangles, false);
                    for (int32 TrisIdx = 0; TrisIdx < NumTriangles; TrisIdx++)
                    {
                        int32 Index0 = SectionData.Triangles.GetVertexIndex(TrisIdx * 3) + FirstVertex;
                        int32 Index1 = SectionData.Triangles.GetVertexIndex(TrisIdx * 3 + 1) + FirstVertex;
                        int32 Index2 = SectionData.Triangles.GetVertexIndex(TrisIdx * 3 + 2) + FirstVertex;

                        CollisionData.Triangles.SetTriangleIndices(TrisIdx + FirstTris, Index0, Index1, Index2);
                        CollisionData.MaterialIndices.SetMaterialIndex(TrisIdx + FirstTris, Section->Get<0>().MaterialSlot);
                    }

                    CollisionData.CollisionSources.Emplace(FirstTris, CollisionData.Triangles.Num() - 1, this, SectionId, ERuntimeMeshCollisionFaceSourceType::Renderable);
                    bHadMeshData = true;
                }
            }
        }
    }

    return bHadMeshData;
}

void URuntimeProviderSphereTerrain::UpdateSectionInternal(int32 LODIndex, int32 SectionId, FRuntimeMeshRenderableMeshData &&SectionData, FBoxSphereBounds KnownBounds)
{
    // This is just to alert the user of invalid mesh data
    SectionData.HasValidMeshData(true);

    {
        FReadScopeLock Lock(ModifierRWLock);

        for (URuntimeMeshModifier *Modifier : CurrentMeshModifiers)
        {
            check(Modifier->IsValidLowLevel());

            Modifier->ApplyToMesh(SectionData);
        }
    }

    {
        FScopeLock Lock(&MeshSyncRoot);
        TMap<int32, FSectionDataMapEntry> *LODSections = SectionDataMap.Find(LODIndex);
        if (LODSections)
        {
            FSectionDataMapEntry *Section = LODSections->Find(SectionId);
            if (Section)
            {
                (*Section) = MakeTuple(Section->Get<0>(), SectionData, KnownBounds);

                //UpdateBounds();
                MarkSectionDirty(LODIndex, SectionId);
            }
        }
    }

    {
        FScopeLock Lock(&CollisionSyncRoot);
        if ((LODIndex = 0) && SectionsForMeshCollision.Contains(SectionId))
        {
            MarkCollisionDirty();
        }
    }
}

// Replacement Create Section Is Called
void URuntimeProviderSphereTerrain::CreateSection(int32 LODIndex, int32 SectionId, const FRuntimeMeshSectionProperties &SectionProperties)
{
    {
        // This should add a section id
        FScopeLock Lock(&MeshSyncRoot);
        SectionDataMap.FindOrAdd(LODIndex).FindOrAdd(SectionId) = MakeTuple(SectionProperties, FRuntimeMeshRenderableMeshData(), FBoxSphereBounds(FVector(-SphereRadius, -SphereRadius, -SphereRadius), FVector(SphereRadius, SphereRadius, SphereRadius), 0));
    }

    URuntimeMeshProvider::CreateSection(LODIndex, SectionId, SectionProperties);
}