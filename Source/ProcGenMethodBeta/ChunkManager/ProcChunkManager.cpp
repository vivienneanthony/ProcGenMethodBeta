// Fill out your copyright notice in the Description page of Project Settings.



#include "ProcChunkManager.h"

#include "Kismet/GameplayStatics.h"

// Initialize Chunk Manager
void UProcChunkManager::Initialize()
{    
    // Log    
	// UE_LOG(LogTemp, Warning, TEXT("Is Initalized B - %s"),  (parentRuntimeProvider->isInitialized ? TEXT("True") : TEXT("False")));
    if(!IsValid(parentRuntimeProvider))
    {
        // Log
        UE_LOG(LogTemp, Warning, TEXT("Component_CM - var parentRuntimeProvider Invalid"));     
        
        // Log
        UE_LOG(LogTemp, Warning, TEXT("Component_CM - Skip parentRuntimeProvider check"));     

        return;
    }

    // check if the runtime provider is initialized
    if(parentRuntimeProvider->GetIsInitialized())
    {
        // set is intialized
        isInitialized=true;        
    
        // Get Octrees Nodes
        parentRuntimeProvider->GetAllNodesAtDepth(MAXOCTREENODEDEPTH, OctreeNodeSections);	

        // Log
        UE_LOG(LogTemp, Warning, TEXT("Component_CM - Initialized Passed"));     
    }else
    {
        // Log
        UE_LOG(LogTemp, Warning, TEXT("Component_CM - Initialized Failed"));     
    }
};

// Set References of Provider Terrain and Calling Actor
void UProcChunkManager::SetReferences(URuntimeProviderSphereTerrain * inRuntimeProvider, AActor * inActor)
{
    // Save the runtime Provider - Would be call often	
	parentRuntimeProvider = inRuntimeProvider;
	
    // keep the parent inactor	
	parentActor = inActor;

    return;
};

// Generate Terrrion By Regions (Not Used)
void UProcChunkManager::GenerateTerrainByRegion(TArray<ATerrainMarker *> inTerrainMarkers)
{        
    // Array to save selected nodes
    TArray<uint32> SectionsPool;

    // Clear Pool
    SectionsPool.Empty();

    // section idx
    uint32 section=0;

    // Convertto our custom vector class
    Vect3 actorScaleVect3 = Vect3(10.0f, 10.0f, 10.0f);

    // Write Scale
    //UE_LOG(LogTemp, Warning, TEXT("Scale %s"), *actorScale.ToString());

    // loop through each actor    
   	for (ATerrainMarker * InMarker : inTerrainMarkers)
	{	
        // Get location	
        FVector InLocation = InMarker->GetActorLocation();
        float InRadius = InMarker->radius;

        // Log
        UE_LOG(LogTemp, Warning, TEXT("Position %s, Radius %f"),*InLocation.ToString(), InRadius);     
      
        section=0;

        // test sections
        for(OctreeNode * octnode : OctreeNodeSections)
        {
            // Check in a certain radius
            if(octnode->VectorInNodeInScale(Vect3(InLocation.X, InLocation.Y, InLocation.Z), InRadius, 2.0f,actorScaleVect3))                        
            {
                //Generate Section
                if(SectionsPool.Contains(section)==false)
                {
                    // Log
                    UE_LOG(LogTemp, Warning, TEXT("Match %d - Generate Add"), section);   

                     // Add to quick search
                    SectionsPool.Add(section);                                               
                } 
                else
                {
                    // Log
                    UE_LOG(LogTemp, Warning, TEXT("Match %d - Already Generated"), section);   
                }                   
            }

            // increment
            section++;
        }
	}

    // Quick search
    if(SectionsPool.Num())
    {
       // Log
       UE_LOG(LogTemp, Warning, TEXT("Generate Terrain By Section Passed - Call Provider GenerateTerrainBySections"));

       // Generate section
       parentRuntimeProvider->GenerateTerrainBySections(SectionsPool);
    }
    else
    {
        // Log
       UE_LOG(LogTemp, Warning, TEXT("Generate Terrain By Section Failed"));
    }
};

