// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Util.h"
#include "TopologyTracer.h"
#include "GameFramework/GameStateBase.h"
#include "MyGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class LABYRINTHDESCENT_API AMyGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	ATopologyTracer * m_tracer;
	
};
