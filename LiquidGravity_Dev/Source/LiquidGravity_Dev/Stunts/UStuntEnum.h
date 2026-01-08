#pragma once

#include "UStuntEnum.generated.h"

UENUM(BlueprintType)
enum class UStuntEnum : uint8
{
	ForwardBackward,
	BackwardForward,
	LeftRight,
	RightLeft,
	GagnamStyle UMETA(Hidden),
};
