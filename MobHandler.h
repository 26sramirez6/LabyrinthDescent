// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Mob.h"
#include "TopologyTracer.h"
#include "PathFinderConfig.h"
#include "MobHandlerSettings.h"
#include "MobHandler.generated.h"

UCLASS()
class LABYRINTHDESCENT_API AMobHandler : public AActor
{
	GENERATED_BODY()
	
public:	
	AMobHandler();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	template<uint16_t _count_per_zone_x, uint16_t _count_per_zone_y>
	void SpawnMobsAllZones();

	FORCEINLINE	void 
	initializeMobHandler(ATopologyTracer const * _topology_tracer) {
		m_topology_tracer = _topology_tracer;
		m_is_initialized = true;
	}

private:
	AMob ** m_mobs;
	ATopologyTracer const * m_topology_tracer;
	bool m_is_initialized = false;
};
