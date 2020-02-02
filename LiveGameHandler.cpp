// Fill out your copyright notice in the Description page of Project Settings.

#include "LiveGameHandler.h"

// Sets default values
ALiveGameHandler::ALiveGameHandler() {
	PrimaryActorTick.bCanEverTick = true;
	//m_target = GetWorld()->SpawnActor<ATarget>();
}

 //Called when the game starts or when spawned
void ALiveGameHandler::BeginPlay() {
	Super::BeginPlay();
}

 //Called every frame
void ALiveGameHandler::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
//void ALiveGameHandler::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//}

//void ALiveGameHandler::SetLiveGameTargetOnClick() {
//	float x, y;
//	APlayerController* player_controller = static_cast<APlayerController*>(this->GetController());
//	player_controller->GetMousePosition(x, y);
//	FVector2D mouse_position{ x, y };
//	FHitResult hit_result;
//	const bool trace_complex = false;
//	if (player_controller->GetHitResultAtScreenPosition(
//		mouse_position, ECC_Visibility, trace_complex, hit_result)) {
//		if (IsValid(m_target)) {
//			m_target->RecieveNewTarget(hit_result.Location);
//		}
//	}
//}