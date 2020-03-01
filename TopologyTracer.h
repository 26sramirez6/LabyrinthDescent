// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <type_traits>
#include <vector>
#include <unordered_map>
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CollisionQueryParams.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Util.h"
#include "PathSettings.h"
#include "CollisionChannels.h"
#include "Heuristics.h"
#include "AStar.h"
#include "GridGraph.h"
#include "Node.h"
#include "TopologyTracer.generated.h"

template<class Node_, typename Priority_, class Center_WU_, 
	class Bounds_WU_, class WU_per_NU_, class Heuristic_,
	uint16_t Connectors, uint16_t MaxReachableElevation_WU,
	VALIDATE(IsGraphNode, Node_)=0,
	VALIDATE(IsVector3, Center_WU_)=0,
	VALIDATE(IsVector3, Bounds_WU_)=0,
	VALIDATE(IsVector3, WU_per_NU_)=0,
	VALIDATE(IsHeuristic, Heuristic_)=0>
struct PathFinderConfig {
	using Node = Node_;
	using Priority = Priority_;
	using Center_WU = Center_WU_;
	using Bounds_WU = Bounds_WU_;
	using GraphDims_NU = typename ScalarMul<Bounds_WU, 2>::type;
	using GraphDims_WU = typename VectorMul<GraphDims_NU, WU_per_NU_>::type;
	
	using WU_per_NU = WU_per_NU_;
	static constexpr uint16_t max_reachable_elevation_WU = MaxReachableElevation_WU;
	static constexpr uint16_t connectors = Connectors;

	static constexpr uint16_t node_count_x = GraphDims_NU::x;
	static constexpr uint16_t node_count_y = GraphDims_NU::y;
	
	static constexpr uint16_t node_count = node_count_x * node_count_y;
	// TODO: if the node determines the zone count, we can't have zone_count_x != zone_count_y ?
	static constexpr uint8_t zone_count = 0x1 << Node_::BitFieldSizes::ZONE_ID;
	static constexpr uint8_t zone_count_sqrt = CompileTimeSqrtU64(zone_count);
	static constexpr uint8_t zone_count_x = zone_count_sqrt;
	static constexpr uint8_t zone_count_y = zone_count_sqrt;
	
	using GraphDims_ZU = Vector3<zone_count_x, zone_count_y, 1>;

	static_assert(
		GraphDims_NU::x % zone_count_sqrt == 0 &&
		GraphDims_NU::y % zone_count_sqrt == 0 &&
		GraphDims_NU::z % zone_count_sqrt == 0,
		"Incompatible node dimensions and zone count specification");

	using ZoneDims_NU = typename ScalarDiv<GraphDims_NU, zone_count_sqrt>::type;

	static constexpr uint16_t zone_node_count_x = ZoneDims_NU::x;
	static constexpr uint16_t zone_node_count_y = ZoneDims_NU::y;
	static constexpr uint16_t zone_node_count = zone_node_count_x * zone_node_count_y;
	using ZoneDims_WU = typename VectorMul<ZoneDims_NU, WU_per_NU>::type;

	static constexpr int16_t top_z_WU = Center_WU::z + Bounds_WU::z;
	static constexpr int16_t bot_z_WU = Center_WU::z - Bounds_WU::z;

	using TopLeftPoint_WU = typename VectorAddLD<Center_WU, 
		typename VectorMul<Bounds_WU, WU_per_NU_>::type >::type;
	using BottomLeftPoint_WU = typename VectorSub<Center_WU, 
		typename VectorMul<Bounds_WU, WU_per_NU_>::type >::type;

	using Graph = GridGraph<Node_, Priority_, Heuristic_, 
		GraphDims_NU, ZoneDims_NU, GraphDims_ZU, Connectors>;
	
	template<uint8_t _zone_id>
	struct GetZoneBLP_WU {
		using type = Vector2<
			BottomLeftPoint_WU::x + (_zone_id % zone_count_x) * ZoneDims_WU::x,
			BottomLeftPoint_WU::y + (_zone_id / zone_count_x) * ZoneDims_WU::y>;
	};

	using TupleZoneBLP_WU = typename TupleForRange<0, zone_count, GetZoneBLP_WU>::type;


};


UCLASS()
class LABYRINTHDESCENT_API ATopologyTracer : public AActor {

	GENERATED_BODY()

public:
	using Tracer = PathFinderConfig <
		GridNode,				// node type
		uint16_t,				// priority type
		Vector3<0, 0, 0>,		// center
		Vector3<120, 120, 200>,	// bounds
		Vector3<16, 16, 1>,		// scale
		Octile<GridNode>,		// heuristic
		8,						// connectors
		10>;					// max reachable elevation 

	ATopologyTracer();
	~ATopologyTracer();
	void Trace();
	void DebugDrawGraph(const float _time) const;
	void RequestPath(const FVector& _start, const FVector& _end) const;
	void DebugDrawPath(const std::vector<Tracer::Node const *>& _path, const float _time) const;
	static void DebugLogConfig();

	FORCEINLINE Tracer::Node const * const 
	GetNearestNode(const FVector& _target) const {
		const uint8_t _zone_id = GetNearestZone(_target);
		const uint8_t _dx_zu = _zone_id % Tracer::zone_count_x; // zones up
		const uint8_t _dy_zu = _zone_id / Tracer::zone_count_x; // zones to the right
		const int16_t _zone_blp_wu_x = Tracer::BottomLeftPoint_WU::x + _dx_zu * Tracer::ZoneDims_WU::x;
		const int16_t _zone_blp_wu_y = Tracer::BottomLeftPoint_WU::y + _dy_zu * Tracer::ZoneDims_WU::y;

		const uint16_t _node_id_within_zone = static_cast<uint16_t>(
			static_cast<uint16_t>((_target.Y - _zone_blp_wu_y) / m_wu_per_nu_f.Y) * Tracer::zone_node_count_x +
			(_target.X - _zone_blp_wu_x) / m_wu_per_nu_f.X
		);
		const uint16_t _node_id = _zone_id * Tracer::zone_node_count + _node_id_within_zone;
		return &m_base_graph->m_nodes[_node_id];
	};

	FORCEINLINE uint8_t
	GetNearestZone(const FVector& _target) const {
		const float _dy_wu = _target.Y - Tracer::BottomLeftPoint_WU::y;
		const float _dx_wu = _target.X - Tracer::BottomLeftPoint_WU::x;
		uint8_t _dy_zu = static_cast<uint8_t>(_dy_wu / m_ZoneDims_WU_f.Y) * Tracer::zone_count_x;
		uint8_t _dx_zu = static_cast<uint8_t>(_dx_wu / m_ZoneDims_WU_f.X);
		return _dy_zu + _dx_zu;
	};

protected:
	virtual void BeginPlay() override;

private:
	Tracer::Graph* m_base_graph	{ nullptr };
	AStar<Tracer::Graph> m_astar;

	static const FVector m_wu_per_nu_f;
	static const FVector m_ZoneDims_WU_f;
	static const uint16_t m_path_size_reservation = 64;
};