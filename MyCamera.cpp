// Fill out your copyright notice in the Description page of Project Settings.

#include "MyCamera.h"

// Sets default values
AMyCamera::AMyCamera() {
	PrimaryActorTick.bCanEverTick = true;

	m_spring_arm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	m_camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	m_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = m_mesh;

	FAttachmentTransformRules attatchment_rules{ EAttachmentRule::KeepRelative, false };

	m_spring_arm->AttachToComponent(RootComponent, attatchment_rules, TEXT("TestSocketName"));
	m_spring_arm->TargetArmLength = 350.f;
	m_spring_arm->SetWorldRotation(FRotator(-60.f, 0.f, 0.f));
	m_camera->AttachToComponent(m_spring_arm, attatchment_rules, USpringArmComponent::SocketName);
}

// Called when the game starts or when spawned
void AMyCamera::BeginPlay() {
	Super::BeginPlay();
	
}

// Called every frame
void AMyCamera::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	FRotator new_yaw = GetActorRotation();
	FRotator new_pitch = m_spring_arm->GetComponentRotation();
	new_yaw.Yaw += m_mouse_input.X;
	new_pitch.Pitch += m_mouse_input.Y;
}