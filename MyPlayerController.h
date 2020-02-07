// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LiveGameHandler.h"
#include "MyCamera.h"
#include "LDPlayerCameraManager.h"
//#include "Engine/Engine.h
#include "EngineMinimal.h"
#include "Engine/World.h"
#include "Engine/GameViewportClient.h"
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

private:
	ALiveGameHandler * m_live_game_handler;
	ATarget * m_target;
	ALDPlayerCameraManager * m_camera_manager;
	
private:
	FIntPoint m_vp_size;
	FVector2D m_edge_scroll_y;
	FVector2D m_edge_scroll_x;
	float m_edge_factor = 80.f;
	FViewport * m_viewport = nullptr;
	bool m_in_live_game = true;
	bool m_ctrl_pressed = false;
};
