// Fill out your copyright notice in the Description page of Project Settings.

#include "Mob.h"

AMob::AMob() : m_new_target_tick_frequency(FMath::RandRange(50, 1000)) {
	PrimaryActorTick.bCanEverTick = true;
	m_base_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = m_base_mesh;
	static ConstructorHelpers::FObjectFinder<UStaticMesh>
		base_mesh_asset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));

	if (base_mesh_asset.Object) {
		m_base_mesh->SetStaticMesh(base_mesh_asset.Object);
		m_base_mesh->SetCollisionResponseToChannel(CollisionChannels::Topology,
			ECollisionResponse::ECR_Ignore);
	} 
	
	m_current_path.reserve(EnabledPathFinderConfig::zone_node_count);
}

void AMob::BeginPlay() {
	Super::BeginPlay();
	
}

void AMob::initializeMob(ATopologyTracer const * _topology_tracer, 
	const uint8_t _zone_id) {
	m_spawn_zone_id = _zone_id;
	m_topology_tracer = _topology_tracer;
	m_is_initialized = true;
}

void AMob::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if (m_is_initialized && m_new_target_tick_frequency < ++m_ticks) {
		generateNewTargetNode();
		m_ticks = 0;
	}
	
}

void AMob::generateNewTargetNode() {
	const FVector& _start = GetActorLocation();
	EnabledPathFinderConfig::Node const * const _end_node = 
		m_topology_tracer->getRandomNodeInZone(m_spawn_zone_id);
	m_topology_tracer->requestPath(_start, _end_node, m_current_path);
}

