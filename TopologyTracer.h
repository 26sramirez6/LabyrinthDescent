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
#include "Heuristics.h"
#include "GridGraph.h"
#include "Node.h"
#include "TopologyTracer.generated.h"

template<class NodeT, typename PriorityT, class CenterVector, 
	class BoundsVector, class HeuristicT, unsigned Connectors,
	VALIDATE(IsGraphNode, NodeT)=0,
	VALIDATE(IsVector3, CenterVector)=0,
	VALIDATE(IsVector3, BoundsVector)=0,
	VALIDATE(IsHeuristic, HeuristicT)=0>
struct TopologyTracer {
	using Priority = PriorityT;
	using Node = NodeT;
	using Center = CenterVector;
	using Bounds = BoundsVector;
	using Dims = typename ScalarMul<Bounds, 2>::type;
	using Graph = GridGraph<NodeT, PriorityT, HeuristicT, Dims, Connectors>;
	static constexpr uint64_t node_count_x = Dims::x;
	static constexpr uint64_t node_count_y = Dims::y;
	static constexpr uint64_t node_count = node_count_x * node_count_y;
	//static constexpr uint64_t node_count_sqrt = CompileTimeSqrt(node_count);
	static constexpr int top_z = Center::z + Bounds::z;
	static constexpr int bot_z = Center::z - Bounds::z;
	using TopLeftPoint = typename VectorAddLD<Center, Bounds>::type;
	using BottomLeftPoint = typename VectorSub<Center, Bounds>::type;
};


UCLASS()
class LABYRINTHDESCENT_API ATopologyTracer : public AActor {

	GENERATED_BODY()

public:
	using Tracer = TopologyTracer<GridNode, unsigned, Vector3<0, 0, 0>, 
		Vector3<100, 100, 200>, Manhattan<GridNode>, 8>;
	FVector m_world_center;
	FVector m_world_bounds;

	ATopologyTracer(const FObjectInitializer& ObjectInitializer)
		: Super(ObjectInitializer), 
		m_world_center(Tracer::Center::x, Tracer::Center::y, Tracer::Center::z),
		m_world_bounds(Tracer::Bounds::x, Tracer::Bounds::y, Tracer::Bounds::z) {}

	void
	Trace() {//GridGraph * graph
		UE_LOG(LogTemp, Log, TEXT("Performing Trace"));
		UWorld * world = this->GetWorld();
		FHitResult out_hit;
		const FCollisionQueryParams _query;
		const FCollisionResponseParams _response;
		FColor _color;

		for (unsigned i = 0; i < Tracer::node_count_y; i++) {
			for (unsigned j = 0; j < Tracer::node_count_x; j++) {
				const unsigned _x = Tracer::BottomLeftPoint::x + j;
				const unsigned _y = Tracer::BottomLeftPoint::y + i;
				const FVector _start = FVector(_x, _y, Tracer::top_z);
				const FVector _end = FVector(_x, _y, Tracer::bot_z);
				//nodes[j + i*Tracer::node_count_x].SetLocation();

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
	}

protected:
	virtual void BeginPlay() override {
		Super::BeginPlay();
		Trace();
	};
};
