// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CollisionChannels.h"
#include "GameFramework/Actor.h"
#include "Math/UnrealMathUtility.h"
#include "TopologyTracer.h"
#include "PathFinderConfig.h"
#include "Mob.generated.h"

class ALiveGameHandler;
class AMyPlayerController;

UCLASS()
class LABYRINTHDESCENT_API AMob : public AActor
{
	GENERATED_BODY()
	
public:	
	AMob();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	void initializeMob(ATopologyTracer const * _topology_tracer, const uint8_t _spawn_zone_id);

private:
	void generateNewTargetNode();


private:
	friend class ALiveGameHandler;
	friend class AMyPlayerController;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent * m_base_mesh;

	ATopologyTracer const * m_topology_tracer;

	uint32_t m_new_target_tick_frequency;
	uint32_t m_ticks;
	uint8_t m_spawn_zone_id;
	bool m_is_initialized = false;

	std::vector<EnabledPathFinderConfig::Node const *> m_current_path;
};
