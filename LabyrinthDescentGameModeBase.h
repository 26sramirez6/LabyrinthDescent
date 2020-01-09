// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyPlayerController.h"
#include "LabyrinthDescentGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class LABYRINTHDESCENT_API ALabyrinthDescentGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ALabyrinthDescentGameModeBase(const FObjectInitializer& ObjectInitializer)
		: Super(ObjectInitializer) {
		this->PlayerControllerClass = AMyPlayerController::StaticClass();
		this->DefaultPawnClass = ATarget::StaticClass();

		UE_LOG(LogTemp, Log, TEXT("Initializing Labyrinth Descent Game Mode Base"));
	}
};
