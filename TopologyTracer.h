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
#include "PathFinderConfig.h"
#include "CollisionChannels.h"
#include "Heuristics.h"
#include "AStar.h"
#include "GridGraph.h"
#include "Node.h"
#include "TopologyTracer.generated.h"


UCLASS()
class LABYRINTHDESCENT_API ATopologyTracer : public AActor {

	GENERATED_BODY()

public:
	ATopologyTracer();
	~ATopologyTracer();
	void trace();
	void debugDrawGraph(const float _time) const;
	void debugDrawPath(const std::vector<EnabledPathFinderConfig::Node const *>& _path, const float _time) const;
	static void debugLogConfig();

	FORCEINLINE EnabledPathFinderConfig::Node const * const 
	getNearestNode(const FVector& _target) const {
		const uint8_t _zone_id = getNearestZone(_target);
		const uint8_t _dx_zu = _zone_id % EnabledPathFinderConfig::zone_count_x; // zones up
		const uint8_t _dy_zu = _zone_id / EnabledPathFinderConfig::zone_count_x; // zones to the right
		const int16_t _zone_blp_wu_x = EnabledPathFinderConfig::BottomLeftPoint_WU::x + _dx_zu * EnabledPathFinderConfig::ZoneDims_WU::x;
		const int16_t _zone_blp_wu_y = EnabledPathFinderConfig::BottomLeftPoint_WU::y + _dy_zu * EnabledPathFinderConfig::ZoneDims_WU::y;

		const uint16_t _node_id_within_zone = static_cast<uint16_t>(
			static_cast<uint16_t>((_target.Y - _zone_blp_wu_y) / m_wu_per_nu_f.Y) * EnabledPathFinderConfig::zone_node_count_x +
			(_target.X - _zone_blp_wu_x) / m_wu_per_nu_f.X
		);
		const uint16_t _node_id = _zone_id * EnabledPathFinderConfig::zone_node_count + _node_id_within_zone;
		return &m_base_graph->m_nodes[_node_id];
	};

	FORCEINLINE uint8_t
	getNearestZone(const FVector& _target) const {
		const float _dy_wu = _target.Y - EnabledPathFinderConfig::BottomLeftPoint_WU::y;
		const float _dx_wu = _target.X - EnabledPathFinderConfig::BottomLeftPoint_WU::x;
		uint8_t _dy_zu = static_cast<uint8_t>(_dy_wu / m_ZoneDims_WU_f.Y) * EnabledPathFinderConfig::zone_count_x;
		uint8_t _dx_zu = static_cast<uint8_t>(_dx_wu / m_ZoneDims_WU_f.X);
		return _dy_zu + _dx_zu;
	};

	FORCEINLINE EnabledPathFinderConfig::Node const * const
	getRandomNodeInZone(const uint8_t _zone_id) const {
		const uint16_t _min_node_id = EnabledPathFinderConfig::zone_node_count * _zone_id;
		const uint16_t _max_node_id = _min_node_id + EnabledPathFinderConfig::zone_node_count;
		return &m_base_graph->m_nodes[FMath::RandRange(_min_node_id, _max_node_id)];
	};

	FORCEINLINE void
	requestPath(
		EnabledPathFinderConfig::Node const * const _start_node,
		EnabledPathFinderConfig::Node const * const _end_node) const {
		requestPathImp(_start_node, _end_node);
	};

	FORCEINLINE void
	requestPath(const FVector& _start_vec, const FVector& _end_vec) const {
		EnabledPathFinderConfig::Node const * const _start_node = getNearestNode(_start_vec);
		EnabledPathFinderConfig::Node const * const _end_node = getNearestNode(_end_vec);
		requestPathImp(_start_node, _end_node);
	};

	FORCEINLINE void
	requestPath(const FVector& _start_vec, EnabledPathFinderConfig::Node const * const _end_node) const {
		EnabledPathFinderConfig::Node const * const _start_node = getNearestNode(_start_vec);
		requestPathImp(_start_node, _end_node);
	};

	FORCEINLINE void
	requestPath(EnabledPathFinderConfig::Node const * const _start_node, const FVector& _end_vec) const {
		EnabledPathFinderConfig::Node const * const _end_node = getNearestNode(_end_vec);
		requestPathImp(_start_node, _end_node);
	};

private:
	void requestPathImp(
		EnabledPathFinderConfig::Node const * const _start_node,
		EnabledPathFinderConfig::Node const * const _end_node) const;

protected:
	virtual void BeginPlay() override;

private:
	EnabledPathFinderConfig::Graph* m_base_graph { nullptr };
	AStar<EnabledPathFinderConfig::Graph> m_astar;

	static const FVector m_wu_per_nu_f;
	static const FVector m_ZoneDims_WU_f;
	static const uint16_t m_path_size_reservation = EnabledPathFinderConfig::zone_node_count;
};