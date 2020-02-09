// Fill out your copyright notice in the Description page of Project Settings.

#include "TopologyTracer.h"


ATopologyTracer::ATopologyTracer {
	UE_LOG(LogTemp, Log, TEXT("Constructing Tracer"));
	bBlockInput = true;
	bHidden = true;
	bIgnoresOriginShifting = true;
	bLockLocation = true;
	SetActorEnableCollision(false);
	m_base_graph = new Tracer::Graph();
}

ATopologyTracer::BeginPlay() override {
	Super::BeginPlay();
};

ATopologyTracer::~ATopologyTracer() {
	if (m_base_graph) {
		delete m_base_graph;
	}
}

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
			if (world->LineTraceSingleByChannel(
				out_hit,
				_start,
				_end,
				CollisionChannels::Topology,
				_query,
				_response)) {
				// float->int16. pros/cons of keeping in int16 vs float
				current->SetLocation(_x, _y, out_hit.Location.Z);
				current->is_reachable = true;
				_color = FColor::Red;
			} else {
				current->SetLocation(_x, _y, 20);
				current->is_reachable = false;
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

void ATopologyTracer::DebugDrawGraph(float time) {
	UWorld * world = GetWorld();
	if (IsValid(world)) {
		FVector start, end;
		Tracer::Node * current;
		for (uint16_t i = 0; i < Tracer::node_count; i++) {
			Tracer::Node * next = m_base_graph->m_connectors[i];
			current = &m_base_graph->m_nodes[i];
			current->ToVector(start);
			DrawDebugPoint(world, start, 5.f, current->is_reachable ? FColor::Red : FColor::Blue, false, time);
			//UE_LOG(LogTemp, Log, TEXT("DEBUGDRAWGRAPH: Drawing node %d, %p at %s"), current->id, current, *start.ToString());

			const uint32_t _neighbor_id = i * Tracer::Graph::connectors;
			for (uint16_t j = 0; j < Tracer::Graph::connectors; j++) {
				Tracer::Node * next = m_base_graph->m_connectors[_neighbor_id + j];
				if (next) {
					next->ToVector(end);
					DrawDebugLine(world, start, end, current->is_reachable ? FColor::Blue : FColor::Blue, false, time, 0, .5f);
				}
			}
		}
	}
}

void ATopologyTracer::RequestPath(const FVector& _start, 
	const FVector& _end) const {
	std::vector<Tracer::Node * const> path(50);
	Tracer::Node const * const start = GetNearestNode(_start);
	Tracer::Node const * const end = GetNearestNode(_end);
	_start
		AStar<Tracer::Graph>::Search(*m_base_graph, start, end, path);
}

FORCEINLINE Tracer::Node const * const
ATopologyTracer::GetNearestNode(const FVector& _target) const {
	FVector translated_target = _target / m_node_scaling;
	translated_target
}

const FVector ATopologyTracer::m_node_scaling = {
	static_cast<float>(Tracer::Scale::x),
	static_cast<float>(Tracer::Scale::y),
	static_cast<float>(Tracer::Scale::z) };