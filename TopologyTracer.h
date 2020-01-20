// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Util.h"
#include "TopologyTracer.generated.h"

UCLASS()
template<Vector3 Center, Vector3 Bounds>
class LABYRINTHDESCENT_API ATopologyTracer : public AActor
{
	GENERATED_BODY()
  
public:	
	static constexpr uint64_t node_count = Bounds::max * Bounds::max;
	static constexpr uint64_t node_count_sqrt = CompileTimeSqrt(node_count);
	static constexpr int top_z = Center.z + Bounds.z;
	static constexpr int bot_z = Center.z - Bounds.z;
	using TopLeftPoint = Vector3<Center.x + Bounds.x, Center.y + Bounds.y, Center.z + Bounds.z>;
	using BottomLeftPoint = Vector3<Center.x - Bounds.x, Center.y - Bounds.y, Center.z - Bounds.z>;

	FVector m_world_center;
	FVector m_world_bounds;

	ATopologyTracer(const FObjectInitializer& ObjectInitializer)
		: Super(ObjectInitializer), 
		m_center(Center.x, Center.y, Center.z), m_bounds(Bounds.x, Bounds.y, Bounds.z) {
	}

	void Trace() {
		UWorld * world = this->GetWorld();
		FHitResult out_hit;
		const FCollisionQueryParams _query;
		const FCollisionResponseParams _response;
		for (unsigned i = 0; i < node_count_sqrt; i++) {
			for (unsigned j = 0; j < node_count_sqrt; j++) {
				const x = BottomLeftPoint.x + i;
				const y = BottomLeftPoint.y + j;
				const FVector _start = FVector(x, y, top_z);
				const FVector _end = FVector(x, y, bot_z);
				world->LineTraceSingleByChannel(
					out_hit,
					_start,
					_end,
					ECollisionChannel.ECC_Visibility,
					_query,
					_response);
			}
		}
	}

protected:
	virtual void BeginPlay() override {
		Trace();
	};
};
