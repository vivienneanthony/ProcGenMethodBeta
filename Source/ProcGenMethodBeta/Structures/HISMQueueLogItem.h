#pragma once

// ProcGenMethodBeta Plugin
// Developer Vivienne Anthony
// Copyright (c) 2021 

#include "HISMQueueLogItem.generated.h"

USTRUCT()
struct FHISMQueueLogItem
{
    GENERATED_BODY()

public:
    int32 SelectedHISM;
    FVector Location;    
    int32 InstanceID;
    bool Generate;
};