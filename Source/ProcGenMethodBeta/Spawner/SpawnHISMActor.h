 
#pragma once
 
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "SpawnHISMThread.h"
#include "SpawnHISMThreadGrass.h"
 
#include "../Structures/TraceCall.h"
#include "../Structures/PopulateAsset.h"
#include "../Structures/HISMQueueLogItem.h"

#include "../Enums/PopulateTypes.h"
#include "../Enums/WorldTrace.h"

#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
 

#include "Engine/Classes/Components/StaticMeshComponent.h"
#include "Engine/Classes/Components/InstancedStaticMeshComponent.h"
#include "Engine/Classes/Components/HierarchicalInstancedStaticMeshComponent.h"

#include "Kismet/KismetMathLibrary.h"

// Fast Noise Wrapper
#include "FastNoiseWrapper.h"

#include "SpawnHISMActor.generated.h"
 
UCLASS()
class PROCGENMETHODBETA_API ASpawnHISMActor : public AActor
{
	GENERATED_BODY()
 
public:	
	// Sets default values for this actor's properties
	ASpawnHISMActor();
 
	UFUNCTION(BlueprintCallable)	
	void Initialize();
 
    UFUNCTION(BlueprintCallable)	
	void DoTrace();

	// Just need array of instances
	UPROPERTY(BlueprintReadWrite)
	TArray<UHierarchicalInstancedStaticMeshComponent *> HISMArray;
 
 	UPROPERTY(BlueprintReadWrite)
	EPopulateTypes PopulateType;

 	UPROPERTY(BlueprintReadWrite)
	TArray<FPopulateAsset> PopulateAssets;

    UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category ="Play Info (ReadOnly)")
    APawn * playerPawn = nullptr;

    UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category ="Play Info (ReadOnly)")
    FTransform playerTransform;

 	UPROPERTY(BlueprintReadWrite, Category="Debug")
    bool enablePoisson = false;

    UPROPERTY(BlueprintReadWrite, Category="Debug")
    bool showDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
	EFastNoise_NoiseType noiseTypeTerrain = EFastNoise_NoiseType::Simplex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
	EFastNoise_FractalType fractalTypeTerrain = EFastNoise_FractalType::RigidMulti;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
	int32 noiseOctavesTerrain = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
	float noiseFrequencyTerrain = 0.001f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
	float noiseCutoffTerrain = 0.2f;

	// Noise - Density Wrapper
	UPROPERTY(BlueprintReadWrite)	
	UFastNoiseWrapper * densityNoiseWrapper;	

	// Noise - Density Wrapper
	UPROPERTY(BlueprintReadWrite)	
	UFastNoiseWrapper * fastNoiseWrapperTerrain;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
	int32 seed;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;	
 
	// runnable
	class FSpawnHISMThread  * workerThread = nullptr;
 
	// container
	FRunnableThread *CurrentRunning_workerThread = nullptr;
 
	// Delegate
	FTraceDelegate TraceDelegate;
	FTraceDelegate TraceDelegateGrass;
 
	// Collision Parameters
	FCollisionQueryParams collisionParams;
 
	//  Do Trace
	void TraceDone(const FTraceHandle& TraceHandle, FTraceDatum & TraceData);	
	void TraceDoneGrass(const FTraceHandle& TraceHandle, FTraceDatum & TraceData);	


	bool ChooseGenerate(FVector Location);

    // Method to force a scan twice
    bool WaitiingForResponse = false;

	int32 selectHISM = 0;

	// log items
	TQueue<FHISMQueueLogItem> generatedHISMs;

	uint64 hismCount;

	uint32 hismLimit = 1000;

	FVector currentActorLocation;
	
	FVector previousActorLocation;

	///////////////////
	///////////////////
	//
	////
	// runnable
	class FSpawnHISMThreadGrass  * workerThreadGrass = nullptr;
 
	// container
	FRunnableThread *CurrentRunning_workerThreadGrass = nullptr;
	
	///////////////////
	//
	///////////////////

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
 
	// End Play
	virtual void EndPlay(EEndPlayReason::Type endplayReason) override;
  
	// Thread Safe
	TQueue<FTraceCall> TraceCallQueue;	

	// Locaition HISM
	TMap<FVector,FHISMQueueLogItem> HISMLocations;

    // Cache newTransform
    FTransform newTransform;

	// Allows quicker production
	FRotator MyRotator;

	// check in bound
	bool CheckInBound(FVector inVector);

	// Trigger
	TQueue<bool> ResetTrigger;

	float accumalatedTime;

	// cache
	FVector Max;
	FVector Min;
	FVector ActorLocation;

	bool trigger;


	FVector inVectorLocation;
	FVector inLocation;

	float ActorDistance;
	
	FVector newLocation;

	// Create trace call
	FTraceCall inTraceCall;

	FVector NormalizeVector;

	uint32 responsePick;		
	
	//////////////////
	/////////////////
	//
	/////////////////
	/////////////////


	// Grass Points
	TQueue<FVector> GrassPoints;
	TQueue<FVector> GrassSpawnPoints;

};
 