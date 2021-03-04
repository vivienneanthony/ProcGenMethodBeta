#pragma once

// ProcGenMethodBeta Plugin
// Developer Vivienne Anthony
// Copyright (c) 2021 


UENUM(BlueprintType)	//"BlueprintType" is essential to include
enum class EPopulateTypes : uint8
{
        PopulateType_LandscapeGrass 	UMETA(DisplayName="LandscapeGrass"),
        PopulateType_LandscapeFoilage 	UMETA(DisplayName="LandscapeFoilage"),
        PopulateType_LandscapeRock      UMETA(DisplayName="LandscapeRock"),
        PopulateType_LandscapeFauna     UMETA(DisplayName="LandscapeFauna"),
        PopulateType_Structure          UMETA(DisplayName="Structure")
};