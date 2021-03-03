// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnHISMActor.h"


// Sets default values
ASpawnHISMActor::ASpawnHISMActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void ASpawnHISMActor::BeginPlay()
{
	// Bind
	TraceDelegate.BindUObject(this, &ASpawnHISMActor::TraceDone);

    // Get Player Pawn
    playerPawn = GetWorld()->GetFirstPlayerController()->GetPawn(); 

	Super::BeginPlay();

    
	
}

// Called every frame
void ASpawnHISMActor::Tick(float DeltaTime)
{
    // if player is valid
    if(IsValid(playerPawn))
    {
        playerTransform = playerPawn -> GetTransform();    
    }

	Super::Tick(DeltaTime);
}

// End Play
void ASpawnHISMActor::EndPlay(EEndPlayReason::Type endplayReason)
{
	if(CurrentRunning_workerThread && workerThread)
	{
		// Pause Thread
		CurrentRunning_workerThread->Suspend(true);

		// Change thread state
		workerThread->bStopThread = true;

		// Suspend thread
		CurrentRunning_workerThread->Suspend(false);

		// Set kill to false
		CurrentRunning_workerThread->Kill(false);

		// Wait for completion
		CurrentRunning_workerThread->WaitForCompletion();

		delete workerThread;
	}
}


// Init Calcuations
void ASpawnHISMActor::Initialize()
{
	// if count is more then 0
	if(HISMArray.Num())
	{
		// Create new thread
		workerThread = new FSpawnHISMThread(HISMArray, this);

		// Add to container
		CurrentRunning_workerThread =  FRunnableThread::Create(workerThread, TEXT("WorkerThread"));
	}
	else
	{
		// log
		UE_LOG(LogTemp, Warning, TEXT("ASpawnHISMActor :: Can't creat thread"));
	}
}

// Do Trace
void ASpawnHISMActor::DoTrace()
{
	if(TraceCallQueue.IsEmpty())
		return;
	
	// Create trace call
	FTraceCall inTraceCall;

	// De Queue
	TraceCallQueue.Dequeue(inTraceCall);

	// Get World
	UWorld * thisWorld = GetWorld();

	// Log
	if(IsValid(thisWorld))
	{
		// if count is more then 0
		DrawDebugLine(thisWorld, inTraceCall.Start, inTraceCall.End, FColor::Green, false, 1, 0, 1);

		// This should do the trace call
		thisWorld->AsyncLineTraceByChannel(EAsyncTraceType::Single, inTraceCall.Start, inTraceCall.End, ECC_Visibility, collisionParams,
                                    FCollisionResponseParams::DefaultResponseParam, &TraceDelegate);
	}
}

// trace
void ASpawnHISMActor::TraceDone(const FTraceHandle& TraceHandle, FTraceDatum & TraceData)
{
	// Get first results
	if(TraceData.OutHits.Num() > 0)
    {
		// Hit results
      	const FHitResult& Result = TraceData.OutHits[0];

		// Log a hit
		UE_LOG(LogTemp, Warning, TEXT("OutHit"));
    }else
    {
        UE_LOG(LogTemp, Warning, TEXT("NonHit"));
    }
}