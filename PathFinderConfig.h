#pragma once
#include "Util.h"
#include "Node.h"
#include "GridGraph.h"
#include "Heuristics.h"

#define CENTER_WU_VEC3 0, 0, 0
#define BOUNDS_WU_VEC3 120, 120, 200
#define WU_PER_NU_VEC3 16, 16, 1
#define HEURISTIC Octile<GridNode>
#define CONNECTORS 8
#define MAX_ELEVATION 10

#define DRAW_GRAPH 0
#define DRAW_FRONTIER 0
#define DRAW_PATH 0
#define LOG_PATH_DIAGNOSTICS 0
#define LOG_PATH_VECTOR 0


template<class Node_, typename Priority_, class Center_WU_,
	class Bounds_WU_, class WU_per_NU_, class Heuristic_,
	uint16_t Connectors, uint16_t MaxReachableElevation_WU,
	VALIDATE(IsGraphNode, Node_) = 0,
	VALIDATE(IsVector3, Center_WU_) = 0,
	VALIDATE(IsVector3, Bounds_WU_) = 0,
	VALIDATE(IsVector3, WU_per_NU_) = 0,
	VALIDATE(IsHeuristic, Heuristic_) = 0>
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

using EnabledPathFinderConfig = PathFinderConfig <
	GridNode,				// node type
	uint16_t,				// priority type
	Vector3<0, 0, 0>,		// center
	Vector3<120, 120, 200>,	// bounds
	Vector3<16, 16, 1>,		// scale
	Octile<GridNode>,		// heuristic
	8,						// connectors
	10>;					// max reachable elevation 