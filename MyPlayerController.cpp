// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerController.h"

AMyPlayerController::AMyPlayerController() {
	UWorld * world = GetWorld();
	if (IsValid(world)) { // must surround in IsValid or crash on load
		m_live_game_handler = world->SpawnActor<ALiveGameHandler>();
		m_target = world->SpawnActor<ATarget>();
		m_camera_manager = world->SpawnActor<ALDPlayerCameraManager>();
	}

	PrimaryActorTick.bCanEverTick = true;
	bAutoManageActiveCameraTarget = false;
	PlayerCameraManagerClass = ALDPlayerCameraManager::StaticClass();
}

void AMyPlayerController::SetupInputComponent() {
	APlayerController::SetupInputComponent();
	if (!InputComponent) {
		UE_LOG(LogTemp, Log, TEXT("Input component not initialized!"));
	}
	checkf(InputComponent, TEXT("Input component on PlayerController not initialized"));

	InputComponent->BindAction("OnRightClick",
		IE_Pressed, this, &AMyPlayerController::OnRightClick);
	InputComponent->BindAxis("OnMouseYaw", this, &AMyPlayerController::OnMouseYaw);
	InputComponent->BindAxis("OnMousePitch", this, &AMyPlayerController::OnMousePitch);
}

void AMyPlayerController::OnRightClick() {
	if (m_in_live_game) {
		float x, y;
		this->GetMousePosition(x, y);
		FVector2D mouse_position{ x, y };
		FHitResult hit_result;
		const bool trace_complex = false;
		if (this->GetHitResultAtScreenPosition(
			mouse_position, ECC_Visibility, trace_complex, hit_result)) {
			if (IsValid(m_target)) {
				m_target->RecieveNewTarget(hit_result.Location);
			}
		}
	}
}

void AMyPlayerController::OnMouseYaw(float axis) {
	if (m_in_live_game && IsValid(m_camera_manager)) {
		m_camera_manager->m_camera->m_mouse_input.X = axis;
	}
}

void AMyPlayerController::OnMousePitch(float axis) {
	if (m_in_live_game && IsValid(m_camera_manager)) {
		m_camera_manager->m_camera->m_mouse_input.Y = axis;
	}
}