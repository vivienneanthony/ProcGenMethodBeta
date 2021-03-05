// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

// Runtime Mesh Componenent
#include "RuntimeMeshComponent.h"

// FastNoise Wrapper
#include "FastNoiseWrapper.h"

#include "../Structures/MeshMarchingCubeParameters.h"

#include "../RuntimeProvider/RuntimeProviderSphereTerrain.h"

#include "../Marker/TerrainMarker.h"

#include "GravityMovementComponent.h"
#include "GravityCharacter.h"

#include "../Public/DirGravityInterface.h"

// Chunk Manager
#include "../ChunkManager/ProcChunkManager.h"

// Marching Cube
#include "../MarchingCube/MeshMarchingCube.h"

#include "Components/SphereComponent.h"

#include "Kismet/GameplayStatics.h"


#include "WorldActor.generated.h"

class URuntimeMeshComponent;

UCLASS()
class PROCGENMETHODBETA_API AWorldActor : public AActor
{
	GENERATED_UCLASS_BODY()

public:
	// Sets default values for this actor's properties
	AWorldActor();

	// Construction
	void OnConstruction(const FTransform &Transform) override;

	// Defaults
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
	EFastNoise_NoiseType in_noiseType = EFastNoise_NoiseType::Simplex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
	EFastNoise_FractalType in_fractalType = EFastNoise_FractalType::Billow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
	int32 in_noiseOctaves = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
	float in_noiseFrequency = 0.001f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
	float in_noiseCutoff = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
	EFastNoise_NoiseType in_noiseTypeTerrain = EFastNoise_NoiseType::Simplex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
	EFastNoise_FractalType in_fractalTypeTerrain = EFastNoise_FractalType::RigidMulti;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
	int32 in_noiseOctavesTerrain = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
	float in_noiseFrequencyTerrain = 0.001f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
	float in_noiseCutoffTerrain = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
	int32 in_cubeSize = 32;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Levels")
	float in_surfaceLevel = 20000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Levels")
	float in_coreLevel = 15000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	UMaterial *Material;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	float in_atmosphereLevel= 40000.0f;

	// RuntimeMeshComponent
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	URuntimeMeshComponent * component_RMC = nullptr;
	
	// Scene Component
	UPROPERTY()	
	USceneComponent * component_Scene = nullptr;

	// Sphere Provier
	UPROPERTY()	
	URuntimeProviderSphereTerrain * provider_SphereTerrain = nullptr;

	UPROPERTY()	
	UProcChunkManager * component_CM = nullptr;		

	UPROPERTY()
	USphereComponent * component_Sphere = nullptr;	

	UPROPERTY()
	USphereComponent * component_VisibilitySphere = nullptr;	

	// declare overlap begin function
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// declare overlap end function
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Set Marching Cube Noise Defaults
	void SetMarchingCubeNoiseDefaults();

	// Sphere Terrain
	void OnConstructionSphereTerrainProvider();
		
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};

