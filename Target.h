// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Target.generated.h"

UCLASS()
class LABYRINTHDESCENT_API ATarget : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATarget(const FObjectInitializer& ObjectInitializer) :
		Super(ObjectInitializer), m_target(ForceInitToZero) {
		
		// this crashes editor
		//for (auto it = GetWorld()->GetPlayerControllerIterator(); it; ++it) {
		//	this->Controller = (AController *)it->Get();
		//}

		PrimaryActorTick.bCanEverTick = true;
		if (this->IsPlayerControlled()) {
			UE_LOG(LogTemp, Log, TEXT("Successfully controlled"));
		}

		PrimaryActorTick.bCanEverTick = true;
	};

private:
	friend class AMyPlayerController;
	FVector m_target;

	UFUNCTION()
	void RecieveNewTarget(const FVector& _target) { 
		this->SetActorLocation(_target, false);
		m_target.Set(_target.X, _target.Y, _target.Z);
		UE_LOG(LogTemp, Log, TEXT("Target value: %s"), *m_target.ToString());
	};//

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
