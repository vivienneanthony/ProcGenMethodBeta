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
	TraceDelegateGrass.BindUObject(this, &ASpawnHISMActor::TraceDoneGrass);

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

//	bValidPlayerPawn = true;

	// Call Super Begin
	Super::BeginPlay();	
}

// Called every frame
void ASpawnHISMActor::Tick(float DeltaTime)
{
    // if player is valid
    //if(IsValid(playerPawn))
    //{
		//if(bValidPlayerPawn)
//		{			
    		playerTransform = playerPawn -> GetTransform();    
//		}
    //}	   

	// Handle Supertick
	Super::Tick(DeltaTime);

	 // If not waiting for a responsedo another trace
    if(!WaitiingForResponse)
    {
		// Do Trace
        DoTrace();
    }

	// increase accumated time
	accumalatedTime+=DeltaTime;

	// set trigger true
	if(accumalatedTime>5.0f)
	{
		
		// Get current location
		currentActorLocation = playerPawn -> GetActorLocation();

		if(FVector::DistSquared(currentActorLocation, previousActorLocation)>500.0f)
		{
			// Set trigger
			trigger = true;

			// Save location
			previousActorLocation = currentActorLocation;

			// Enqueue
			ResetTrigger.Enqueue(trigger);		
		}

		accumalatedTime = 0.0f;
	}

	if(!GrassSpawnPoints.IsEmpty())
	{
		FVector point;

		GrassSpawnPoints.Dequeue(point);

		// Do async trace
		//DrawDebugLine(GetWorld(),point,FVector(0.0f,0.0f,0.0f), FColor::Red, false, 1, 0, 2);
       

		GetWorld()->AsyncLineTraceByChannel(EAsyncTraceType::Single, point, FVector(0.0f,0.0f,0.0f), ECC_GameTraceChannel1, collisionParams,
                                    FCollisionResponseParams::DefaultResponseParam, &TraceDelegateGrass);
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


	if(CurrentRunning_workerThreadGrass && workerThreadGrass)
	{
		// Pause Thread
		CurrentRunning_workerThreadGrass->Suspend(true);

		// Change thread state
		workerThreadGrass->bStopThread = true;

		// Suspend thread
		CurrentRunning_workerThreadGrass->Suspend(false);

		// Set kill to false
		CurrentRunning_workerThreadGrass->Kill(false);

		// Wait for completion
		CurrentRunning_workerThreadGrass->WaitForCompletion();

		delete workerThreadGrass;
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

		if(PopulateType==EPopulateTypes::PopulateType_LandscapeGrass)
		{
			// Create new thread
			workerThreadGrass = new FSpawnHISMThreadGrass(2500.0f, this);

			// Add to container
			CurrentRunning_workerThreadGrass =  FRunnableThread::Create(workerThreadGrass, TEXT("WorkerThreadGrass"));
		}
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
	// If trace queue is empty
	if(TraceCallQueue.IsEmpty())
    {
        WaitiingForResponse = false;
		
		return;
    }
		
	// De Queue
	TraceCallQueue.Dequeue(inTraceCall);

	// Quickly check in bound - Immediate reject
	if(CheckInBound(inTraceCall.Start)==false)
	{
		WaitiingForResponse = false;
		
		return;
	}

	// Get World
	UWorld * thisWorld = GetWorld();

	// Log
	if(IsValid(thisWorld))
	{
        // Set Processing
        WaitiingForResponse = true;

		responsePick = inTraceCall.Pick;

		// if count is more then 0
        // DrawDebugLine(thisWorld,inTraceCall.Start*5,inTraceCall.End, FColor::Green, false, 1, 0, 2);
       
		// Do async trace
		thisWorld->AsyncLineTraceByChannel(EAsyncTraceType::Single, inTraceCall.Start*5, inTraceCall.End, ECC_GameTraceChannel1, collisionParams,
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

	  	// If actor has tag terrain
		if(hitResults.Actor->ActorHasTag("Terrain"))
		{	
			// prevent double results
			if(!HISMLocations.Contains(hitResults.Location))
			{
				NormalizeVector = hitResults.Location;

				NormalizeVector.Normalize();

				if((1.0f-FVector::DotProduct(NormalizeVector, hitResults.Normal))<.10f)
				{
					// Get results and changesize scale to match
					newTransform.SetLocation(hitResults.ImpactPoint);

					// make a new rotator
					MyRotator = FRotationMatrix::MakeFromZ(hitResults.ImpactNormal).Rotator();
		
					// Set ritatib
					newTransform.SetRotation(FQuat(MyRotator));

					// Async Spawn - Prevent Creation on span - Right now selects the first one but can be change to multiple
					if(responsePick>HISMArray.Num()-1)
					{
						responsePick=0;

					}

					// Change selected hismto
					selectHISM=responsePick;
					
					int32 InstanceID = HISMArray[selectHISM]->AddInstanceWorldSpace(newTransform);						

					// Add item to quere
					FHISMQueueLogItem itemadded;
			
					itemadded.SelectedHISM = selectHISM;
					itemadded.Location = hitResults.Location;
					itemadded.InstanceID = InstanceID;

					// add			
					HISMLocations.Add(hitResults.Location, itemadded);

					// add count
					hismCount++;
				}

				// add vector
				if(PopulateType==EPopulateTypes::PopulateType_LandscapeGrass)
				{
					// Add to queue
					GrassPoints.Enqueue(hitResults.Location);
				}

			}
			else
			{
				UE_LOG(LogTemp, Warning,TEXT("Rejected"));
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
	//if(IsValid(playerPawn))
	//{
		//UE_LOG(LogTemp, Warning,TEXT("String %s"),*inVector.ToString());

		// Get actor Location
		inLocation = playerPawn->GetActorLocation();

		// Get vector safe normal
		inVectorLocation = inVector.GetSafeNormal();
		
		// Calculate distance
		ActorDistance = FVector::Dist(inLocation, FVector(0.0f,0.0f,0.0f));

		// Calculate newLocation based on invector
		newLocation =  inVectorLocation*ActorDistance;

		if(FVector::Dist(newLocation, inLocation)<4000.0f)
		{	
				return true;
		}else
		{
			return false;
		}
	// }
	
	return false;
}



// trace
void ASpawnHISMActor::TraceDoneGrass(const FTraceHandle& TraceHandle, FTraceDatum & TraceData)
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

	  	// If actor has tag terrain
		if(hitResults.Actor->ActorHasTag("Terrain"))
		{	
				//FVector ThisNormalizeVector = hitResults.Location;

				//ThisNormalizeVector.Normalize();

				//if((1.0f-FVector::DotProduct(ThisNormalizeVector, hitResults.Normal))<.10f)
				//{
					// Get results and changesize scale to match
					FTransform ThisNewTransform;
					
					ThisNewTransform.SetLocation(hitResults.ImpactPoint);

					// make a new rotator
					FRotator ThisRotator = FRotationMatrix::MakeFromZ(hitResults.ImpactNormal).Rotator();
		
					// Set ritatib
					ThisNewTransform.SetRotation(FQuat(ThisRotator));

					HISMArray[0]->AddInstanceWorldSpace(ThisNewTransform);						
				//}				

				 UE_LOG(LogTemp, Warning, TEXT("Grass"));
			
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
}