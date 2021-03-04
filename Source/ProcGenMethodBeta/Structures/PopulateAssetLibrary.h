#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"

// header
#include "PopulateAssetLibrary.generated.h"

// ProcGenMethodBeta Plugin
// Developer Vivienne Anthony
// Copyright (c) 2021 

// Create Populate Structure
USTRUCT(BlueprintType)
struct PROCGENMETHODBETA_API FPopulateAssetLibrary : public FTableRowBase
{
    GENERATED_BODY()
	
    // seed
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FPopulateAsset>  Assets;
};