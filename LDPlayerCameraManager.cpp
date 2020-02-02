// Fill out your copyright notice in the Description page of Project Settings.

#include "LDPlayerCameraManager.h"

ALDPlayerCameraManager::ALDPlayerCameraManager() {
	UWorld * world = GetWorld();
	if (IsValid(world)) { // must surround in IsValid or crash on load
		m_camera = world->SpawnActor<AMyCamera>();
	}
	this->ViewTarget.SetNewTarget(m_camera);
}