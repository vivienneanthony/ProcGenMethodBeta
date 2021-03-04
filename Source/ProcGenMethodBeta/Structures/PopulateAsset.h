#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"

// header
#include "PopulateAsset.generated.h"

// ProcGenMethodBeta Plugin
// Developer Vivienne Anthony
// Copyright (c) 2021 

// Create Populate Structure
USTRUCT(BlueprintType)
struct PROCGENMETHODBETA_API FPopulateAsset : public FTableRowBase
{
    GENERATED_BODY()
	
    // seed
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Name;    
    
    // seed
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Seed;    

	// So to simplify your life for later debugging, always use UPROPERTY()	
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UStaticMesh* StaticMesh;
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool Procedural = false;
	    
    // cull minimum
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 cullMinimum = 10;

    // cull maximum
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 cullMaximum = 1000;

    // scale minimum
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float scaleMinimum = 1.0f;

    // scale maximum
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float scaleMaxium = 2.0f;
    
};


// NOTE : can add fauna and or building type

