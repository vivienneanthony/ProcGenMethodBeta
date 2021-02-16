// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldActor.h"

#include "Providers/RuntimeMeshProviderSphere.h"
#include "Providers/RuntimeMeshProviderStatic.h"

#include "Providers/RuntimeMeshProviderCollision.h"
#include "../RuntimeProvider/RuntimeProviderSphereTerrain.h"

#include <cstdlib>

// Sets default values
AWorldActor::AWorldActor(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//component_RMC = NewObject<URuntimeMeshComponent>(this, TEXT("RunTimeMeshComponent"));
	component_RMC = CreateDefaultSubobject<URuntimeMeshComponent>(TEXT("RuntimeMeshComponent0"));

// Use Runtime Mesh Terraini to produce mesh
	SphereTerrainProvider =  CreateDefaultSubobject<URuntimeProviderSphereTerrain>(TEXT("RuntimeProviderSphereTerrain "));

	// Set component static
	component_RMC->Mobility = EComponentMobility::Static;

	// make root component
	RootComponent = component_RMC;
}

// Construction
void AWorldActor::OnConstruction(const FTransform &Transform)
{
	Super::OnConstruction(Transform);
	
	// Test static
	TestProviderSphereTerrain();
}

// Called when the game starts or when spawned
void AWorldActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AWorldActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Test
void AWorldActor::TestSphereProvider()
{
	// Create A RuntimeMesh Provider
	URuntimeMeshProviderSphere *SphereProvider = NewObject<URuntimeMeshProviderSphere>(this, TEXT("RuntimeMeshProvider-Sphere"));

	// If setup
	if (SphereProvider)
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

void AWorldActor::TestProviderSphereTerrain()
{	
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

			// Set Radius
			SphereTerrainProvider->SetSphereRadius(20000.0f);

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
	//if (component_MC->GetVerticesData().Num() <= 0)
	////{
	//	UE_LOG(LogTemp, Warning, TEXT("No Mesh Data Generated"));
	//	return;
	//}

	//URuntimeMeshProviderStatic *StaticProvider = NewObject<URuntimeMeshProviderStatic>(this, TEXT("RuntimeMeshProvider-Static"));

	//if (StaticProvider)
	//{
	//	if (component_RMC)
	//	{
	// test initialize
	//component_RMC->Initialize(StaticProvider);

	//StaticProvider->SetupMaterialSlot(0, TEXT("TriMat"), Material);

	// This creates 3 vertex colors
	//TArray<FColor> Colors{FColor::Blue, FColor::Red, FColor::Green};

	// Blank info
	//TArray<FVector> EmptyNormals;
	//TArray<FVector2D> EmptyTexCoords;
	//TArray<FRuntimeMeshTangent> EmptyTangents;

	//StaticProvider->CreateSectionFromComponents(0, 0, 0, component_MC->GetVerticesData(), component_MC->GetTrianglesData(), EmptyNormals, EmptyTexCoords, Colors, EmptyTangents, ERuntimeMeshUpdateFrequency::Infrequent, true);
	//	}
	//}
}