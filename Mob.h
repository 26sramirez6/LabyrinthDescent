// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CollisionChannels.h"
#include "GameFramework/Actor.h"
#include "Mob.generated.h"

class ALiveGameHandler;
class AMyPlayerController;

UCLASS()
class LABYRINTHDESCENT_API AMob : public AActor
{
	GENERATED_BODY()
	
public:	
	AMob();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	friend class ALiveGameHandler;
	friend class AMyPlayerController;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent * m_base_mesh;

};
