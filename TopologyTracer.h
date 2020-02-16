// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <type_traits>
#include <vector>
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CollisionQueryParams.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Util.h"
#include "CollisionChannels.h"
#include "Heuristics.h"
#include "AStar.h"
#include "GridGraph.h"
#include "Node.h"
#include "TopologyTracer.generated.h"

template<class NodeT, typename PriorityT, class CenterVector, 
	class BoundsVector, class ScaleVector, class HeuristicT, 
	uint16_t Connectors, uint16_t MaxReachableElevation,
	VALIDATE(IsGraphNode, NodeT)=0,
	VALIDATE(IsVector3, CenterVector)=0,
	VALIDATE(IsVector3, BoundsVector)=0,
	VALIDATE(IsVector3, ScaleVector)=0,
	VALIDATE(IsHeuristic, HeuristicT)=0>
struct TopologyTracer {
	using Node = NodeT;
	using Priority = PriorityT;
	using Center = CenterVector;
	using Bounds = BoundsVector;
	using GraphDims_NU = typename ScalarMul<Bounds, 2>::type;
	using GraphDims_WU = typename VectorMul<GraphDims_NU, ScaleVector>::type;
	
	using Scale = ScaleVector;
	using Graph = GridGraph<NodeT, PriorityT, HeuristicT, GraphDims_NU, Connectors>;
	static constexpr uint16_t max_reachable_elevation = MaxReachableElevation;
	static constexpr uint16_t connectors = Connectors;

	static constexpr uint16_t node_count_x = GraphDims_NU::x;
	static constexpr uint16_t node_count_y = GraphDims_NU::y;
	
	static constexpr uint16_t node_count = node_count_x * node_count_y;

	static constexpr uint16_t zone_count = 1 << NodeT::BitFieldSizes::ZONE_ID;
	static constexpr uint16_t zone_count_sqrt = 1 << (NodeT::BitFieldSizes::ZONE_ID - 1);

	static_assert(
		GraphDims_NU::x % zone_count_sqrt == 0 &&
		GraphDims_NU::y % zone_count_sqrt == 0 &&
		GraphDims_NU::z % zone_count_sqrt == 0 && ,
		"Incompatible node dimensions and zone count specification");

	using ZoneDims_NU = ScalarDiv<GraphDims_NU, zone_count_sqrt>::type;

	static constexpr uint16_t zone_node_count_x = 1 << NodeT::BitFieldSizes::ZONE_ID;
	static constexpr int16_t top_z = Center::z + Bounds::z;
	static constexpr int16_t bot_z = Center::z - Bounds::z;
	

	using TopLeftPoint = typename VectorAddLD<Center, typename VectorMul<Bounds, ScaleVector>::type >::type;
	using BottomLeftPoint = typename VectorSub<Center, typename VectorMul<Bounds, ScaleVector>::type >::type;
};


UCLASS()
class LABYRINTHDESCENT_API ATopologyTracer : public AActor {

	GENERATED_BODY()

public:
	using Tracer = TopologyTracer<
		GridNode,				// node type
		uint16_t,				// priority type
		Vector3<0, 0, 0>,		// center
		Vector3<64, 64, 200>,	// bounds
		Vector3<32, 32, 1>,		// scale
		Octile<GridNode>,		// heuristic
		8,						// connectors
		10>;					// max reachable elevation 

	ATopologyTracer();
	~ATopologyTracer();
	void Trace();
	void DebugDrawGraph(const float _time) const;
	void RequestPath(const FVector& _start, const FVector& _end) const;
	void DebugDrawPath(const std::vector<Tracer::Node const *>& _path, const float _time) const;

	FORCEINLINE Tracer::Node const * const GetNearestNode(const FVector& _target) const {
		uint16_t node_id = static_cast<uint16_t>(
			(static_cast<uint16_t>((_target.Y - Tracer::BottomLeftPoint::y) / m_node_scaling.Y)) * Tracer::node_count_x +
			(_target.X - Tracer::BottomLeftPoint::x) / m_node_scaling.X
		);
		return &m_base_graph->m_nodes[node_id];
	};

protected:
	virtual void BeginPlay() override;

private:
	Tracer::Graph* m_base_graph	{ nullptr };
	AStar<Tracer::Graph> m_astar;

	static const FVector m_node_scaling;
	static const uint16_t m_path_size_reservation = 64;
};