// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <cmath>
#include "CoreMinimal.h"

/**
 * 
 */

struct GraphNode {
	uint16 id;
	int x, y, z;
};

template<typename NodeT>
struct Manhattan {
	inline int calc(NodeT* current, NodeT* next) const {
		return std::abs(current->x - next->x) +
			std::abs(current->y - next->y) +
			std::abs(current->z - next->z);
	}
};

template<typename NodeT, typename PriortyT, typename HeuristicT,
	unsigned int NodeCount = 1024, unsigned int Connectors = 8>
class LABYRINTHDESCENT_API Graph
{
public:
	static constexpr unsigned int connector_size = Connectors;
private:
	GraphNode m_nodes[NodeCount*Connectors];
	HeuristicT m_heuristic;

public:
	Graph(HeuristicT heuristic) : m_heuristic(heuristic) {};

	~Graph();

	inline NodeT* GetConnectors(const NodeT& current) {
		return m_nodes[_target.id*Connectors];
	}

	inline PriortyT Cost(const NodeT* current, const NodeT* next) const {
		return m_heuristic.calc(current, next);
	}
};
