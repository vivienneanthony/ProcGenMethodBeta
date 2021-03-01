// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Components/BoxComponent.h"

#include "TerrainMarker.generated.h"

UCLASS()
class PROCGENMETHODBETA_API ATerrainMarker : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATerrainMarker();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float radius = 2000.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool visibilityInGame = true;

	UPROPERTY()
	UBoxComponent* collisionMesh = nullptr;

	// Override base construction
	void OnConstruction(const FTransform &Transform) override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;	
	
};
