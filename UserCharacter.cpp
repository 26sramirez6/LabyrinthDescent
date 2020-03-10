// Fill out your copyright notice in the Description page of Project Settings.

#include "UserCharacter.h"

AUserCharacter::AUserCharacter() {
	PrimaryActorTick.bCanEverTick = true;
	SetActorEnableCollision(true);
	bBlockInput = true;
	m_base_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = m_base_mesh;

	static ConstructorHelpers::FObjectFinder<UStaticMesh>
		base_mesh_asset(TEXT("StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'"));

	if (base_mesh_asset.Object) {
		m_base_mesh->SetStaticMesh(base_mesh_asset.Object);
		m_base_mesh->SetCollisionResponseToChannel(CollisionChannels::Topology, ECollisionResponse::ECR_Ignore);
	}

	m_current_path.reserve(EnabledPathFinderConfig::zone_node_count);
}

void AUserCharacter::BeginPlay() {
	Super::BeginPlay();
	
}

void AUserCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	switch (m_current_state) {
	case StateTypes::StartOnPath:
		tickStartOnPath(DeltaTime);
		break;
	case StateTypes::ContinueOnPath:
		tickContinueOnPath(DeltaTime);
		break;
	case StateTypes::EndPath:
		tickEndPath(DeltaTime);
		break;
	case StateTypes::Idle:
		tickIdle(DeltaTime);
		break;
	}
}

void AUserCharacter::tickStartOnPath(const float _delta_time) {
	if (!m_current_path.size()) {
		m_current_state = StateTypes::EndPath;
		m_current_waypoint = 0;
		return;
	}
	m_current_waypoint = 1;
	m_current_waypoint_node = m_current_path[m_current_waypoint];
	m_current_waypoint_node->ToVector(m_current_waypoint_vec);
	m_current_state = StateTypes::ContinueOnPath;
}

void AUserCharacter::tickContinueOnPath(const float _delta_time) {
	const FVector _current = GetActorLocation();
	const FVector _dir = (m_current_waypoint_vec - _current)*_delta_time*m_speed;
	SetActorLocation(_current + _dir);
}

void AUserCharacter::tickEndPath(const float _delta_time) {
	m_queue_new_path = false;
}

void AUserCharacter::tickIdle(const float _delta_time) {
	if (m_queue_new_path) {
		m_current_state = StateTypes::StartOnPath;
	}
}