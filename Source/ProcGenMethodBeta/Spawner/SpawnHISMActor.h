 
#pragma once
 
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "SpawnHISMThread.h"
 
#include "../Structures/TraceCall.h"
#include "../Structures/PopulateAsset.h"

#include "../Enums/PopulateTypes.h"
#include "../Enums/WorldTrace.h"

#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
 

#include "Engine/Classes/Components/StaticMeshComponent.h"
#include "Engine/Classes/Components/InstancedStaticMeshComponent.h"
#include "Engine/Classes/Components/HierarchicalInstancedStaticMeshComponent.h"
 
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
    bool showDebug = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;	
 
	// runnable
	class FSpawnHISMThread  * workerThread = nullptr;
 
	// container
	FRunnableThread *CurrentRunning_workerThread = nullptr;
 
	// Delegate
	FTraceDelegate TraceDelegate;
 
	// Collision Parameters
	FCollisionQueryParams collisionParams;
 
	//  Do Trace
	void TraceDone(const FTraceHandle& TraceHandle, FTraceDatum & TraceData);	
 
    // Method to force a scan twice
    bool WaitiingForResponse = false;

	int32 selectHISM = 0;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
 
	// End Play
	virtual void EndPlay(EEndPlayReason::Type endplayReason) override;
  
	// Thread Safe
	TQueue<FTraceCall> TraceCallQueue;

    // Cache newTransform
    FTransform newTransform;

	

 
};
 