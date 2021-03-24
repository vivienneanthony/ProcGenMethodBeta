 
#pragma once
 
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"

#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"

#include "SpawnHISMThread.h"
 
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
#include "../Poisson/PoissonSampler.h"

#include "SpawnPoissonSamplerActor.generated.h"
 
UCLASS()
class PROCGENMETHODBETA_API ASpawnPoissonSamplerActor : public AActor
{
	GENERATED_BODY()
 
public:	
	// Sets default values for this actor's properties
	ASpawnPoissonSamplerActor();
 	
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    int32 seed = 1337;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float radius = 1000;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float minimumDistance = 1000;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float iterate = 30;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    UStaticMesh * inMesh;
    
    UFUNCTION()
    void SpawnFunction(TArray<FVector> inVectors);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;	
 	
    PoissonSampler sampler;
    
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
 
	// End Play
	virtual void EndPlay(EEndPlayReason::Type endplayReason) override;
 
};
 