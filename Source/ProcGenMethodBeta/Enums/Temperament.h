#pragma once

// ProcGenMethodBeta Plugin
// Developer Vivienne Anthony
// Copyright (c) 2021 


UENUM(BlueprintType)	//"BlueprintType" is essential to include
enum class ETemperaments : uint8
{
        Temperament_Anonymous   UMETA(DisplayName="AnonymousTemperant"),
        Temperament_Chaos       UMETA(DisplayName="RainyTemperant"),
        Temperament_Clear    	UMETA(DisplayName="ClearSkyTemperant"),
        Temperament_Cloudy 	UMETA(DisplayName="CloudySkyTemperant"),
        Temperament_Electric    UMETA(DisplayName="EletricTemperant"),        
        Temperament_Misty 	UMETA(DisplayName="MistyTemperant"),
        Temperament_Rainy       UMETA(DisplayName="RainyTemperant"),        
        Temperament_Volcanic    UMETA(DisplayName="VolcanicTemperant")            
};
