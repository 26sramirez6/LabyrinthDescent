// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TopologyTracer.generated.h"

UCLASS()
template<unsigned NodeCount = 1024> 
class LABYRINTHDESCENT_API ATopologyTracer : public AActor
{
	GENERATED_BODY()
  
public:	
	static constexpr unsigned node_count = NodeCount;
	FVector m_world_center;
	FVector m_world_bounds;

	ATopologyTracer(const FObjectInitializer& ObjectInitializer)
		: Super(ObjectInitializer), m_center(0,0,0), m_bounds(500,500,500) {
	}

protected:
	virtual void BeginPlay() override {
		UWorld * world = this->GetWorld();
		FHitResult out_hit;
		
		for (unsigned i = 0; i < NodeCount; i++) {
			world->LineTraceSingleByChannel(
				out_hit,
				const FVector & Start,
				const FVector & End,
				ECollisionChannel TraceChannel,
				const FCollisionQueryParams & Params,
				const FCollisionResponseParams & ResponseParam);
		}
	};

public:	
	virtual void Tick(float DeltaTime) override;
};
