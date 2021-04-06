#pragma once

// ProcGenMethodBeta Plugin
// Developer Vivienne Anthony
// Copyright (c) 2021 

#include "TraceCall.generated.h"

USTRUCT()
struct FTraceCall
{
    GENERATED_BODY()

public:
    FVector Start;
    FVector End;
    float Scale;
    int32 Pick;
};