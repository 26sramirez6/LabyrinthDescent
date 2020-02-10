// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <cmath>
#include <cstdint>
#include "Node.h"
#include "Util.h"
#include "Heuristics.h"
#include "CoreMinimal.h"


class ATopologyTracer;

template<class GraphT>
class AStar;

template<typename NodeT, typename PriorityT, typename HeuristicT,
	typename DimensionsT, uint16_t Connectors,
VALIDATE(IsGraphNode, NodeT)=0,
VALIDATE(std::is_arithmetic, PriorityT)=0,
VALIDATE(IsHeuristic, HeuristicT)=0,
VALIDATE(IsVector3, DimensionsT)=0>
class LABYRINTHDESCENT_API GridGraph
{
public:
	static constexpr uint16_t node_count_x = DimensionsT::x;
	static constexpr uint16_t node_count_y = DimensionsT::y;
	static constexpr uint16_t node_count = node_count_x * node_count_y;
	static constexpr uint16_t connectors = Connectors;
	static constexpr PriorityT max_weight = node_count + 1;
	using Node = NodeT;
	using Priority = PriorityT;
	using Heuristic = HeuristicT;

public:
	GridGraph() { InitializeData(); };

	~GridGraph() {};

	FORCEINLINE NodeT const * const * GetConnectors(NodeT const * _current) const {
		return &m_connectors[_current->id*Connectors];
	}

	FORCEINLINE PriorityT CalcHeuristic(NodeT const * _current, NodeT const * _next) const {
		return m_heuristic.calc(_current, _next);
	}

	FORCEINLINE PriorityT EdgeWeight(NodeT const * _current, NodeT const * _neighbor) const {
		return m_weights[_current->id + _neighbor->id % Connectors];
	}

private:
	void InitializeData() {
		for (unsigned i = 0; i < node_count_y; i++) {
			for (unsigned j = 0; j < node_count_x; j++) {
				const uint16_t node_id = i*node_count_x + j;
				m_nodes[node_id].id = node_id;
				const uint32_t current = node_id*Connectors;
				// bot
				m_connectors[current] = (i == 0 || j == 0) ?
					nullptr : &m_nodes[node_id - node_count_x - 1];
				m_connectors[current + 1] = (i == 0) ?
					nullptr : &m_nodes[node_id - node_count_x];
				m_connectors[current + 2] = (i == 0 || j == node_count_x - 1) ?
					nullptr : &m_nodes[node_id - node_count_x + 1];

				// mid
				m_connectors[current + 3] = (j == 0) ?
					nullptr : &m_nodes[node_id - 1];
				m_connectors[current + 4] = (j == node_count_x - 1) ?
					nullptr : &m_nodes[node_id + 1];

				// top
				m_connectors[current + 5] = (i == node_count_y - 1 || j == 0) ?
					nullptr : &m_nodes[node_id + node_count_x - 1];
				m_connectors[current + 6] = (i == node_count_y - 1) ?
					nullptr : &m_nodes[node_id + node_count_x];
				m_connectors[current + 7] = (i == node_count_y - 1 || j == node_count_x - 1) ?
					nullptr : &m_nodes[node_id + node_count_x + 1];
			}
		}
	}

private:
	friend class ATopologyTracer;
	friend class AStar< GridGraph<NodeT, PriorityT, HeuristicT, DimensionsT, Connectors> >;
	NodeT m_nodes[node_count];
	NodeT * m_connectors[node_count*Connectors];
	PriorityT m_weights[node_count*Connectors];
	HeuristicT m_heuristic;
};

template<typename T> struct IsGraph : std::integral_constant<bool, false> {};
//template<template<typename...> class T, typename ...U> struct IsGraph<T<U...>> : std::integral_constant<bool, false> {};
template<typename ...T> struct IsGraph<GridGraph<T...>> : std::integral_constant<bool, true> {};