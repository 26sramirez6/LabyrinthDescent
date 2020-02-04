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
	class BoundsVector, class ScaleVector, class HeuristicT, uint16_t Connectors,
	VALIDATE(IsGraphNode, NodeT)=0,
	VALIDATE(IsVector3, CenterVector)=0,
	VALIDATE(IsVector3, BoundsVector)=0,
	VALIDATE(IsVector3, ScaleVector)=0,
	VALIDATE(IsHeuristic, HeuristicT)=0>
struct TopologyTracer {
	using Priority = PriorityT;
	using Node = NodeT;
	using Center = CenterVector;
	using Bounds = BoundsVector;
	using NodeDims = typename ScalarMul<Bounds, 2>::type;
	using Scale = ScaleVector;
	using Graph = GridGraph<NodeT, PriorityT, HeuristicT, NodeDims, Connectors>;
	static constexpr uint16_t node_count_x = NodeDims::x;
	static constexpr uint16_t node_count_y = NodeDims::y;
	static constexpr uint16_t node_count = node_count_x * node_count_y;
	static constexpr int top_z = Center::z + Bounds::z;
	static constexpr int bot_z = Center::z - Bounds::z;
	using TopLeftPoint = typename VectorAddLD<Center, typename VectorMul<Bounds, ScaleVector>::type >::type;
	using BottomLeftPoint = typename VectorSub<Center, typename VectorMul<Bounds, ScaleVector>::type >::type;
};


UCLASS()
class LABYRINTHDESCENT_API ATopologyTracer : public AActor {

	GENERATED_BODY()

public:
	using Tracer = TopologyTracer<
		GridNode, 
		uint16_t, 
		Vector3<0, 0, 0>, 
		Vector3<50, 50, 200>, 
		Vector3<10, 10, 1>, 
		Manhattan<GridNode>, 
		8>;

	FVector m_world_center;
	FVector m_world_bounds;

	ATopologyTracer(const FObjectInitializer& ObjectInitializer)
		: Super(ObjectInitializer), 
		m_world_center(Tracer::Center::x, Tracer::Center::y, Tracer::Center::z),
		m_world_bounds(Tracer::Bounds::x, Tracer::Bounds::y, Tracer::Bounds::z) {}

	void
	Trace(Tracer::Graph * graph) {
		UE_LOG(LogTemp, Log, TEXT("Performing Trace"));
		UWorld * world = this->GetWorld();
		FHitResult out_hit;
		const FCollisionQueryParams _query;
		const FCollisionResponseParams _response;
		FColor _color;

		for (uint16_t i = 0; i < Tracer::node_count_y*Tracer::Scale::y; i+=Tracer::Scale::y) {
			for (uint16_t j = 0; j < Tracer::node_count_x*Tracer::Scale::x; j+=Tracer::Scale::x) {
				float _x = Tracer::BottomLeftPoint::x + j;
				float _y = Tracer::BottomLeftPoint::y + i;
				const FVector _start( _x, _y, Tracer::top_z );
				const FVector _end( _x, _y, Tracer::bot_z );
				Tracer::Node node = graph->m_nodes[j + i * Tracer::node_count_x];
				if (world->LineTraceSingleByChannel(
					out_hit,
					_start,
					_end,
					ECC_Camera,
					_query,
					_response)) {
					// float->int16. pros/cons of keeping in int16 vs float
					node.SetLocation(_x, _y, out_hit.Location.Z);
					node.is_reachable = true;
					_color = FColor::Red;
				} else {
					node.SetLocation(_x, _y, 0);
					node.is_reachable = false;
					_color = FColor::Blue;
				};
				FVector point;
				node.ToVector(point);
				DrawDebugPoint(world, point, 5., _color, false, 50.);
			}
		}
	}

protected:
	virtual void BeginPlay() override {
		Super::BeginPlay();
		Trace();
	};
};
