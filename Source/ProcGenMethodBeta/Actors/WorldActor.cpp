// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldActor.h"

#include "Providers/RuntimeMeshProviderSphere.h"
#include "Providers/RuntimeMeshProviderStatic.h"
#include "../RuntimeProvider/RuntimeProviderSphereTerrain.h"

#include <cstdlib>

// Sets default values
AWorldActor::AWorldActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a root scene component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));

	// Set mobility to static
	if (RootComponent != nullptr)
	{
		// Write Log
		UE_LOG(LogTemp, Warning, "Scene Component Not Created"));
	}
}

// Construction
void AWorldActor::OnConstruction(const FTransform &Transform)
{
	Super::OnConstruction(Transform);
}

// Called when the game starts or when spawned
void AWorldActor::BeginPlay()
{
	Super::BeginPlay();

	// Make sure object exist
	if (component_RMC == nullptr)
	{
		component_RMC = NewObject<URuntimeMeshComponent>(this, TEXT("RunTimeMeshComponent"));
	}

	if (component_MC == nullptr)
	{
		component_MC = NewObject<UMeshMarchingCube>(this, TEXT("MeshMarchingCube"));
	}

	// Test static
	TestProviderSphereTerrain();
}

// Called every frame
void AWorldActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Copy Defaults To Marching Cube
void AWorldActor::SetMarchingCubeNoiseDefaults()
{
	// copy default
	component_MC->noiseType = in_noiseType;
	component_MC->fractalType = in_fractalType;
	component_MC->noiseFrequency = in_noiseFrequency;
	component_MC->noiseOctaves = in_noiseOctaves;
	component_MC->noiseCutoff = in_noiseCutoff;

	// copy default
	component_MC->noiseTypeTerrain = in_noiseTypeTerrain;
	component_MC->fractalTypeTerrain = in_fractalTypeTerrain;
	component_MC->noiseFrequencyTerrain = in_noiseFrequencyTerrain;
	component_MC->noiseOctavesTerrain = in_noiseOctavesTerrain;
	component_MC->noiseCutoffTerrain = in_noiseCutoffTerrain;

	// other defaults
	component_MC->cubeCellSize = in_cubeCellSize;
	component_MC->cubeSize = in_cubeSize;
	component_MC->surfaceLevel = in_surfaceLevel;
	component_MC->coreLevel = in_coreLevel;
}

// Test
void AWorldActor::TestSphereProvider()
{
	// Create A RuntimeMesh Provider
	URuntimeMeshProviderSphere *SphereProvider = NewObject<URuntimeMeshProviderSphere>(this, TEXT("RuntimeMeshProvider-Sphere"));

	// If setup
	if (SphereProvider)
	{
		// If Runtimemesh provider is created
		if (component_RMC)
		{
			// Set Sphere Radius Default
			SphereProvider->SetSphereRadius(20000.0f);

			// If material set material if it exist
			if (Material)
			{
				SphereProvider->SetSphereMaterial(Material);
			}

			// Initialize
			component_RMC->Initialize(SphereProvider);
		}
	}
}

void AWorldActor::TestProviderSphereTerrain()
{
	// Use Runtime Mesh Terraini to produce mesh
	URuntimeProviderSphereTerrain *SphereTerrainProvider = NewObject<URuntimeProviderSphereTerrain>(this, TEXT("RuntimeProviderSphereTerrain "));

	// Sphere
	if (SphereTerrainProvider)
	{
		if (component_RMC)
		{
			// Create a parameter
			FMeshMarchingCubeParameters outParameters;

			// copy defaults
			outParameters.noiseType = in_noiseType;
			outParameters.fractalType = in_fractalType;
			outParameters.noiseOctaves = in_noiseOctaves;
			outParameters.noiseFrequency = in_noiseFrequency;
			outParameters.noiseCutoff = in_noiseCutoff;
			outParameters.noiseTypeTerrain = in_noiseTypeTerrain;
			outParameters.fractalTypeTerrain = in_fractalTypeTerrain;
			outParameters.noiseOctavesTerrain = in_noiseOctavesTerrain;
			outParameters.noiseFrequencyTerrain = in_noiseFrequencyTerrain;
			outParameters.noiseCutoffTerrain = in_noiseCutoffTerrain;
			outParameters.cubeCellSize = in_cubeCellSize;
			outParameters.cubeSize = in_cubeSize;
			outParameters.surfaceLevel = in_surfaceLevel;
			outParameters.coreLevel = in_coreLevel;

			// Set Marching Cube Parameters
			SphereTerrainProvider->SetMarchingCubeParameters(outParameters);

			// Set Material
			SphereTerrainProvider->SetSphereMaterial(Material);

			// test initialize
			component_RMC->Initialize(SphereTerrainProvider);
		}
	}
}

// Test
void AWorldActor::TestStaticProvider()
{
	// Fail safe if no mesh data is generated
	if (component_MC->GetVerticesData().Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Mesh Data Generated"));
		return;
	}

	URuntimeMeshProviderStatic *StaticProvider = NewObject<URuntimeMeshProviderStatic>(this, TEXT("RuntimeMeshProvider-Static"));

	if (StaticProvider)
	{
		if (component_RMC)
		{
			// test initialize
			component_RMC->Initialize(StaticProvider);

			StaticProvider->SetupMaterialSlot(0, TEXT("TriMat"), Material);

			// This creates 3 vertex colors
			TArray<FColor> Colors{FColor::Blue, FColor::Red, FColor::Green};

			// Blank info
			TArray<FVector> EmptyNormals;
			TArray<FVector2D> EmptyTexCoords;
			TArray<FRuntimeMeshTangent> EmptyTangents;

			StaticProvider->CreateSectionFromComponents(0, 0, 0, component_MC->GetVerticesData(), component_MC->GetTrianglesData(), EmptyNormals, EmptyTexCoords, Colors, EmptyTangents, ERuntimeMeshUpdateFrequency::Infrequent, true);
		}
	}
}