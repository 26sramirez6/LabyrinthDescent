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
	
	static constexpr uint16_t connectors = Connectors;
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

	ATopologyTracer(const FObjectInitializer& ObjectInitializer)
		: Super(ObjectInitializer) {
		UE_LOG(LogTemp, Log, TEXT("Constructing Tracer"));
		m_base_graph = new Tracer::Graph();
	}

	~ATopologyTracer() {
		if (m_base_graph) {
			delete m_base_graph;
		}
	}

	void Trace() {
		UE_LOG(LogTemp, Log, TEXT("Performing Trace"));
		UWorld * world = this->GetWorld();
		FHitResult out_hit;
		const FCollisionQueryParams _query;
		const FCollisionResponseParams _response;
		FColor _color;
		uint16_t id = 0;
		for (uint16_t i = 0; i < Tracer::node_count_y*Tracer::Scale::y; i+=Tracer::Scale::y) {
			for (uint16_t j = 0; j < Tracer::node_count_x*Tracer::Scale::x; j+=Tracer::Scale::x, id++) {
				const float _x = Tracer::BottomLeftPoint::x + j;
				const float _y = Tracer::BottomLeftPoint::y + i;
				const FVector _start( _x, _y, Tracer::top_z );
				const FVector _end( _x, _y, Tracer::bot_z );
				if (world->LineTraceSingleByChannel(
					out_hit,
					_start,
					_end,
					ECC_Camera,
					_query,
					_response)) {
					// float->int16. pros/cons of keeping in int16 vs float
					m_base_graph->m_nodes[id].SetLocation(_x, _y, out_hit.Location.Z);
					m_base_graph->m_nodes[id].is_reachable = true;
					_color = FColor::Red;
				} else {
					m_base_graph->m_nodes[id].SetLocation(_x, _y, 20);
					m_base_graph->m_nodes[id].is_reachable = false;
					_color = FColor::Blue;
				};
				FVector point;
				m_base_graph->m_nodes[id].ToVector(point);
				UE_LOG(LogTemp, Log, TEXT("TRACE: Drawing node %d, %p at %s"), m_base_graph->m_nodes[id].id, &m_base_graph->m_nodes[id], *point.ToString());
				//DrawDebugPoint(world, point, 5.f, _color, false, 50.);
				//UE_LOG(LogTemp, Log, TEXT("Drew debug point"));
			}
		}
	}

	void DebugDrawGraph(float time) {
		UWorld * world = GetWorld();
		if (IsValid(world)) {
			FVector start, end;
			for (uint16_t i = 0; i < Tracer::node_count; i++) {
				Tracer::Node * next = m_base_graph->m_connectors[i];
				m_base_graph->m_nodes[i].ToVector(start);
				DrawDebugPoint(world, start, 5.f, m_base_graph->m_nodes[i].is_reachable ? FColor::Red : FColor::Blue, false, time);
				UE_LOG(LogTemp, Log, TEXT("DEBUGDRAWGRAPH: Drawing node %d, %p at %s"), m_base_graph->m_nodes[i].id, &m_base_graph->m_nodes[i], *start.ToString());

				const uint32_t _neighbor_id = i * Tracer::Graph::connectors;
				for (uint16_t j = 0; j < Tracer::Graph::connectors; j++) {
					Tracer::Node * next = m_base_graph->m_connectors[_neighbor_id + j];
					if (next) {
						next->ToVector(end);
						DrawDebugLine(world, start, end, m_base_graph->m_nodes[i].is_reachable ? FColor::Blue : FColor::Blue, false, time);
					}
				}
			}
		}
	}

protected:
	virtual void BeginPlay() override {
		Super::BeginPlay();
	};

private:
	ATopologyTracer::Tracer::Graph* m_base_graph{ nullptr };
};
