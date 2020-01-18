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

template<typename NodeT, typename PriorityT, typename HeuristicT,
	unsigned int NodeCount = 1024, unsigned int Connectors = 8>
class LABYRINTHDESCENT_API Graph
{
public:
	static constexpr unsigned int connectors = Connectors;
	static constexpr unsigned int node_count = NodeCount;
private:
	GraphNode m_nodes[NodeCount*Connectors];
	PriorityT m_weights[NodeCount*Connectors];
	HeuristicT m_heuristic;

public:
	Graph(HeuristicT heuristic) : m_heuristic(heuristic) {};

	~Graph();

	inline NodeT* GetConnectors(const NodeT& _current) {
		return m_nodes[_current.id*Connectors];
	}

	inline PriorityT Heuristic(const NodeT* _current, const NodeT* _next) const {
		return m_heuristic.calc(_current, _next);
	}

	inline PriorityT EdgeWeight(const NodeT* _current, const NodeT* _neighbor) const {
		return m_weights[_current->id + _neighbor->id % Connectors];
	}
};
