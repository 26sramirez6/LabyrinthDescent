// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Target.h"
#include "MyCamera.h"
#include "LiveGameHandler.generated.h"

class AMyPlayerController;

UCLASS()
class LABYRINTHDESCENT_API ALiveGameHandler : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ALiveGameHandler();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE void AdjustSpringArmLength(float axis) { m_arm_length_delta = m_zoom_factor*axis; }
	FORCEINLINE void AdjustCameraYaw(float axis) { m_mouse_rotation_delta.X = m_rotion_factor*axis; }
	FORCEINLINE void AdjustCameraPitch(float axis) { m_mouse_rotation_delta.Y = m_rotion_factor*axis; }
	FORCEINLINE void AdjustCameraX(float axis) { m_mouse_location_delta.X = m_location_factor *axis; }
	FORCEINLINE void AdjustCameraY(float axis) { m_mouse_location_delta.Y = m_location_factor *axis; }

private:
	friend class AMyPlayerController;
	void SetLiveGameTargetOnClick();

	ATarget* m_target;
	USpringArmComponent* m_spring_arm;
	UCameraComponent* m_camera;
	UStaticMeshComponent* m_mesh;
 	FVector2D m_mouse_rotation_delta;
	FVector m_mouse_location_delta;
	float m_arm_length_delta = 0.f;
	static constexpr uint32 m_zoom_factor = 40;
	static constexpr uint32 m_location_factor = 10;
	static constexpr uint32 m_rotion_factor = 1;
};
