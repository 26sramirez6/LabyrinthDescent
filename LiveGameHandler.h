// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Target.h"
#include "UserCharacter.h"
#include "AStar.h"
#include "TopologyTracer.h"
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

	FORCEINLINE void AdjustSpringArmLengthByAxis(float axis) { m_arm_length_delta = m_zoom_factor*axis; }
	FORCEINLINE void AdjustCameraYawByAxis(float axis) { m_mouse_rotation_delta.X = m_rotion_factor*axis; }
	FORCEINLINE void AdjustCameraPitchByAxis(float axis) { m_mouse_rotation_delta.Y = m_rotion_factor*axis; }
	FORCEINLINE void IncrementCameraLocationX() { m_spring_arm->AddRelativeLocation(m_delta_camera_inc_x); }
	FORCEINLINE void IncrementCameraLocationY() { m_spring_arm->AddRelativeLocation(m_delta_camera_inc_y); }
	FORCEINLINE void DecrementCameraLocationX() { m_spring_arm->AddRelativeLocation(m_delta_camera_dec_x); }
	FORCEINLINE void DecrementCameraLocationY() { m_spring_arm->AddRelativeLocation(m_delta_camera_dec_y); }

private:
	friend class AMyPlayerController;
	void SetLiveGameTargetOnClick();

	UPROPERTY(VisibleAnywhere)
	ATarget* m_target;

	UPROPERTY(VisibleAnywhere)
	AUserCharacter* m_user_character;

	UPROPERTY(VisibleAnywhere)
	ATopologyTracer* m_topo;
	
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* m_spring_arm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* m_camera;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* m_base_mesh;

 	FVector2D m_mouse_rotation_delta;
	FVector m_mouse_location_delta;

	float m_arm_length_delta = 0.f;
	
	static const float m_arm_length;

	static const FVector m_delta_camera_inc_x;
	static const FVector m_delta_camera_dec_x;
	static const FVector m_delta_camera_inc_y;
	static const FVector m_delta_camera_dec_y;
	static const FVector m_base_mesh_start_location;
	static const FRotator m_spring_arm_start_rotator;

	static constexpr uint16 m_zoom_factor = 40;
	static constexpr uint16 m_location_factor = 5;
	static constexpr uint16 m_rotion_factor = 1;
};
