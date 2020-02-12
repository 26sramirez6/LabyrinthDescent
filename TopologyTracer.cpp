// Fill out your copyright notice in the Description page of Project Settings.

#include "TopologyTracer.h"
#include "Kismet/KismetMathLibrary.h"

ATopologyTracer::ATopologyTracer() {
	UE_LOG(LogTemp, Log, TEXT("Constructing Tracer"));
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
	uint16_t id = 0;
	Tracer::Node * current;
	for (uint16_t i = 0; i < Tracer::node_count_y*Tracer::Scale::y; i += Tracer::Scale::y) {
		for (uint16_t j = 0; j < Tracer::node_count_x*Tracer::Scale::x; j += Tracer::Scale::x, id++) {
			const float _x = Tracer::BottomLeftPoint::x + j;
			const float _y = Tracer::BottomLeftPoint::y + i;
			const FVector _start(_x, _y, Tracer::top_z);
			const FVector _end(_x, _y, Tracer::bot_z);
			current = &m_base_graph->m_nodes[id];
			current->is_reachable = true;
			if (world->LineTraceSingleByChannel(
				out_hit,
				_start,
				_end,
				CollisionChannels::Topology,
				_query,
				_response)) {
				// float->int16. pros/cons of keeping in int16 vs float
				current->SetLocation(_x, _y, out_hit.Location.Z);
				current->is_off_topology = false;
				//_color = FColor::Red;
			} else {
				current->SetLocation(_x, _y, 50);
				current->is_off_topology = true;
				_color = FColor::Blue;
			};
			FVector point;
			current->ToVector(point);
			//UE_LOG(LogTemp, Log, TEXT("TRACE: Drawing node %d, %p at %s"), current->id, current, *point.ToString());
			//DrawDebugPoint(world, point, 5.f, _color, false, 50.);
			//UE_LOG(LogTemp, Log, TEXT("Drew debug point"));
		}
	}
}

void ATopologyTracer::DebugDrawGraph(const float _time) const {
	UWorld * world = GetWorld();
	if (IsValid(world)) {
		FVector start, end;
		Tracer::Node * current;
		for (uint16_t i = 0; i < Tracer::node_count; i++) {
			Tracer::Node * next = m_base_graph->m_connectors[i];
			current = &m_base_graph->m_nodes[i];
			current->ToVector(start);
			DrawDebugPoint(world, start, 5.f, current->is_reachable ? FColor::Red : FColor::Blue, false, _time);
			//UE_LOG(LogTemp, Log, TEXT("DEBUGDRAWGRAPH: Drawing node %d, %p at %s, reachable?: %d"), current->id, current, *start.ToString(), current->is_reachable);

			const uint32_t _neighbor_id = i * Tracer::Graph::connectors;
			for (uint16_t j = 0; j < Tracer::Graph::connectors; j++) {
				Tracer::Node * next = m_base_graph->m_connectors[_neighbor_id + j];
				if (next) {
					next->ToVector(end);
					DrawDebugLine(world, start, end, current->is_reachable ? FColor::Red : FColor::Blue, false, _time, 0, .5f);
				}
			}
		}
	}
}

void ATopologyTracer::RequestPath(const FVector& _start, const FVector& _end) const {
	std::vector<Tracer::Node const *> path;
	path.reserve(m_path_size_reservation);
	Tracer::Node const * const start = GetNearestNode(_start);
	UE_LOG(LogTemp, Log, TEXT("received id: %d pointer %p"), start->id, start);
	Tracer::Node const * const end = GetNearestNode(_end);
	UE_LOG(LogTemp, Log, TEXT("received id: %d pointer %p"), end->id, end);

	FVector start_vec, end_vec, zero_vec;
	
	start->ToVector(start_vec);
	end->ToVector(end_vec);

	DrawDebugPoint(GetWorld(), start_vec, 5.f, FColor::Red, true, 100.f, SDPG_MAX);
	DrawDebugPoint(GetWorld(), end_vec, 5.f, FColor::Red, true, 100.f, SDPG_MAX);
	for (int i = 0; i < 200; i++) {
		m_base_graph->m_nodes[i].ToVector(zero_vec);
		FColor color;
		if (i == 0) {
			color = FColor::Green;
		} else if (i < 100) {
			color = FColor::Blue;
		} else {
			color = FColor::Red;
		}
		//UE_LOG(LogTemp, Log, TEXT("node %d, location: %s"), i, *zero_vec.ToString());
		DrawDebugPoint(GetWorld(), zero_vec, 5.f, color, true, 100.f, SDPG_MAX);
	}
	UE_LOG(LogTemp, Log, TEXT("Start node id: %d, End node id: %d"), start->id, end->id);

	//AStar<Tracer::Graph>::Search(m_base_graph, start, end, path);
	//UE_LOG(LogTemp, Log, TEXT("Completed path, start: %s, end: %s, length: %d"), 
	//	*_start.ToString(), *_end.ToString(), path.size());

	//DebugDrawPath(path, 100.f);
}

void ATopologyTracer::DebugDrawPath(const std::vector<Tracer::Node const *>& _path, const float _time) const {
	UWorld * world = GetWorld();
	FVector start, end;
	for (uint16_t i = 0; i < _path.size() - 1; ++i) {
		_path[i]->ToVector(start);
		_path[i+1]->ToVector(end);
		//UE_LOG(LogTemp, Log, TEXT("location: %s"), *start.ToString());
		UE_LOG(LogTemp, Log, TEXT("node %d on path, id: %d, location: %s"), i, _path[i]->id, *start.ToString());
		DrawDebugLine(world, start, end, FColor::Green, true, _time, 0, 5.f);
	}
}

const FVector ATopologyTracer::m_node_scaling = {
	static_cast<float>(Tracer::Scale::x),
	static_cast<float>(Tracer::Scale::y),
	static_cast<float>(Tracer::Scale::z) };