// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraActor.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "MyCamera.generated.h"

UCLASS()
class LABYRINTHDESCENT_API AMyCamera : public ACameraActor
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMyCamera();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	USpringArmComponent* m_spring_arm;
	UCameraComponent* m_camera;
	UStaticMeshComponent* m_mesh;
	FVector2D m_mouse_input;
};
