// Fill out your copyright notice in the Description page of Project Settings.

#include "TopologyTracer.h"

ATopologyTracer::ATopologyTracer() {
	UE_LOG(LogTemp, Log, TEXT("Constructing Tracer"));
	ATopologyTracer::DebugLogConfig();
	bBlockInput = true;
	bHidden = true;
	bIgnoresOriginShifting = true;
	bLockLocation = true;
	SetActorEnableCollision(false);
	m_base_graph = new Tracer::Graph();
}

ATopologyTracer::~ATopologyTracer() {
	if (m_base_graph) {
		delete m_base_graph;
	}
}

void ATopologyTracer::BeginPlay() {
	Super::BeginPlay();
};

void ATopologyTracer::Trace() {
	UE_LOG(LogTemp, Log, TEXT("Performing Trace"));
	UWorld * world = this->GetWorld();
	FHitResult out_hit;
	const FCollisionQueryParams _query;
	const FCollisionResponseParams _response;
	FColor _color;
	Tracer::Node * current;
	for (uint8_t z = 0; z < Tracer::zone_count; z++) {
		const uint8_t _dx_zu = z % Tracer::zone_count_x; // zones up
		const uint8_t _dy_zu = z / Tracer::zone_count_x; // zones to the right, int division intended
		const int16_t _zone_blp_wu_x = Tracer::BottomLeftPoint_WU::x + _dx_zu * Tracer::WU_per_ZU::x;
		const int16_t _zone_blp_wu_y = Tracer::BottomLeftPoint_WU::y + _dy_zu * Tracer::WU_per_ZU::y;
		const uint16_t _first_idx_in_zone = z * Tracer::zone_node_count;
		uint16_t inner_zone_idx = 0;
		for (uint16_t i_wu = 0; i_wu < Tracer::zone_node_count_y*Tracer::WU_per_NU::y; i_wu += Tracer::WU_per_NU::y) {
			for (uint16_t j_wu = 0; j_wu < Tracer::zone_node_count_x*Tracer::WU_per_NU::x; j_wu += Tracer::WU_per_NU::x) {
				const uint16_t _node_id = _first_idx_in_zone + inner_zone_idx++;
				const float _x_wu = _zone_blp_wu_x + j_wu;
				const float _y_wu = _zone_blp_wu_y + i_wu;
				const FVector _start(_x_wu, _y_wu, Tracer::top_z_WU);
				const FVector _end(_x_wu, _y_wu, Tracer::bot_z_WU);
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
					current->is_reachable = out_hit.Location.Z < Tracer::max_reachable_elevation_WU;
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

void ATopologyTracer::DebugDrawGraph(const float _time) const {
	UWorld * world = GetWorld();
	std::unordered_map<uint8_t, FColor> color_zone_map;

	if (IsValid(world)) {
		FVector start, end;
		Tracer::Node * current;
		for (uint16_t i = 0; i < Tracer::node_count; i++) {
			current = &m_base_graph->m_nodes[i];
			const uint8_t _zone_id = current->zone_id;
			current->ToVector(start);
			if (color_zone_map.find(_zone_id) == color_zone_map.end()) {
				color_zone_map.emplace(_zone_id, FColor::MakeRandomColor());
			}
			FColor zone_color = color_zone_map.at(_zone_id);
			DrawDebugPoint(world, start, 5.f, zone_color, false, _time);

			//UE_LOG(LogTemp, Log, TEXT("DEBUGDRAWGRAPH: Drawing node %d, %p at %s, reachable?: %d"), current->id, current, *start.ToString(), current->is_reachable);
			const uint32_t _neighbor_id = i * Tracer::Graph::connectors;
			for (uint16_t j = 0; j < Tracer::Graph::connectors; j++) {
				Tracer::Node * next = m_base_graph->m_connectors[_neighbor_id + j];
				if (next) {
					next->ToVector(end);
					DrawDebugLine(world, start, end, zone_color, false, _time, 0, .5f);
				}
			}
			
		}
	}
}

void ATopologyTracer::DebugLogConfig() {
	UE_LOG(LogTemp, Log, TEXT("TopologyTracer Config:"));
	UE_LOG_VECTOR3(Tracer::Center_WU);
	UE_LOG_VECTOR3(Tracer::Bounds_WU);
	UE_LOG_VECTOR3(Tracer::WU_per_NU);
	UE_LOG_VECTOR3(Tracer::GraphDims_NU);
	UE_LOG_VECTOR3(Tracer::GraphDims_ZU);
	UE_LOG_VECTOR3(Tracer::WU_per_ZU);
	UE_LOG_VECTOR3(Tracer::ZoneDims_NU);
	UE_LOG_VECTOR3(Tracer::TopLeftPoint_WU);
	UE_LOG_VECTOR3(Tracer::BottomLeftPoint_WU);
	UE_LOG(LogTemp, Log, TEXT("zone count: %d"), Tracer::zone_count);
	UE_LOG(LogTemp, Log, TEXT("zone node count %d"), Tracer::zone_node_count);
	UE_LOG(LogTemp, Log, TEXT("node count %d"), Tracer::node_count);
}

void ATopologyTracer::RequestPath(const FVector& _start, const FVector& _end) const {
	std::vector<Tracer::Node const *> path;
	path.reserve(m_path_size_reservation);
	Tracer::Node const * const start = GetNearestNode(_start);
	Tracer::Node const * const end = GetNearestNode(_end);
	
#if DRAW_FRONTIER
	const bool _path_found = AStar<Tracer::Graph>::Search(m_base_graph, start, end, path, GetWorld());
#else
	const bool _path_found = AStar<Tracer::Graph>::Search(m_base_graph, start, end, path);
#endif

#if LOG_PATH_DIAGNOSTICS
	FVector start_loc, end_loc;
	start->ToVector(start_loc);
	end->ToVector(end_loc);

	UE_LOG(LogTemp, Log, TEXT("Path requested for start node %d, "
		"zone %d at %s to end node %d, zone %d at %s"), 
		start->id, start->zone_id, *start_loc.ToString(), end->id, end->zone_id, *end_loc.ToString());
	if (_path_found) {
		UE_LOG(LogTemp, Log, TEXT("Completed path, start: %s, end: %s, length: %d"),
			*_start.ToString(), *_end.ToString(), path.size());
	}
#endif

#if DRAW_PATH
	if (_path_found) DebugDrawPath(path, 5.f);
#endif
}

void ATopologyTracer::DebugDrawPath(const std::vector<Tracer::Node const *>& _path, const float _time) const {
	UWorld * world = GetWorld();
	FVector start, end;
	for (uint16_t i = 0; i < _path.size() - 1; ++i) {
		_path[i]->ToVector(start);
		_path[i+1]->ToVector(end);
#if LOG_PATH_VECTOR
		UE_LOG(LogTemp, Log, TEXT("node %d on path, id: %d, location: %s"), i, _path[i]->id, *start.ToString());
#endif
		DrawDebugLine(world, start, end, FColor::Green, true, _time, 0, 5.f);
	}
}

const FVector ATopologyTracer::m_wu_per_nu_f = {
	static_cast<float>(Tracer::WU_per_NU::x),
	static_cast<float>(Tracer::WU_per_NU::y),
	static_cast<float>(Tracer::WU_per_NU::z) };

const FVector ATopologyTracer::m_wu_per_zu_f = {
	static_cast<float>(Tracer::WU_per_ZU::x),
	static_cast<float>(Tracer::WU_per_ZU::y),
	static_cast<float>(Tracer::WU_per_ZU::z) };