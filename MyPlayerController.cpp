// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerController.h"

AMyPlayerController::AMyPlayerController() {
	m_live_game_handler = GetWorld()->SpawnActor<ALiveGameHandler>();
	this->Possess(m_live_game_handler);
	PrimaryActorTick.bCanEverTick = true;
}

void AMyPlayerController::SetupInputComponent() {
	APlayerController::SetupInputComponent();
	if (!InputComponent) {
		UE_LOG(LogTemp, Log, TEXT("Input component not initialized!"));
	}

	checkf(InputComponent, TEXT("Input component on PlayerController not initialized"));
	InputComponent->BindAction("OnRightClick",
		IE_Pressed, this, &AMyPlayerController::OnRightClick);
}

void AMyPlayerController::OnRightClick() {
	if (m_in_live_game) {
		float x, y;
		player_controller->GetMousePosition(x, y);
		FVector2D mouse_position{ x, y };
		FHitResult hit_result;
		const bool trace_complex = false;
		if (player_controller->GetHitResultAtScreenPosition(
			mouse_position, ECC_Visibility, trace_complex, hit_result)) {
			if (IsValid(m_target)) {
				m_target->RecieveNewTarget(hit_result.Location);
			}
		}
		m_live_game_handler->SetLiveGameTargetOnClick();
	}
}