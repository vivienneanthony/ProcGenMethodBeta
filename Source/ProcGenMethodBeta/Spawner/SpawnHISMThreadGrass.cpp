#include "SpawnHISMThread.h"
#include <sys/select.h>
 
 
// Sets default values
FSpawnHISMThreadGrass::FSpawnHISMThreadGrass(float inRadius, ASpawnHISMActor * callingActor)
{
 	// Is Valid
	if(IsValid(callingActor))
	{			
		// calling actor
		currentThreadActor = callingActor;

		// Radius
		currentRadius = inRadius;		
	}
}
 
 
// Initialize
bool FSpawnHISMThreadGrass::Init()
{
	// set stop thread to false and calculate poisson points
	bStopThread = false;
 		
	return true;
}
 
// Stop
void FSpawnHISMThreadGrass::Stop()
{ 
}
 
// Run
uint32 FSpawnHISMThreadGrass::Run()
{
	
 	// loop
 	while(!bStopThread)
 	{
		// gets the placer location then it traces through the vectors in a specific area then spits it out
        FPlatformProcess::Sleep(0.01);

		if(!spawningPoints)
		{
			if(!currentThreadActor->GrassPoints.IsEmpty())
			{
				// Dequeue a vector
				FVector inVector;

				// remove a vector
				currentThreadActor->GrassPoints.Dequeue(inVector);

				// Set currentVector
				currentVector = inVector;

				// this would be the counter
				spawnCounter = 0;

				// Set spane points to true
				spawningPoints = true;

				// Initialize;
				spawnPoints.Initialize(1337, currentRadius, inVector, inVector, 30);
			}			
		}

		// spawning a specific point
		if(spawningPoints)
		{
			// if counter more then 20
			if(spawnCounter>500)
			{
				// set spawnpoints to false
				spawningPoints = false;
				spawnCounter = 0;
			}
			else
			{
				// Out a vector
				FVector4 resultVector = spawnPoints.GetPoint();

				FVector outVector = FVector(resultVector.X, resultVector.Y, resultVector.Z);

				// Increase Counter 				
				spawnCounter++;

				currentThreadActor->GrassSpawnPoints.Enqueue(outVector+currentVector);
			}
		}	
 	}
 
 	return 0;
}
