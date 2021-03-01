// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainMarker.h"


// Sets default values
ATerrainMarker::ATerrainMarker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create default	
	collisionMesh = CreateDefaultSubobject<UBoxComponent>(FName("Collision Mesh"));

	// Set root
	SetRootComponent(collisionMesh);

}

// Called when the game starts or when spawned
void ATerrainMarker::BeginPlay()
{
	// Begin play
	Super::BeginPlay();
	
}

// Called every frame
void ATerrainMarker::Tick(float DeltaTime)
{
	// Handle tick
	Super::Tick(DeltaTime);
}

// Construction
void ATerrainMarker::OnConstruction(const FTransform &Transform)
{	
		
	// On construction super
	Super::OnConstruction(Transform);

	// test
	collisionMesh->SetBoxExtent(FVector(radius, radius, radius), false);

	// Set Hidden In Game
	SetActorHiddenInGame(visibilityInGame);
	
}