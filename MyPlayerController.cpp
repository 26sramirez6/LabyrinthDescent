// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerController.h"

AMyPlayerController::AMyPlayerController() {
	PrimaryActorTick.bCanEverTick = true;
	bAutoManageActiveCameraTarget = false;
	PlayerCameraManagerClass = ALDPlayerCameraManager::StaticClass();

	UWorld * world = GetWorld();
	if (IsValid(world)) { // must surround in IsValid or crash on load
		UE_LOG(LogTemp, Log, TEXT("Spawning members in PC"));//
	}
}

void AMyPlayerController::BeginPlay() {
	Super::BeginPlay();
	m_live_game_handler = Cast<ALiveGameHandler>(this->GetPawn());
	if (IsValid(m_live_game_handler)) {
		if (IsValid(m_live_game_handler->m_camera)) {
			SetViewTargetWithBlend(m_live_game_handler);
			return;
		}
	}
	UE_LOG(LogTemp, Log, TEXT("Couldn't Set View Target!!!"));
}

void AMyPlayerController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AMyPlayerController::SetupInputComponent() {
	APlayerController::SetupInputComponent();
	if (!InputComponent) {
		UE_LOG(LogTemp, Log, TEXT("Input component not initialized!"));
	}
	checkf(InputComponent, TEXT("Input component on PlayerController not initialized"));

	InputComponent->BindAction("OnRightClick", IE_Pressed, this, &AMyPlayerController::OnRightClick);
	InputComponent->BindAction("OnCtrlPressed", IE_Pressed, this, &AMyPlayerController::OnCtrlPressed);
	InputComponent->BindAction("OnCtrlReleased", IE_Released, this, &AMyPlayerController::OnCtrlReleased);

	InputComponent->BindAxis("OnMouseX", this, &AMyPlayerController::OnMouseX);
	InputComponent->BindAxis("OnMouseY", this, &AMyPlayerController::OnMouseY);
	InputComponent->BindAxis("OnMouseScroll", this, &AMyPlayerController::OnMouseScroll);
}

void AMyPlayerController::OnCtrlPressed() { m_ctrl_pressed = true; }
void AMyPlayerController::OnCtrlReleased() { m_ctrl_pressed = false; }

void AMyPlayerController::OnRightClick() {
	if (m_in_live_game && IsValid(m_live_game_handler)) {
		m_live_game_handler->SetLiveGameTargetOnClick();
	}
}

void AMyPlayerController::OnMouseX(float axis) {
	if (m_in_live_game && IsValid(m_live_game_handler)) {
		if (m_ctrl_pressed) {
			m_live_game_handler->AdjustCameraYaw(axis);
		} else {
			m_live_game_handler->AdjustCameraY(axis);
		}
	}
}

void AMyPlayerController::OnMouseY(float axis) {
	if (m_in_live_game && IsValid(m_live_game_handler)) {
		if (m_ctrl_pressed) {
			m_live_game_handler->AdjustCameraPitch(axis);
		} else {
			m_live_game_handler->AdjustCameraX(axis);
		}
	}
}

void AMyPlayerController::OnMouseScroll(float axis) {
	if (m_in_live_game && IsValid(m_live_game_handler)) {
		m_live_game_handler->AdjustSpringArmLength(axis);
	}
}