#include "SpawnHISMThread.h"
 
 
// Sets default values
FSpawnHISMThread::FSpawnHISMThread(EPopulateTypes inPopulateType, int32 inSeed, bool inbEnablePoisson, EWorldTrace traceType, ASpawnHISMActor * callingActor)
{
 	// Is Valid
	if(IsValid(callingActor))
	{
		currentTraceType = traceType;
		currentThreadActor = callingActor;
		currentPopulateType = inPopulateType;

		// Parameters
		collisionParams.bTraceComplex=true;	   			
		
		bEnabledPoisson = inbEnablePoisson;

		currentSeed = inSeed;

		idx = 0;
	}
}
 
 
// Initialize
bool FSpawnHISMThread::Init()
{
	// set stop thread to false and calculate poisson points
	bStopThread = false;
 	
	// if Possion Enabled
	if(bEnabledPoisson)
	{
		// set to true
		poissonSampler.bVectorToSpherical = true;

		// copy
		poissonSampler.fastNoiseWrapperTerrain=currentThreadActor->fastNoiseWrapperTerrain;

		// Set radius
		SetRadius = 15000.0f;
		SetInterations = 30.0f;
		SetMinimumDistance = 5000.0f;

		switch (currentPopulateType)
		{
			case (EPopulateTypes::PopulateType_LandscapeGrass):
			MaximumCount = 99999;
			break;
    	    case (EPopulateTypes::PopulateType_LandscapeFoilage):
			MaximumCount = 10000;
			break;
        	case (EPopulateTypes::PopulateType_LandscapeRock): 
			MaximumCount = 10000;
			break;    
        	case (EPopulateTypes::PopulateType_LandscapeFauna):			
			MaximumCount = 10000;
			break;
        	case (EPopulateTypes::PopulateType_Structure):			
			MaximumCount = 200;
			break;
		}

		// do the iteration - create vector points
		//TArray<FVector> Points = poissonSampler.SampleSphere(currentSeed, FVector(0.0f,0.0f,0.0f), 50000.0f, SetMinimumDistance, 30, 15000.0f);

		// Copy here
		//for(auto Point:Points)
		//{		
		//	VectorPoints.Enqueue(Point);

			// gets the placer location then it traces through the vectors in a specific area then spits it out
    	//    FPlatformProcess::Sleep(0.01);
	//	}		
		
		// Initialize;
		spawnPoints.Initialize(currentSeed, 16000.0f, FVector(0.0f,0.0f,0.0f), currentThreadActor->GetActorLocation(), MaximumCount); 
	}
	
	return true;
}
 
// Stop
void FSpawnHISMThread::Stop()
{
 VectorPoints.Empty();
}
 
// Run
uint32 FSpawnHISMThread::Run()
{
	
 	// loop
 	while(!bStopThread)
 	{
		// gets the placer location then it traces through the vectors in a specific area then spits it out
        FPlatformProcess::Sleep(0.01);

		if(bEnabledPoisson)
		{			
			PoissonBasedOutput();						
		}
		else
		{		
		 // test async
			AsyncTraceCollisionToPoint();				
		}	
		
		if(!currentThreadActor->ResetTrigger.IsEmpty())
		{
			// tigger
			bool results = false;

			currentThreadActor->ResetTrigger.Dequeue(results);

			if(results)
			{
				spawnPoints.ResetSeed();
			}
		}
 	}
 
 	return 0;
}


// Choose a coordinate
FVector FSpawnHISMThread::ChooseACoordinate()
{	
	// Get actor world transform
	FVector worldLocation = currentThreadActor -> playerTransform.GetLocation();
	FRotator worldRotator= currentThreadActor -> playerTransform.Rotator();

	// grass
	switch(currentPopulateType)
	{
		case (EPopulateTypes::PopulateType_LandscapeGrass):
			// random spot			
			worldLocation.X+=FMath::RandRange((float)-512, (float)512);
			worldLocation.Y+=FMath::RandRange((float)-512, (float)512); 			
			break;		
		case (EPopulateTypes::PopulateType_LandscapeFoilage):
			worldLocation.X+=FMath::RandRange((float)-4000, (float)4000);
			worldLocation.Y+=FMath::RandRange((float)-4000, (float)4000); 
			break;
		case (EPopulateTypes::PopulateType_LandscapeRock):
			worldLocation.X+=FMath::RandRange((float)-4000, (float)4000);
			worldLocation.Y+=FMath::RandRange((float)-4000, (float)4000); 
			break;
		case (EPopulateTypes::PopulateType_LandscapeFauna):
			worldLocation.X+=FMath::RandRange((float)-4000, (float)4000);
			worldLocation.Y+=FMath::RandRange((float)-4000, (float)4000); 
			break;
		case (EPopulateTypes::PopulateType_Structure):
			worldLocation.X+=FMath::RandRange((float)-4000, (float)4000);
			worldLocation.Y+=FMath::RandRange((float)-4000, (float)4000); 
			break;
	}	

	return worldLocation;
}
 
// Replacement to BP trace - Should be dirt faster
void FSpawnHISMThread::AsyncTraceCollisionToPoint()
{    
	//  Choose a coordinate
	//Start = ChooseACoordinate();
 	
	// Same as square length
	//End =  FVector(Start.X, Start.Y, -1.0f);

	// start and end
	//OutTraceCall.Start = Start;
	//OutTraceCall.End = End;
	//OutTraceCall.Scale = 1.0f;

    //currentThreadActor->TraceCallQueue.Enqueue(OutTraceCall);

	return;
}


// Replacement to BP trace - Should be dirt faster
void FSpawnHISMThread::PoissonBasedOutput()
{    
	// Get actor location
	FVector currentLocation = currentThreadActor->GetActorLocation();
	
	// Set location
	spawnPoints.SetActorLocation(currentLocation);
	
	// Get new point
	FVector newPoint = spawnPoints.GetNextPoint();

	// if new point is not zero
	if(newPoint!=FVector(0.0f,0.0f,0.0f))
	{
		//  Choose a coordinate - Use spawn point
		FVector Start = newPoint;
	
		FVector End =  FVector(0.0f,0.0f,0.0f);

		// start and end
		OutTraceCall.Start = Start;
		OutTraceCall.End = End;
		OutTraceCall.Scale = 1.0f;


		//UE_LOG(LogTemp, Warning, TEXT("point %s"),*Start.ToString());

		// Create a new outtrace info
		currentThreadActor->TraceCallQueue.Enqueue(OutTraceCall);

	}

	/*if(!VectorPoints.IsEmpty())
	{
		//  Choose a coordinate
		FVector Start;
	
	
		if(VectorPoints.Dequeue(Start))
		{	 	
			// Same as square length
			FVector End;

			End =  FVector(0.0f,0.0f,0.0f);

			// start and end
			OutTraceCall.Start = Start;
			OutTraceCall.End = End;
			OutTraceCall.Scale = 1.0f;

			// UE_LOG(LogTemp, Warning, TEXT("point %s"),*Start.ToString());

  			currentThreadActor->TraceCallQueue.Enqueue(OutTraceCall);

		}
	}*/


	return;
}
