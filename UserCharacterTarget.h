// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "CollisionChannels.h"
#include "UserCharacterTarget.generated.h"

// forward declared for friend
class ALiveGameHandler;

UCLASS()
class LABYRINTHDESCENT_API AUserCharacterTarget : public AActor
{
	GENERATED_BODY()

public:
	AUserCharacterTarget();

	void recieveNewTarget(const FVector& _target) { 
		this->SetActorLocation(_target, false);
		m_target.Set(_target.X, _target.Y, _target.Z);
		UE_LOG(LogTemp, Log, TEXT("Target value: %s"), *m_target.ToString());
	};

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	friend class ALiveGameHandler;
	friend class AMyPlayerController;

	UPROPERTY(VisibleAnywhere)
	FVector m_target;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent * m_base_mesh;
};
