// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldActor.h"

#include "Providers/RuntimeMeshProviderSphere.h"
#include "Providers/RuntimeMeshProviderStatic.h"

#include <cstdlib>

// Sets default values
AWorldActor::AWorldActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create Defaults
	component_RMC = CreateDefaultSubobject<URuntimeMeshComponent>(TEXT("RuntimeMeshComponent"));

	component_MC = CreateDefaultSubobject<UMeshMarchingCube>(TEXT("MeshMarchingCube"));
}

// Called when the game starts or when spawned
void AWorldActor::BeginPlay()
{
	Super::BeginPlay();

	// Copy defaults
	SetMarchingCubeNoiseDefaults();

	// Initialize Grid Data
	component_MC->InitializeNoiseGridData();

	// CreateData
	component_MC->Polygonization();

	// Test static
	TestStaticProvider();
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
	component_MC->surfacelevel = in_surfacelevel;
	component_MC->coreLevel = in_coreLevel;
}

// Test
void AWorldActor::TestBoxProvider()
{
	URuntimeMeshProviderSphere *SphereProvider = NewObject<URuntimeMeshProviderSphere>(this, TEXT("RuntimeMeshProvider-Sphere"));

	if (SphereProvider)
	{
		if (component_RMC)
		{
			// Set default
			SphereProvider->SetSphereRadius(1000.0f);

			if (Material)
			{
				SphereProvider->SetSphereMaterial(Material);
			}

			// test initialize
			component_RMC->Initialize(SphereProvider);
		}
	}
}

// Test
void AWorldActor::TestStaticProvider()
{
	URuntimeMeshProviderStatic *StaticProvider = NewObject<URuntimeMeshProviderStatic>(this, TEXT("RuntimeMeshProvider-Static"));

	if (StaticProvider)
	{
		if (component_RMC)
		{
			// test initialize
			component_RMC->Initialize(StaticProvider);

			// Set Material
			if (Material)
			{
				StaticProvider->SetupMaterialSlot(0, TEXT("TriMat"), Material);
			}

			// This creates 3 vertex colors
			TArray<FColor> Colors{FColor::Blue, FColor::Red, FColor::Green};

			// Blank info
			TArray<FVector2D> EmptyTexCoords;
			TArray<FRuntimeMeshTangent> EmptyTangents;

			StaticProvider->CreateSectionFromComponents(0, 0, 0, component_MC->GetVerticesData(), component_MC->GetTrianglesData(), component_MC->GetNormalData(), EmptyTexCoords, Colors, EmptyTangents, ERuntimeMeshUpdateFrequency::Infrequent, true);
		}
	}
}