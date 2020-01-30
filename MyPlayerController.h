// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Target.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LABYRINTHDESCENT_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	void SetupInputComponent() {
		Super::SetupInputComponent();
		if (!InputComponent) {
			UE_LOG(LogTemp, Log, TEXT("Input component not initialized!"));
		}

		checkf(InputComponent, TEXT("Input component not initialized"));
		InputComponent->BindAction("SetTarget",
			IE_Pressed, this, &AMyPlayerController::SetTargetOnClick);
	}

	void SetTargetOnClick() {
		float x, y;
		this->GetMousePosition(x, y);
		FVector2D mouse_position(x, y);
		FHitResult hit_result;
		const bool trace_complex = false;
		if (this->GetHitResultAtScreenPosition(
			mouse_position, ECC_Visibility, trace_complex, hit_result)) {
			ATarget * pawn = Cast<ATarget>(this->GetPawn());
			if (pawn != nullptr && IsValid(pawn)) {
				pawn->RecieveNewTarget(hit_result.Location);
			}
		}
	}

	void CameraScroll() {

	}
};
