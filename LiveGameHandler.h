// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Target.h"
#include "LiveGameHandler.generated.h"

UCLASS()
class LABYRINTHDESCENT_API ALiveGameHandler : public AActor
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

	// Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	//friend class AMyPlayerController;
	//void SetLiveGameTargetOnClick();

	//ATarget* m_target;
};
