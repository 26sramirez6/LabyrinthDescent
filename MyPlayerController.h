// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LiveGameHandler.h"
#include "MyCamera.h"
#include "LDPlayerCameraManager.h"
#include "Engine/World.h"
#include "UObject/NameTypes.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LABYRINTHDESCENT_API AMyPlayerController : public APlayerController 
{
	GENERATED_BODY()
	
private:
	AMyPlayerController();
	void SetupInputComponent() override;
	void OnRightClick();
	void OnCtrlPressed();
	void OnCtrlReleased();
	void OnMouseX(float axis);
	void OnMouseY(float axis);
	void OnMouseScroll(float axis);
	void BeginPlay() override;
	void Tick(float DeltaTime) override;

public:
	ALiveGameHandler * m_live_game_handler;
	ATarget * m_target;
	ALDPlayerCameraManager * m_camera_manager;
	bool m_in_live_game = true;
	bool m_ctrl_pressed = false;
};
