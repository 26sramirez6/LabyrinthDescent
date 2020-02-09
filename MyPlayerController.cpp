// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerController.h"

AMyPlayerController::AMyPlayerController() {
	PrimaryActorTick.bCanEverTick = true;
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	bHidden = true;
	bIgnoresOriginShifting = true;
	bLockLocation = true;
	
	PlayerCameraManagerClass = ALDPlayerCameraManager::StaticClass();
	if (GEngine && GEngine->GameViewport) {
		m_viewport = GEngine->GameViewport->Viewport;
		m_vp_size = GEngine->GameViewport->Viewport->GetSizeXY();
		m_edge_scroll_y.Set(static_cast<float>(m_vp_size.Y) / m_edge_factor,
			static_cast<float>(m_vp_size.Y) - static_cast<float>(m_vp_size.Y) / m_edge_factor);
		m_edge_scroll_x.Set(static_cast<float>(m_vp_size.X) / m_edge_factor,
			static_cast<float>(m_vp_size.X) - static_cast<float>(m_vp_size.X) / m_edge_factor);
	}
	
	UE_LOG(LogTemp, Log, TEXT("Viewport Size: %s"), *m_vp_size.ToString());
	UE_LOG(LogTemp, Log, TEXT("Edge Scroll Y: %s"), *m_edge_scroll_x.ToString());
	UE_LOG(LogTemp, Log, TEXT("Edge Scroll X: %s"), *m_edge_scroll_y.ToString());
}

void AMyPlayerController::BeginPlay() {
	Super::BeginPlay();
	SetInitialLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);
	SpawnLocation = FVector::ZeroVector;
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
			m_live_game_handler->AdjustCameraYawByAxis(axis);
		} else {
			float x, y;
			if (LIKELY(GetMousePosition(x, y))) { 
				if (x < m_edge_scroll_x.X) {
					m_live_game_handler->DecrementCameraLocationY();
				} else if (x > m_edge_scroll_x.Y) {
					m_live_game_handler->IncrementCameraLocationY();
				}
			}
		}
	}
}

void AMyPlayerController::OnMouseY(float axis) {
	if (m_in_live_game && IsValid(m_live_game_handler)) {
		if (m_ctrl_pressed) {
			m_live_game_handler->AdjustCameraPitchByAxis(axis);
		} else {
			float x, y;
			if (LIKELY(GetMousePosition(x, y))) {
				if (y < m_edge_scroll_y.X) {
					m_live_game_handler->IncrementCameraLocationX();
				} else if (y > m_edge_scroll_y.Y) {
					m_live_game_handler->DecrementCameraLocationX();
				}
			}
		}
	}
}

void AMyPlayerController::OnMouseScroll(float axis) {
	if (m_in_live_game && IsValid(m_live_game_handler)) {
		m_live_game_handler->AdjustSpringArmLengthByAxis(axis);
	}
}