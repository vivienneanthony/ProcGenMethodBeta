#pragma once

// ProcGenMethodBeta Plugin
// Developer Vivienne Anthony
// Copyright (c) 2021 


UENUM(BlueprintType)			//"BlueprintType" is essential to include
enum class ESubWorldClasses : uint8
{
        SubWorldClass_ContinentalWorld        UMETA(DisplayName="ContinentalWorld"),
        SubWorldClass_DeadWorld 	      UMETA(DisplayName="DeadWorld"),
        SubWorldClass_GaseousWorld 	      UMETA(DisplayName="GaseousWorld"),        
        SubWorldClass_OceanicWorld            UMETA(DisplayName="OceanicWorld"),
        SubWorldClass_PageanWorld             UMETA(DisplayName="PageanWorld"),
        SubWorldClass_ReefWorld               UMETA(DisplayName="ReefWorld"),	 
        SubWorldClass_SwampWorld              UMETA(DisplayName="SwampWorld")
        
        
};
