#include "SpawnHISMThread.h"
 
 
// Sets default values
FSpawnHISMThread::FSpawnHISMThread(TArray<UHierarchicalInstancedStaticMeshComponent *> inHISMArray, ASpawnHISMActor * callingActor)
{
 	if(inHISMArray.Num())
	{
	   HISMArray = inHISMArray;
	   currentThreadActor = callingActor;
 
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
 
	worldLocation.X+=FMath::RandRange((float)-1000, (float)1000);
	worldLocation.Y+=FMath::RandRange((float)-1000, (float)1000);
 
	return worldLocation;
}
 
// Replacement to BP trace - Should be dirt faster
void FSpawnHISMThread::AsyncTraceCollisionToPoint()
{    
	//  Choose a coordinate
	FVector Start = ChooseACoordinate();
 
	// Same as square length
	FVector End =  FVector(Start.X, Start.Y, -1.0f);

	// trace call
	FTraceCall OutTraceCall;
 
	// start and end
	OutTraceCall.Start = Start;
	OutTraceCall.End = End;
   
    currentThreadActor->TraceCallQueue.Enqueue(OutTraceCall);

	return;
}
 