// Fill out your copyright notice in the Description page of Project Settings.

#include "LiveGameHandler.h"

// Sets default values
ALiveGameHandler::ALiveGameHandler() : 
	m_mouse_rotation_delta(ForceInitToZero),
	m_mouse_location_delta(ForceInitToZero) {
	bBlockInput = true;
	bHidden = true;
	bIgnoresOriginShifting = true;
	/*bLockLocation = true; causes launch failure?*/
	PrimaryActorTick.bCanEverTick = true;
	SetActorLocation(FVector::ZeroVector);
	SetActorEnableCollision(false);

	m_spring_arm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	m_camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	m_base_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = m_base_mesh;

	FAttachmentTransformRules attachment_rules{ EAttachmentRule::KeepRelative, false };
	m_spring_arm->AttachToComponent(RootComponent, attachment_rules);
	m_spring_arm->TargetArmLength = m_arm_length;
	m_spring_arm->SetWorldRotation(m_spring_arm_start_rotator);
	m_camera->AttachToComponent(m_spring_arm, attachment_rules, USpringArmComponent::SocketName);

	UWorld * world = GetWorld();
	if (IsValid(world)) { // must surround in IsValid or crash on load
		UE_LOG(LogTemp, Log, TEXT("Spawning core actors in LiveGameHandler"));
		m_target = world->SpawnActor<AUserCharacterTarget>();
		m_user_character = world->SpawnActor<AUserCharacter>(FVector::ZeroVector, FRotator::ZeroRotator);
		m_user_character->setTargetObject(m_target);
		m_topo = world->SpawnActor<ATopologyTracer>();
		m_mob_handler = world->SpawnActor<AMobHandler>();
		m_mob_handler->initializeMobHandler(m_topo);
		m_topo->trace();
#if DRAW_GRAPH
		m_topo->debugDrawGraph(100.f);
#endif

#if SPAWN_MOBS
		m_mob_handler->SpawnMobsAllZones<MOB_COUNT_PER_ZONE_X, MOB_COUNT_PER_ZONE_Y>();
#endif
	}
}

void ALiveGameHandler::BeginPlay() {
	Super::BeginPlay();
	// TODO: Why won't these work in the constructor?, overridden by PIE
	SetActorLocation(FVector::ZeroVector);
	m_base_mesh->SetRelativeLocation(m_base_mesh_start_location);
}

void ALiveGameHandler::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	FRotator new_yaw = GetActorRotation();
	new_yaw.Yaw += m_mouse_rotation_delta.X;
	SetActorRotation(new_yaw);

	FRotator new_pitch = m_spring_arm->GetComponentRotation();
	new_pitch.Pitch = FMath::Clamp(new_pitch.Pitch + m_mouse_rotation_delta.Y, -80.f, 0.f);
	m_spring_arm->SetWorldRotation(new_pitch);
	//m_spring_arm->AddRelativeLocation(m_mouse_location_delta);
	m_spring_arm->TargetArmLength = FMath::Clamp(m_spring_arm->TargetArmLength - 
		m_arm_length_delta, 0.f, 2000.f);
}

void ALiveGameHandler::setLiveGameTargetOnClick() {
	APlayerController * pc = static_cast<APlayerController*>(this->GetController());
	float x, y;
	pc->GetMousePosition(x, y);
	FVector2D mouse_position{ x, y };
	FHitResult hit_result;
	const bool trace_complex = false;
	if (pc->GetHitResultAtScreenPosition(
		mouse_position, CollisionChannels::Topology, trace_complex, hit_result)) {
		m_target->recieveNewTarget(hit_result.Location);

		m_user_character->clearPathBuffer();

		const bool _path_found = m_topo->requestPath(
			m_user_character->GetActorLocation(), 
			hit_result.Location, m_user_character->getPathBuffer());

		if (_path_found) m_user_character->alertPathFound();
	}
}

const FVector ALiveGameHandler::m_delta_camera_inc_x =			{ 10.f, 0.f, 0.f };
const FVector ALiveGameHandler::m_delta_camera_dec_x =			{ -10.f, 0.f, 0.f };
const FVector ALiveGameHandler::m_delta_camera_inc_y =			{ 0.f, 10.f, 0.f };
const FVector ALiveGameHandler::m_delta_camera_dec_y =			{ 0.f, -10.f, 0.f };
const FVector ALiveGameHandler::m_base_mesh_start_location =	{ 0.f, 0.f, 90.f };
const FRotator ALiveGameHandler::m_spring_arm_start_rotator =	{ -60.f, 0.f, 0.f };
const float ALiveGameHandler::m_arm_length = 650.f;
