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
}

void AUserCharacter::BeginPlay() {
	Super::BeginPlay();
	
}

void AUserCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

