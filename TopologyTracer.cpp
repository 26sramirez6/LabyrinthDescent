// Fill out your copyright notice in the Description page of Project Settings.

#include "TopologyTracer.h"

ATopologyTracer::ATopologyTracer() {
	UE_LOG(LogTemp, Log, TEXT("Constructing EnabledPathFinderConfig"));
	ATopologyTracer::debugLogConfig();
	bBlockInput = true;
	bHidden = true;
	bIgnoresOriginShifting = true;
	/*bLockLocation = true; causes launch failure?*/
	SetActorEnableCollision(false);
	m_base_graph = new EnabledPathFinderConfig::Graph();
}

ATopologyTracer::~ATopologyTracer() {
	if (m_base_graph) {
		delete m_base_graph;
	}
}

void ATopologyTracer::BeginPlay() {
	Super::BeginPlay();
};


void ATopologyTracer::trace() {
	UE_LOG(LogTemp, Log, TEXT("Performing trace"));
	UWorld * world = this->GetWorld();
	FHitResult out_hit;
	const FCollisionQueryParams _query;
	const FCollisionResponseParams _response;
	FColor _color;
	EnabledPathFinderConfig::Node * current;
	for (uint8_t z = 0; z < EnabledPathFinderConfig::zone_count; z++) {
		const uint8_t _dx_zu = z % EnabledPathFinderConfig::zone_count_x; // zones up
		const uint8_t _dy_zu = z / EnabledPathFinderConfig::zone_count_x; // zones to the right, int division intended
		const int16_t _zone_blp_wu_x = EnabledPathFinderConfig::BottomLeftPoint_WU::x + _dx_zu * EnabledPathFinderConfig::ZoneDims_WU::x;
		const int16_t _zone_blp_wu_y = EnabledPathFinderConfig::BottomLeftPoint_WU::y + _dy_zu * EnabledPathFinderConfig::ZoneDims_WU::y;
		const uint16_t _first_idx_in_zone = z * EnabledPathFinderConfig::zone_node_count;
		uint16_t inner_zone_idx = 0;
		for (uint16_t i_wu = 0; i_wu < EnabledPathFinderConfig::zone_node_count_y*EnabledPathFinderConfig::WU_per_NU::y; i_wu += EnabledPathFinderConfig::WU_per_NU::y) {
			for (uint16_t j_wu = 0; j_wu < EnabledPathFinderConfig::zone_node_count_x*EnabledPathFinderConfig::WU_per_NU::x; j_wu += EnabledPathFinderConfig::WU_per_NU::x) {
				const uint16_t _node_id = _first_idx_in_zone + inner_zone_idx++;
				const float _x_wu = _zone_blp_wu_x + j_wu;
				const float _y_wu = _zone_blp_wu_y + i_wu;
				const FVector _start(_x_wu, _y_wu, EnabledPathFinderConfig::top_z_WU);
				const FVector _end(_x_wu, _y_wu, EnabledPathFinderConfig::bot_z_WU);
				current = &m_base_graph->m_nodes[_node_id];
				current->is_off_topology = false;
				if (world->LineTraceSingleByChannel(
					out_hit,
					_start,
					_end,
					CollisionChannels::Topology,
					_query,
					_response)) {

					current->SetLocation(_x_wu, _y_wu, out_hit.Location.Z);
					current->is_reachable = out_hit.Location.Z < EnabledPathFinderConfig::max_reachable_elevation_WU;
					_color = FColor::Red;
				} else {
					current->SetLocation(_x_wu, _y_wu, 50);
					current->is_off_topology = true;
					_color = FColor::Blue;
				};
				FVector point;
				current->ToVector(point);
			}
		}
	}
}

void ATopologyTracer::debugDrawGraph(const float _time) const {
	UWorld * world = GetWorld();
	std::unordered_map<uint8_t, FColor> color_zone_map;

	if (IsValid(world)) {
		FVector start, end;
		EnabledPathFinderConfig::Node * current;
		for (uint16_t i = 0; i < EnabledPathFinderConfig::node_count; i++) {
			current = &m_base_graph->m_nodes[i];
			const uint8_t _zone_id = current->zone_id;
			current->ToVector(start);
			if (color_zone_map.find(_zone_id) == color_zone_map.end()) {
				color_zone_map.emplace(_zone_id, FColor::MakeRandomColor());
			}
			FColor zone_color = color_zone_map.at(_zone_id);
			DrawDebugPoint(world, start, 5.f, zone_color, false, _time);

			const uint32_t _neighbor_id = i * EnabledPathFinderConfig::Graph::connectors;
			for (uint16_t j = 0; j < EnabledPathFinderConfig::Graph::connectors; j++) {
				EnabledPathFinderConfig::Node * next = m_base_graph->m_connectors[_neighbor_id + j];
				if (next) {
					next->ToVector(end);
					DrawDebugLine(world, start, end, zone_color, false, _time, 0, .5f);
				}
			}
			
		}
	}
}

void ATopologyTracer::debugLogConfig() {
	UE_LOG(LogTemp, Log, TEXT("TopologyTracer Config:"));
	UE_LOG_VECTOR3(EnabledPathFinderConfig::Center_WU);
	UE_LOG_VECTOR3(EnabledPathFinderConfig::Bounds_WU);
	UE_LOG_VECTOR3(EnabledPathFinderConfig::WU_per_NU);
	UE_LOG_VECTOR3(EnabledPathFinderConfig::GraphDims_NU);
	UE_LOG_VECTOR3(EnabledPathFinderConfig::GraphDims_ZU);
	UE_LOG_VECTOR3(EnabledPathFinderConfig::ZoneDims_WU);
	UE_LOG_VECTOR3(EnabledPathFinderConfig::ZoneDims_NU);
	UE_LOG_VECTOR3(EnabledPathFinderConfig::TopLeftPoint_WU);
	UE_LOG_VECTOR3(EnabledPathFinderConfig::BottomLeftPoint_WU);
	UE_LOG(LogTemp, Log, TEXT("zone count: %d"), EnabledPathFinderConfig::zone_count);
	UE_LOG(LogTemp, Log, TEXT("zone node count %d"), EnabledPathFinderConfig::zone_node_count);
	UE_LOG(LogTemp, Log, TEXT("node count %d"), EnabledPathFinderConfig::node_count);
}

void ATopologyTracer::requestPathImp(
	EnabledPathFinderConfig::Node const * const _start_node,
	EnabledPathFinderConfig::Node const * const _end_node) const {
	std::vector<EnabledPathFinderConfig::Node const *> path;
	path.reserve(m_path_size_reservation);
#if DRAW_FRONTIER
	const bool _path_found = AStar<EnabledPathFinderConfig::Graph>::Search(m_base_graph, _start_node, _end_node, path, GetWorld());
#else
	const bool _path_found = AStar<EnabledPathFinderConfig::Graph>::Search(m_base_graph, _start_node, _end_node, path);
#endif

#if LOG_PATH_DIAGNOSTICS
	FVector start_vec, end_vec;
	_start_node->ToVector(start_vec);
	_end_node->ToVector(end_vec);

	UE_LOG(LogTemp, Log, TEXT("Path requested for start node %d, "
		"zone %d at %s to end node %d, zone %d at %s"),
		_start_node->id, _start_node->zone_id, *start_vec.ToString(), _end_node->id, _end_node->zone_id, *end_vec.ToString());
	if (_path_found) {
		UE_LOG(LogTemp, Log, TEXT("Completed path, start: %s, end: %s, length: %d"),
			*start_vec.ToString(), *end_vec.ToString(), path.size());
	}
#endif

#if DRAW_PATH
	if (_path_found) debugDrawPath(path, 5.f);
#endif
}

void ATopologyTracer::debugDrawPath(const std::vector<EnabledPathFinderConfig::Node const *>& _path, const float _time) const {
	UWorld * world = GetWorld();
	FVector start, end;
	for (uint16_t i = 0; i < _path.size() - 1; ++i) {
		_path[i]->ToVector(start);
		_path[i + 1]->ToVector(end);
#if LOG_PATH_VECTOR
		UE_LOG(LogTemp, Log, TEXT("node %d on path, id: %d, location: %s"), i, _path[i]->id, *start.ToString());
#endif
		DrawDebugLine(world, start, end, FColor::Green, true, _time, 0, 5.f);
	}
}

const FVector ATopologyTracer::m_wu_per_nu_f = {
	static_cast<float>(EnabledPathFinderConfig::WU_per_NU::x),
	static_cast<float>(EnabledPathFinderConfig::WU_per_NU::y),
	static_cast<float>(EnabledPathFinderConfig::WU_per_NU::z) };

const FVector ATopologyTracer::m_ZoneDims_WU_f = {
	static_cast<float>(EnabledPathFinderConfig::ZoneDims_WU::x),
	static_cast<float>(EnabledPathFinderConfig::ZoneDims_WU::y),
	static_cast<float>(EnabledPathFinderConfig::ZoneDims_WU::z) };