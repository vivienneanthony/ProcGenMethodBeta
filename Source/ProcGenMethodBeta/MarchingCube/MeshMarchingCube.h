// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "../Util/Vect3.h"

#include "FastNoiseWrapper.h"

#include "MarchingCubeUtil.h"
#include "MarchingCubeCell.h"

#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

#include "MeshMarchingCube.generated.h"

UCLASS()
class PROCGENMETHODBETA_API UMeshMarchingCube : public UObject
{
	// Generate Body
	GENERATED_BODY()
public:
	// Constructor
	UMeshMarchingCube();

	// Fill Grid values from perlin noise
	UFUNCTION(BlueprintCallable)
	void InitializeNoiseGridData();

	UFUNCTION(BlueprintCallable)
	void Polygonization();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
	EFastNoise_NoiseType noiseType = EFastNoise_NoiseType::Simplex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
	EFastNoise_FractalType fractalType = EFastNoise_FractalType::RigidMulti;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
	int32 noiseOctaves = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
	float noiseFrequency = 0.02f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
	float noiseCutoff = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
	EFastNoise_NoiseType noiseTypeTerrain = EFastNoise_NoiseType::Simplex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
	EFastNoise_FractalType fractalTypeTerrain = EFastNoise_FractalType::RigidMulti;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
	int32 noiseOctavesTerrain = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
	float noiseFrequencyTerrain = 0.02f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
	float noiseCutoffTerrain = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
	float cubeCellSize = 64;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
	float cubeSize = 256;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Levels")
	float surfacelevel = 20000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Levels")
	float coreLevel = 15000.0f;

	// Getter
	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetVerticesData();

	UFUNCTION(BlueprintCallable)
	TArray<int32> GetTrianglesData();

	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetNormalData();


	// Testing
	UPROPERTY();
	UFastNoiseWrapper *fastNoiseWrapper = nullptr;;

	UPROPERTY();
	UFastNoiseWrapper *fastNoiseWrapperTerrain = nullptr;;

	
protected:
	// Vertice Data
	TArray<FVector> verticesData;
	TArray<int32> trianglesData;
	TArray<FVector> verticesNormalData;

	// Do Intrepreation
	FVector VertexInterp(float isolevel, FVector p1, FVector p2, double valp1, double valp2);

	// Math Fuction to give distance square
	float distanceSquare(FVector v1, FVector v2);

	// Calculate Cell Data
	void CalculateCellData(MarchingCubeCell &cell, uint32 x, uint32 y, uint32 z);
};
