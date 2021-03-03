 
#pragma once
 
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "SpawnHISMThread.h"
 
#include "../Structures/TraceCall.h"
 
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
 
	// Just need array of instances
	UPROPERTY(BlueprintReadWrite)
	TArray<UHierarchicalInstancedStaticMeshComponent *> HISMArray;
 
    UFUNCTION(BlueprintCallable)	
	void DoTrace();

    UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
    APawn * playerPawn = nullptr;

    UPROPERTY(BlueprintReadOnly)
    FTransform playerTransform;

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
 
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
 
	// End Play
	virtual void EndPlay(EEndPlayReason::Type endplayReason) override;
 
 
	// Thread Safe
	TQueue<FTraceCall> TraceCallQueue;
 
};
 