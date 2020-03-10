// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PathFinderConfig.h"
#include "StateHandler.h"
#include "GameFramework/Actor.h"
#include "UObject/ConstructorHelpers.h"
#include "CollisionChannels.h"
#include "UserCharacterTarget.h"
#include "UserCharacter.generated.h"

UCLASS()
class LABYRINTHDESCENT_API AUserCharacter : public AActor
{
	GENERATED_BODY()
	
public:	
	AUserCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE std::vector<EnabledPathFinderConfig::Node const *> &
	getPathBuffer() { return m_current_path; }

	FORCEINLINE void
	alertPathFound() { m_queue_new_path = true; }

private:
	friend class ALiveGameHandler;

	FORCEINLINE void 
	setTargetObject(AUserCharacterTarget const * _target) { m_target = _target; }

	void tickStartOnPath(const float _delta_time);
	void tickContinueOnPath(const float _delta_time);
	void tickEndPath(const float _delta_time);
	void tickIdle(const float _delta_time);
	

private:
	UPROPERTY(VisibleAnywhere)
	AUserCharacterTarget const * m_target;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* m_base_mesh;

	std::vector<EnabledPathFinderConfig::Node const *> m_current_path;

	StateTypes m_current_state = StateTypes::Idle;

	bool m_queue_new_path = false;
	uint16_t m_current_waypoint = 0;
	EnabledPathFinderConfig::Node const * m_current_waypoint_node;
	FVector m_current_waypoint_vec;

protected:
	float m_speed = 1.f;
};
