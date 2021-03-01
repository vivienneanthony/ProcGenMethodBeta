// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldActor.h"

#include "Providers/RuntimeMeshProviderSphere.h"
#include "Providers/RuntimeMeshProviderStatic.h"

#include "Providers/RuntimeMeshProviderCollision.h"
#include "../RuntimeProvider/RuntimeProviderSphereTerrain.h"

#include <cstdlib>

// Sets default values
AWorldActor::AWorldActor(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// ChunkManager
	component_CM = CreateDefaultSubobject<UProcChunkManager>(TEXT("UProcChunkManager"));

	// Runtime Mesh Component
	component_RMC = CreateDefaultSubobject<URuntimeMeshComponent>(TEXT("RuntimeMeshComponent0"));

	// Use Runtime Mesh Terraini to produce mesh
	provider_SphereTerrain = CreateDefaultSubobject<URuntimeProviderSphereTerrain>(TEXT("RuntimeProviderSphereTerrain "));

	// Set component static
	component_RMC->Mobility = EComponentMobility::Static;
	
	// make root component
	RootComponent = component_RMC;	

	// Set References
	component_CM->SetReferences(provider_SphereTerrain, this);

	// Sphere component
	component_Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("ComponentSphere"));

	// Set Size
	if(IsValid(component_Sphere))
	{
		component_Sphere->InitSphereRadius(radius);;

		 // declare overlap events
		component_Sphere->OnComponentBeginOverlap.AddDynamic(this, &AWorldActor::OnOverlapBegin); 
		component_Sphere->OnComponentEndOverlap.AddDynamic(this, &AWorldActor::OnOverlapEnd); 

		component_Sphere->SetCollisionProfileName(TEXT("OverlapAllDynamics"));
		component_Sphere->SetGenerateOverlapEvents(true);

	}
}

// Construction
void AWorldActor::OnConstruction(const FTransform &Transform)
{

	// Set Size
	if(IsValid(component_Sphere))
	{
		component_Sphere->SetSphereRadius(radius, true);;
	}

	Super::OnConstruction(Transform);
			
	// UE_LOG(LogTemp, Warning, TEXT("Is Initalized C - %s"),  (provider_SphereTerrain->isInitialized ? TEXT("True") : TEXT("False")));
}

// Called when the game starts or when spawned
void AWorldActor::BeginPlay()
{
	Super::BeginPlay();

	// Set Providervalues
	OnConstructionSphereTerrainProvider();			
	
	// Log
	// UE_LOG(LogTemp, Warning, TEXT("Is Initalized D - %s"),  (provider_SphereTerrain->isInitialized ? TEXT("True") : TEXT("False")));

	// if ChunkManager is Iniatilized
	if(component_CM->isInitialized==true)
	{
		// Get actor
    	TArray<AActor *> TerrainMarkerActors;
		TArray<ATerrainMarker *> TerrainMarkers;

        // Get all actors with tag
	   UGameplayStatics::GetAllActorsOfClass(GetWorld(),   ATerrainMarker::StaticClass(), TerrainMarkerActors);

		for(AActor * InActor:TerrainMarkerActors)
		{
			TerrainMarkers.Add((ATerrainMarker *) InActor);
		}

		// Generate
		component_CM->GenerateTerrainByRegion(TerrainMarkers);
	}else
	{
		// Log
		UE_LOG(LogTemp, Warning, TEXT("Component_CM not initialized"));     
	}
}

// Called every frame
void AWorldActor::Tick(float DeltaTime)
{
	// Actor Tick
	Super::Tick(DeltaTime);
}

void AWorldActor::OnConstructionSphereTerrainProvider()
{	
	// Check all component
	if(!IsValid(provider_SphereTerrain)||!IsValid(component_CM)||!IsValid(component_RMC))
	{
		// Log
		UE_LOG(LogTemp, Warning, TEXT("AWorldActor - Provider, Chunk Manager, or RMC fail")); 

		return;
	}

	// Create a parameter
	FMeshMarchingCubeParameters outParameters;

	// copy defaults
	outParameters.noiseType = in_noiseType;
	outParameters.fractalType = in_fractalType;
	outParameters.noiseOctaves = in_noiseOctaves;
	outParameters.noiseFrequency = in_noiseFrequency;
	outParameters.noiseCutoff = in_noiseCutoff;
	outParameters.noiseTypeTerrain = in_noiseTypeTerrain;
	outParameters.fractalTypeTerrain = in_fractalTypeTerrain;
	outParameters.noiseOctavesTerrain = in_noiseOctavesTerrain;
	outParameters.noiseFrequencyTerrain = in_noiseFrequencyTerrain;
	outParameters.noiseCutoffTerrain = in_noiseCutoffTerrain;	
	outParameters.cubeSize = in_cubeSize;
	outParameters.surfaceLevel = in_surfaceLevel;
	outParameters.coreLevel = in_coreLevel;

	// Set Marching Cube Parameters
	provider_SphereTerrain->SetMarchingCubeParameters(outParameters);

	// Set Radius
	provider_SphereTerrain->SetSphereRadius(20000.0f);

	// Set Material
	provider_SphereTerrain->SetSphereMaterial(Material);	

	// test initialize
	component_RMC->Initialize(provider_SphereTerrain);					
	
	// Log
	// UE_LOG(LogTemp, Warning, TEXT("Is Initalized A - %s"),  (provider_SphereTerrain->isInitialized ? TEXT("True") : TEXT("False")));

	// Should Initialize - This can be set to handle rendering specific areas
	component_CM->Initialize();	
	
}


void AWorldActor::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Confirm its a character
	if (AGravityCharacter* OtherCharacter = Cast<AGravityCharacter>(OtherActor))
	{
   		 // do stuff with OtherCharacter
		if (OtherCharacter->GetClass()->ImplementsInterface(UDirGravityInterface::StaticClass()))
		{		
		   IDirGravityInterface * interfacecall = Cast<IDirGravityInterface>(OtherCharacter);

		   interfacecall->Execute_UpdateGravity(OtherCharacter);
		}
	}

} 

void AWorldActor::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}
