#pragma once

// ProcGenMethodBeta Plugin
// Developer Vivienne Anthony
// Copyright (c) 2021 


#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"

#include "../Enums/WorldTypes.h"
#include "../Enums/SubWorldTypes.h"
#include "../Enums/Temperament.h"
#include "../Enums/PopulateTypes.h"

#include "../Structures/PopulateAssetLibrary.h"

#include "PopulateAsset.h"

// header
#include "WorldLibrary.generated.h"


// Create Populate Structure
USTRUCT(BlueprintType)
struct PROCGENMETHODBETA_API FWorldLibrary : public FTableRowBase
{
   GENERATED_BODY()
	
   // So to simplify your life for later debugging, always use UPROPERTY()
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
   FText Name;

   // So to simplify your life for later debugging, always use UPROPERTY()
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
   FText Seed;

    // So to simplify your life for later debugging, always use UPROPERTY()
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
   EWorldClasses WorldClass;

    // So to simplify your life for later debugging, always use UPROPERTY()
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
   ESubWorldClasses SubWorldClass;

    // So to simplify your life for later debugging, always use UPROPERTY()
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
   TArray<ETemperaments> Temperaments;

    // So to simplify your life for later debugging, always use UPROPERTY()
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
   TMap<EPopulateTypes, FPopulateAssetLibrary> PopulateLibrary;
};
