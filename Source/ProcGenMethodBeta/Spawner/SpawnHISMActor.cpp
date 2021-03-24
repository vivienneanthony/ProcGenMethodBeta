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

	// Create density wrapper
	fastNoiseWrapperTerrain = CreateDefaultSubobject<UFastNoiseWrapper>(TEXT("FastNoiseWrapperTerrain"));		
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

	 // Setup second noise
    fastNoiseWrapperTerrain->SetNoiseType(noiseTypeTerrain);
    fastNoiseWrapperTerrain->SetFractalType(fractalTypeTerrain);
    fastNoiseWrapperTerrain->SetFrequency(noiseFrequencyTerrain);
    fastNoiseWrapperTerrain->SetOctaves(noiseOctavesTerrain);

	// Set paramenters
	collisionParams.bReturnFaceIndex = false;
	collisionParams.bReturnPhysicalMaterial = true;
	collisionParams.bTraceComplex = true;

	// show
	showDebug = true;

	// Get Location
	previousActorLocation = playerPawn -> GetActorLocation();

	// Call Super Begin
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

	 // If not waiting for a responsedo another trace
    if(!WaitiingForResponse)
    {
		// Do Trace
        DoTrace();
    }

	accumalatedTime+=DeltaTime;

	if(accumalatedTime>5.0f)
	{
		trigger = true;

		// Get current location
		currentActorLocation = playerPawn -> GetActorLocation();

		if(FVector::DistSquared(currentActorLocation, previousActorLocation)>500.0f)
		{
			previousActorLocation = currentActorLocation;

			ResetTrigger.Enqueue(trigger);		
		}

		accumalatedTime = 0.0f;
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
		workerThread = new FSpawnHISMThread(PopulateType, seed, enablePoisson, EWorldTrace::WorldTrace_FlatWorld, this);

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
		    DrawDebugLine(thisWorld,inTraceCall.Start*4,inTraceCall.End, FColor::Green, false, 1, 0, 2);
        }

		// Do async trace
		thisWorld->AsyncLineTraceByChannel(EAsyncTraceType::Single, inTraceCall.Start*4, inTraceCall.End, ECC_Visibility, collisionParams,
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

	   if(CheckInBound(hitResults.Location)==false)
	   {
		// Do nothing - Skip
	   }
	   else 
	   {	
		   if(hitResults.Actor->ActorHasTag("Terrain"))
		   {	
				// prevent double results
				if(!HISMLocations.Contains(hitResults.Location))
				{
					// Get results and changesize scale to match
					newTransform.SetLocation(hitResults.ImpactPoint);

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

					// add			
					HISMLocations.Add(hitResults.Location, itemadded);

					// add count
					hismCount++;
				}
				else
				{
					UE_LOG(LogTemp, Warning,TEXT("Rejected"));
				}
		   }
	   }        
    }
	else
    {
		// Log a hit
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
	bool returnstate = true;

	// Used density to show a area 
	// Probably can be put into the thread itself

	return returnstate;
}



  
bool ASpawnHISMActor::CheckInBound(FVector inVector)
{
	if(IsValid(playerPawn))
	{
		ActorLocation = playerPawn->GetActorLocation();

    	// Should check boundary
    	Min = ActorLocation - FVector(4000.0f, 4000.0f, 4000.0f);
    	Max = ActorLocation + FVector(4000.0f, 4000.0f, 4000.0f);

    	// In Accurate
    	if((inVector.X>Min.X)&&(inVector.Y>Min.Y)&&(inVector.Z>Min.Z)&&(inVector.X<Max.X)&&(inVector.Y<Max.Y)&&(inVector.Z<Max.Z))
    	{
        	return true;
	    }
	    else
    	{
        	return false;
	    }
	}
	else
	{
		return false;
	}

	return false;
}