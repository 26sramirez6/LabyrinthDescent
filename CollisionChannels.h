#pragma once
#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"

struct CollisionChannels {
	static constexpr ECollisionChannel Topology = ECC_GameTraceChannel1;
	static constexpr ECollisionChannel VisibleUnit = ECC_GameTraceChannel2;
};