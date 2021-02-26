// Fill out your copyright notice in the Description page of Project Settings.

#include "RuntimeProviderSphereTerrain.h"
#include <algorithm>

// Maximum Depth Octree

// Using LODDOF FOR DETAIL LEVEL PER NODE

URuntimeMeshProviderSphere::URuntimeMeshProviderSphere()
    : MaxLOD(0), SphereRadius(20000.0f)
{    
    // Set MaxLod to 0
    MaxLOD = 0;
}

// Initialize
void URuntimeProviderSphereTerrain::Initialize()
{

    // Create subobject add to property to prevent deletion on creation
    URuntimeMeshModifierNormals *modifierNormals = NewObject<URuntimeMeshModifierNormals>(this, "RuntimeMeshModifierNormals Calc");
   
    // Not adding a new object
    FWriteScopeLock Lock(ModifierRWLock);
    CurrentMeshModifiers.Add(modifierNormals);
   
    // Create Runtime Mesh LOD Properties
    FRuntimeMeshLODProperties LODProperties;

    // Set screen size
    LODProperties.ScreenSize = 0.0f;

    // This configure LOD properties
    ConfigureLODs({LODProperties});

    // Set Material
    SetupMaterialSlot(0, FName("Auto Terrain Material"), AutoTerrainMaterial);

    // Generate Octree
    rootOctreeNode.BoundRegion.Min = Vect3(-SphereRadius, -SphereRadius, -SphereRadius);
    rootOctreeNode.BoundRegion.Max = Vect3(SphereRadius, SphereRadius, SphereRadius);
    
    // Clear tree
    rootOctreeNode.BuildTree(MAXOCTREENODEDEPTH);

    // Empty
    OctreeNodeSections.Empty();

    // Return list of nodes
    rootOctreeNode.GetAllNodesAtDepth(MAXOCTREENODEDEPTH, OctreeNodeSections);

    isInitialized = true;
}

  
void URuntimeProviderSphereTerrain::Generate()
{
     // Setup regular properties
    FRuntimeMeshSectionProperties Properties;
    Properties.bCastsShadow = true;
    Properties.bIsVisible = true;
    Properties.MaterialSlot = 0;
    Properties.bWants32BitIndices = true;
    Properties.UpdateFrequency = ERuntimeMeshUpdateFrequency::Infrequent;

     // Log
    UE_LOG(LogTemp, Warning, TEXT("Sections %d"), OctreeNodeSections.Num());

    // create all sections  // This should get all sections
    for (unsigned int section = 0; section < OctreeNodeSections.Num(); section++)
    {
        // CreateDefault Section
        this->CreateSection(0, section, Properties);
    }
}

void URuntimeProviderSphereTerrain::GenerateSection(uint32 section)
{
     // Setup regular properties
    FRuntimeMeshSectionProperties Properties;
    Properties.bCastsShadow = true;
    Properties.bIsVisible = true;
    Properties.MaterialSlot = 0;
    Properties.bWants32BitIndices = true;
    Properties.UpdateFrequency = ERuntimeMeshUpdateFrequency::Infrequent;

    // CreateDefault Section
    this->CreateSection(0, section, Properties);    
}


bool URuntimeProviderSphereTerrain::GetSectionMeshForLOD(int32 LODIndex, int32 SectionId, FRuntimeMeshRenderableMeshData &MeshData)
{

    // check lodindex is 0
    check(LODIndex == 0);

    // Get Section Data here
    FSectionDataMapEntry *Section = SectionDataMap.Find(SectionId);

    // return these results
    if (Section)
    {
        // Get section data
        bool result = GenerateSectionData(LODIndex, SectionId, Section->Get<1>());

        if (result)
        {
            // Apply bodifiers
            if (Section->Get<1>().HasValidMeshData())
            {
                FReadScopeLock Lock(ModifierRWLock);

                // Only use the first modifier
                URuntimeMeshModifierNormals *modifier = (URuntimeMeshModifierNormals *)CurrentMeshModifiers[0];

                // Smooth (I think the mesh was applying all modifiers messing with the data)
                modifier->CalculateNormalsTangents(Section->Get<1>(), true);
            }

            // Set Mesh Data
            MeshData = Section->Get<1>();

            //Set this to affect collision
            SetRenderableSectionAffectsCollision(SectionId, true);

            return true;
        }
        else
        {
            SetRenderableSectionAffectsCollision(SectionId, false);
        }

        return false;
    }

    return false;
}

// Get section for a specific LOD
bool URuntimeProviderSphereTerrain::GenerateSectionData(int32 LODIndex, int32 SectionId, FRuntimeMeshRenderableMeshData &SectionData)
{

    // Null pointer marching cube
    UMeshMarchingCube *sectionMarchingCube = nullptr;

    // create a new section
    sectionMarchingCube = NewObject<UMeshMarchingCube>(this, MakeUniqueObjectName(this, UMeshMarchingCube::StaticClass(), "March"));
    
    // add to pull prevent deletion
    MarchingCubePool.Add(sectionMarchingCube);

    // set parameters
    sectionMarchingCube->SetParameters(configMeshMarchingCubeParameters);

    // set prime
    sectionMarchingCube->InitializeNoiseGridData();

    // Write Long if marching cube can't be created
    if (!sectionMarchingCube)
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

    // Version 2 of polygonization
    bool results = sectionMarchingCube->PolygonizationV2(polygonizationBoundRegionMin, polygonizationBoundRegionMax, positions, triangles);
     
    // delete from pool
    MarchingCubePool.Remove(sectionMarchingCube);

    // Write Log
    UE_LOG(LogTemp, Warning, TEXT("Get Section Mesh Lod - Copy to SectionData (Could be a memory move)"));

    bool bTriangleGenerated = false;

    auto AddVertex = [&](const FVector &InPosition, const FVector &InTangentX, const FVector &InTangentZ, const FVector2D &InTexCoord) {
        SectionData.Positions.Add(InPosition);
        SectionData.Tangents.Add(InTangentZ, InTangentX);
        SectionData.Colors.Add(FColor::White);
        SectionData.TexCoords.Add(InTexCoord);
    };

    // Blank fvector and TextCoordinate
    FVector Tangent;
    FVector2D TextCoord;

    UE_LOG(LogTemp,Warning,TEXT("Positions %d"),positions.Num() );
    
    if(results)
    {
         for (uint32 i = 0; i < positions.Num(); i++)
        {
             AddVertex(positions[i], Tangent, Tangent, TextCoord);
        }

        for (uint32 i = 0; i < triangles.Num(); i += 3)
        {
            SectionData.Triangles.AddTriangle(triangles[i], triangles[i + 1], triangles[i + 2]);

            if (bTriangleGenerated == false)
            {
                bTriangleGenerated = true;
            }
        }
    }

    return results;
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


void URuntimeProviderSphereTerrain::CreateSection(int32 LODIndex, int32 SectionId, const FRuntimeMeshSectionProperties &SectionProperties)
{ 
    // Create a section
    {
        FScopeLock Lock(&MeshSyncRoot);
        SectionDataMap.FindOrAdd(SectionId) = MakeTuple(SectionProperties, FRuntimeMeshRenderableMeshData(), FBoxSphereBounds(FVector(OctreeNodeSections[SectionId]->BoundRegion.Min.x, OctreeNodeSections[SectionId]->BoundRegion.Min.y, OctreeNodeSections[SectionId]->BoundRegion.Min.z), FVector(OctreeNodeSections[SectionId]->BoundRegion.Max.x, OctreeNodeSections[SectionId]->BoundRegion.Max.y, OctreeNodeSections[SectionId]->BoundRegion.Max.z), 0));

        // Call super create section
        Super::CreateSection(LODIndex, SectionId, SectionProperties);
    }
}

void URuntimeProviderSphereTerrain::GenerateTerrainBySections(TArray<uint32> inSections)
{        
      // Setup regular properties
    FRuntimeMeshSectionProperties Properties;
    Properties.bCastsShadow = true;
    Properties.bIsVisible = true;
    Properties.MaterialSlot = 0;
    Properties.bWants32BitIndices = true;
    Properties.UpdateFrequency = ERuntimeMeshUpdateFrequency::Infrequent;

    // Generate Each Section
    for(uint32 section=0;section<inSections.Num();section++)
    {
            // CreateDefault Section
            this->CreateSection(0, inSections[section], Properties);    
    }
};

