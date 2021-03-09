#include "SpawnHISMThread.h"
 
 
// Sets default values
FSpawnHISMThread::FSpawnHISMThread(EPopulateTypes inPopulateType, EWorldTrace traceType, ASpawnHISMActor * callingActor)
{
 	// Is Valid
	if(IsValid(callingActor))
	{
		currentTraceType = traceType;
		currentThreadActor = callingActor;
		currentPopulateType = inPopulateType;

		// Parameters
		collisionParams.bTraceComplex=true;	   			
				
	}
}
 
 
// Initialize
bool FSpawnHISMThread::Init()
{
	bStopThread = false;
 	
	return true;
}
 
// Stop
void FSpawnHISMThread::Stop()
{
 
}
 
// Run
uint32 FSpawnHISMThread::Run()
{
 	// loop
 	while(!bStopThread)
 	{
        FPlatformProcess::Sleep(0.01);

		 // test async
		AsyncTraceCollisionToPoint();				
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
	Start = ChooseACoordinate();
 	
	// Same as square length
	End =  FVector(Start.X, Start.Y, -1.0f);

	// start and end
	OutTraceCall.Start = Start;
	OutTraceCall.End = End;
	OutTraceCall.Scale = 1.0f;

    currentThreadActor->TraceCallQueue.Enqueue(OutTraceCall);

	return;
}