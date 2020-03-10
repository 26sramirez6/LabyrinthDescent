// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <unordered_map>
#include "GameFramework/Actor.h"
#include "CoreMinimal.h"

/**
 * 
 */

enum class StateTypes {
	StartOnPath,
	ContinueOnPath, 
	EndPath,
	Idle
};

class AbstractBaseState;

class LABYRINTHDESCENT_API StateMachine {
public:
	StateMachine(AActor * owner) : m_owner(owner) {};
	~StateMachine() {};

	FORCEINLINE void
	onTick(const float _deltaTime) {
		if (m_current_state) {
			//m_current_state->onTick(_deltaTime);
		}
	}

private:
	AActor * m_owner;
	AbstractBaseState * m_current_state;
	std::unordered_map<StateTypes, AbstractBaseState> m_state_map;
};


class AbstractBaseState {
public:
	virtual ~AbstractBaseState() = default;
};

template<typename DerivedState>
class LABYRINTHDESCENT_API BaseState : AbstractBaseState {
public:
	BaseState(StateMachine * state_machine) : m_state_machine(state_machine) {};
	virtual ~BaseState() {};

	FORCEINLINE void onTick(const float _deltaTime) { static_cast<DerivedState * const>(this)->onTickImp(_deltaTime); };
	FORCEINLINE void onEnter() { static_cast<DerivedState * const>(this)->onEnterImp(_deltaTime); };
	FORCEINLINE void onExit() { static_cast<DerivedState * const>(this)->onExitImp(_deltaTime); };
private:
	StateMachine * m_state_machine;
};

class LABYRINTHDESCENT_API ContinueOnPath : BaseState<ContinueOnPath> {
	void onTickImp(const float _deltaTime) {}
	void onEnterImp(const float _deltaTime) {}
	void onExitImp(const float _deltaTime) {}
};