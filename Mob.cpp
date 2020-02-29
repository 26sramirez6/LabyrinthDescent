// Fill out your copyright notice in the Description page of Project Settings.

#include "Mob.h"

AMob::AMob() {
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
}

void AMob::BeginPlay() {
	Super::BeginPlay();
	
}

void AMob::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

