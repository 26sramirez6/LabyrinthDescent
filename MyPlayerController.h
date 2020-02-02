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
	
public:

	AMyPlayerController();
	void SetupInputComponent() override;
	void OnRightClick();
	void OnMouseYaw(float axis);
	void OnMousePitch(float axis);

public:
	ALiveGameHandler * m_live_game_handler;
	ATarget * m_target;
	ALDPlayerCameraManager * m_camera_manager;
	bool m_in_live_game = true;
};
