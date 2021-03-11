// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnPoissonSamplerActor.h"


// Sets default values
ASpawnPoissonSamplerActor::ASpawnPoissonSamplerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;	
}

// Called when the game starts or when spawned
void ASpawnPoissonSamplerActor::BeginPlay()
{
	Super::BeginPlay();	

	FVector location = GetActorLocation();

	// Test if array is built
	TArray<FVector>  results = sampler.SampleSphere(seed, location, radius, minimumDistance, iterate);

	// Output
	UE_LOG(LogTemp, Warning, TEXT("Produced %d"), results.Num());

	SpawnFunction(results);
}

// Called every frame
void ASpawnPoissonSamplerActor::Tick(float DeltaTime)
{

}


// Called every frame
void ASpawnPoissonSamplerActor::SpawnFunction(TArray<FVector> inVectors)
{
	// spawnparameter
	FActorSpawnParameters parameter;

	parameter.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FRotator inRotator;

	// for each iteration
   for(auto inVector: inVectors)
   {
	   /// yada
	   AStaticMeshActor * SpawnedActor = (AStaticMeshActor *) GetWorld()->SpawnActor<AStaticMeshActor>(inVector, inRotator, parameter);
	   
	   UStaticMeshComponent * component = SpawnedActor->GetStaticMeshComponent();

		// add sphere
	   component->SetStaticMesh(inMesh);
   }
}

// End Play
void ASpawnPoissonSamplerActor::EndPlay(EEndPlayReason::Type endplayReason)
{	
}

