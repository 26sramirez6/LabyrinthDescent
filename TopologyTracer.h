// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <type_traits>
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CollisionQueryParams.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Util.h"
#include "TopologyTracer.generated.h"

template<class CenterT, class BoundsT,
	VALIDATE(CenterT, IsVector3),
	VALIDATE(BoundsT, IsVector3)>
struct TopologyTracer {
	using Center = CenterT;
	using Bounds = BoundsT;
	static constexpr uint64_t node_count_x = Bounds::x * 2;
	static constexpr uint64_t node_count_y = Bounds::y * 2;
	static constexpr uint64_t node_count = node_count_x * node_count_y;
	//static constexpr uint64_t node_count_sqrt = CompileTimeSqrt(node_count);
	static constexpr int top_z = Center::z + Bounds::z;
	static constexpr int bot_z = Center::z - Bounds::z;
	using TopLeftPoint = Vector3<Center::x + Bounds::x, Center::y + Bounds::y, Center::z + Bounds::z>;
	using BottomLeftPoint = Vector3<Center::x - Bounds::x, Center::y - Bounds::y, Center::z - Bounds::z>;
};


UCLASS()
class LABYRINTHDESCENT_API ATopologyTracer : public AActor {

	GENERATED_BODY()

public:
	using Tracer = TopologyTracer<Vector3<0, 0, 0>, Vector3<100, 100, 200>>;
	FVector m_world_center;
	FVector m_world_bounds;

	//ATopologyTracer() {}
	ATopologyTracer(const FObjectInitializer& ObjectInitializer)
		: Super(ObjectInitializer), 
		m_world_center(Tracer::Center::x, Tracer::Center::y, Tracer::Center::z),
		m_world_bounds(Tracer::Bounds::x, Tracer::Bounds::y, Tracer::Bounds::z) {}

	void
	Trace() {
		UWorld * world = this->GetWorld();
		FHitResult out_hit;
		const FCollisionQueryParams _query;
		const FCollisionResponseParams _response;
		FColor _color;
		for (unsigned i = 0; i < Tracer::node_count_x; i++) {
			for (unsigned j = 0; j < Tracer::node_count_y; j++) {
				const unsigned _x = Tracer::BottomLeftPoint::x + i;
				const unsigned _y = Tracer::BottomLeftPoint::y + j;
				const FVector _start = FVector(_x, _y, Tracer::top_z);
				const FVector _end = FVector(_x, _y, Tracer::bot_z);
				if (world->LineTraceSingleByChannel(
					out_hit,
					_start,
					_end,
					ECC_Visibility,
					_query,
					_response)) {
					_color = FColor::Red;
				} else {
					_color = FColor::Blue;
				};

				DrawDebugLine(world, _start, _end, FColor::Blue,
					false, 50., ECC_WorldStatic, 1.);
			}
		}
		UE_LOG(LogTemp, Log, TEXT("Performing Trace"));
	}

protected:
	virtual void BeginPlay() override {
		Super::BeginPlay();
		Trace();
	};
};
