#pragma once

// ProcGenMethodBeta Plugin
// Developer Vivienne Anthony
// Copyright (c) 2021 


UENUM(BlueprintType)			//"BlueprintType" is essential to include
enum class EWorldTrace : uint8
{
        WorldTrace_FlatWorld	 	    UMETA(DisplayName="FlatWorld"),
        WorldTrace_SphericalWorld 	 	UMETA(DisplayName="SphericalWorld")    
};