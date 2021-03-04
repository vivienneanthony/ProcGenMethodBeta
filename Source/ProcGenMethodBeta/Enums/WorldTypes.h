#pragma once

// ProcGenMethodBeta Plugin
// Developer Vivienne Anthony
// Copyright (c) 2021 


UENUM(BlueprintType)			//"BlueprintType" is essential to include
enum class EWorldClasses : uint8
{
        WorldClass_AuroraWorld 	 	UMETA(DisplayName="AuroraWorld"),
    	WorldClass_ChaosWorld		UMETA(DisplayName="ChaosWorld"),
        WorldClass_DesertWorld  	UMETA(DisplayName="DesertWorld"),        
        WorldClass_EarthWorld  	    UMETA(DisplayName="EarthWorld"),        
        WorldClass_GasWorld    	 	UMETA(DisplayName="GasWorld"),
        WorldClass_GreyWorld 	 	UMETA(DisplayName="GreyWorld"),
        WorldClass_IceWorld      	UMETA(DisplayName="IceWorld"),        
        WorldClass_LavaWorld     	UMETA(DisplayName="LavaWorld"),
        WorldClass_MechanicalWorld	UMETA(DisplayName="MechanicalWorld"),
        WorldClass_RedWorld      	UMETA(DisplayName="RedWorld"),
        WorldClass_RockyWorld    	UMETA(DisplayName="RockyWorld"),
        WorldClass_WaterWorld    	UMETA(DisplayName="WaterWorld"),
        WorldClass_WillowWorld   	UMETA(DisplayName="WillowWorld")	 
};