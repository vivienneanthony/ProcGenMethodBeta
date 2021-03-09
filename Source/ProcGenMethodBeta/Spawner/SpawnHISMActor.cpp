// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnHISMActor.h"
#include "SpawnHISMThread.h"


// Sets default values
ASpawnHISMActor::ASpawnHISMActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;	

	// Create density wrapper
	densityNoiseWrapper = CreateDefaultSubobject<UFastNoiseWrapper>(TEXT("FastNoiseWrapper"));		
}

// Called when the game starts or when spawned
void ASpawnHISMActor::BeginPlay()
{
	// Bind
	TraceDelegate.BindUObject(this, &ASpawnHISMActor::TraceDone);

    // Get Player Pawn
    playerPawn = GetWorld()->GetFirstPlayerController()->GetPawn(); 

	// Set up fast noise
	densityNoiseWrapper->SetupFastNoise();

	// Set paramenters
	collisionParams.bReturnFaceIndex = false;
	collisionParams.bReturnPhysicalMaterial = true;
	collisionParams.bTraceComplex = true;

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

	if(IsValid(playerPawn))
	{
		if(!generatedHISMs.IsEmpty())
		{
			// get from tail
			FHISMQueueLogItem * peekedQueueHISMItem = generatedHISMs.Peek();

			// him
			if( UKismetMathLibrary::Abs(FVector::Dist(peekedQueueHISMItem->Location, playerTransform.GetLocation()) )>100)		
			{
				//UE_LOG(LogTemp, Warning, "Deleting instance from %f"), returnvalue);

				// pop
				HISMArray[peekedQueueHISMItem->SelectedHISM]->RemoveInstance(peekedQueueHISMItem->InstanceID);

				// pop
				generatedHISMs.Pop();

				hismCount--;			
			}
		}

	}

    // If not waiting for a responsedo another trace
    if(!WaitiingForResponse)
    {
        DoTrace();
    }
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
		workerThread = new FSpawnHISMThread(PopulateType, EWorldTrace::WorldTrace_FlatWorld, this);

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
    {
        WaitiingForResponse = false;

		return;
    }
		
	if(hismCount>hismLimit)
	{
		return;
	}

	// Create trace call
	FTraceCall inTraceCall;

	// De Queue
	TraceCallQueue.Dequeue(inTraceCall);

	// Get World
	UWorld * thisWorld = GetWorld();

	// Log
	if(IsValid(thisWorld))
	{
        // Set Processing
        WaitiingForResponse = true;

		// if count is more then 0
        if(showDebug)
        {
		    DrawDebugLine(thisWorld, inTraceCall.Start, inTraceCall.End, FColor::Green, false, 1, 0, 1);
        }

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
      	const FHitResult& hitResults = TraceData.OutHits[0];

		// Log a hit
        if(showDebug)
        { 
            UE_LOG(LogTemp, Warning, TEXT("OutHit"));
        }

	   // only if object has terrain
	   if(hitResults.Actor->ActorHasTag("Terrain")&&ChooseGenerate(hitResults.Location))
	   {		
		   	   				   
        		// Get results and changesize scale to match
        		newTransform.SetLocation(hitResults.Location);

				// make a new rotator
				MyRotator = FRotationMatrix::MakeFromZ(hitResults.ImpactNormal).Rotator();

			
				// Set ritatib
				newTransform.SetRotation(FQuat(MyRotator));

        		// Async Spawn - Prevent Creation on span - Right now selects the first one but can be change to multiple
				selectHISM=FMath::RandRange(0, HISMArray.Num()-1);

        		int32 InstanceID = HISMArray[selectHISM]->AddInstanceWorldSpace(newTransform);						

				// add
				FHISMQueueLogItem itemadded;
				
				itemadded.SelectedHISM = selectHISM;
				itemadded.Location = hitResults.Location;
				itemadded.InstanceID = InstanceID;

				// added
				generatedHISMs.Enqueue(itemadded);

				// add count
				hismCount++;
	   }        
    }else
    {
        // Show a non hit
        if(showDebug)
        {
            UE_LOG(LogTemp, Warning, TEXT("NonHit"));
        }
    }
    
    WaitiingForResponse = false;

    // Do another trace
    DoTrace();
}


bool ASpawnHISMActor::ChooseGenerate(FVector Location)
{
	bool returnstate = false;

	float density = densityNoiseWrapper->GetNoise3D(Location.X, Location.Y, Location.Z);
	
	// get results make sure range is 0 to 1 for density
    density = UKismetMathLibrary::MapRangeClamped(density,-1.0f, 1.0f, 0.0f, 1.0f);

	if(density>0.5)
	{
		returnstate = true;
	}else
	{
		returnstate = false;
	}

	return returnstate;
}