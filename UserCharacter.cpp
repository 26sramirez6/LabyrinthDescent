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
	const FVector _current_loc = GetActorLocation();
	const FVector _forward = GetActorForwardVector();
	const FRotator _current_rot = GetActorRotation();
	const unsigned _path_size = m_current_path.size();
	//m_current_waypoint = m_current_waypoint >= _path_size ? _path_size - 1 : m_current_waypoint;
	while (true) {
		if (LIKELY(m_current_waypoint < _path_size - 1)) {
			const float _dist_to_next_waypoint =
				FVector::Dist2D(m_current_waypoint_vec, _current_loc);
			UE_LOG(LogTemp, Log, TEXT("current waypoint: %d"), m_current_waypoint);
			UE_LOG(LogTemp, Log, TEXT("dist to next waypoint: %f"), _dist_to_next_waypoint);
			
			if (_dist_to_next_waypoint < m_pick_next_waypoint_distance) {
				m_current_waypoint_node = m_current_path[++m_current_waypoint];
				m_current_waypoint_node->ToVector(m_current_waypoint_vec);
			} else {
				break;
			}
		} else {
			break;
		}
	}

	FVector dir = (m_current_waypoint_vec - _current_loc);
	const float _target_dist_to_next_waypoint = dir.Size();
	//dir.Normalize();
	
	if (UNLIKELY(m_current_waypoint == _path_size &&
		_target_dist_to_next_waypoint < m_end_reached_distance)) {
		m_target_reached = true;
		m_current_state = StateTypes::EndPath;
	} else {
		const float _slow_down = LIKELY(_target_dist_to_next_waypoint > m_slow_down_distance) ? 
			1.f : _target_dist_to_next_waypoint / m_slow_down_distance;
		const float _factor = _delta_time * m_run_speed * _slow_down;
		const FVector _delta_loc = _forward * _factor;
		AddActorWorldOffset(_delta_loc);
	}
	
	const FRotator _target_rot = UKismetMathLibrary::FindLookAtRotation(_current_loc, m_current_waypoint_vec);
	const FRotator _delta_rot = FQuat::Slerp(_current_rot.Quaternion(),
		_target_rot.Quaternion(), m_turn_speed*_delta_time).Rotator();

	DrawDebugLine(GetWorld(), _current_loc, m_current_waypoint_vec, FColor::Cyan, false, 1.f, 0, .5f);
	SetActorRotation(_delta_rot);

}

void AUserCharacter::tickEndPath(const float _delta_time) {
	m_queue_new_path = false;
	// finish out rotations
	m_current_state = StateTypes::Idle;
}

void AUserCharacter::tickIdle(const float _delta_time) {
	if (m_queue_new_path) {
		m_current_state = StateTypes::StartOnPath;
	}
}