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

template<typename NodeT, typename PriorityT, 
	typename HeuristicT, typename GraphDims_NU, 
	typename ZoneDims_NU, typename GraphDims_ZU,
	uint16_t Connectors,
VALIDATE(IsGraphNode, NodeT)=0,
VALIDATE(std::is_arithmetic, PriorityT)=0,
VALIDATE(IsHeuristic, HeuristicT)=0,
VALIDATE(IsVector3, GraphDims_NU)=0,
VALIDATE(IsVector3, ZoneDims_NU)=0,
VALIDATE(IsVector3, GraphDims_ZU)=0>
class LABYRINTHDESCENT_API GridGraph
{
public:
	static constexpr uint8_t zone_count = 0x1 << NodeT::BitFieldSizes::ZONE_ID;
	static constexpr uint16_t node_count_x = GraphDims_NU::x;
	static constexpr uint16_t node_count_y = GraphDims_NU::y;
	static constexpr uint16_t node_count = node_count_x * node_count_y;
	static constexpr uint16_t zone_node_count_x = ZoneDims_NU::x;
	static constexpr uint16_t zone_node_count_y = ZoneDims_NU::y;
	static constexpr uint16_t zone_node_count = zone_node_count_x * zone_node_count_y;
	static constexpr uint16_t zone_edge_v_corrector = zone_node_count - (zone_node_count_x - 1);
	static constexpr uint16_t zone_edge_h_corrector = (zone_node_count * (GraphDims_ZU::x - 1)) + zone_node_count_x;

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
		return 1;//m_weights[_current->id*Connectors + _neighbor->id % Connectors];
	}

private:
	void InitializeData() {
		for (uint8_t z = 0; z < zone_count; z++) {		
			const uint16_t _first_idx_in_zone = z * zone_node_count;
			for (uint16_t i = 0; i < zone_node_count_y; i++) {
				for (uint16_t j = 0; j < zone_node_count_x; j++) {
					const uint16_t _node_id = _first_idx_in_zone + i*zone_node_count_x + j;
					const uint32_t _current = _node_id * Connectors;
					Node & node = m_nodes[_node_id];
					node.id = _node_id;
					node.zone_id = z;
					
					const bool _is_global_bot_edge = (z % zone_count_x==0) && j==0;
					const bool _is_global_top_edge = (z+1) % zone_count_x==0 && j==zone_node_count_x - 1;
					const bool _is_global_left_edge = z < zone_node_count_x && i==0;
					const bool _is_global_right_edge = z > zone_count - zone_node_count_x - 1 && i==zone_node_count_y - 1;

					// bot left
					m_connectors[_current] = UNLIKELY(_is_global_bot_edge || _is_global_left_edge) ? // graph bot or left edge
						nullptr : UNLIKELY(j==0 && i==0) ? // zone bl corner
							&m_nodes[_node_id - zone_edge_h_corrector - zone_edge_v_corrector] : UNLIKELY(i==0) ? // zone left edge
								&m_nodes[_node_id - 1 - zone_edge_h_corrector] : UNLIKELY(j==0) ? // zone bot edge
									&m_nodes[_node_id - zone_edge_v_corrector - zone_node_count_x] : &m_nodes[_node_id - zone_node_count_x - 1];

					// bot mid
					m_connectors[_current + 1] = UNLIKELY(_is_global_bot_edge) ?
						nullptr : UNLIKELY(j==0) ?
							&m_nodes[_node_id - zone_edge_v_corrector]  : &m_nodes[_node_id - 1];

					// bot right
					m_connectors[_current + 2] = UNLIKELY(_is_global_bot_edge || _is_global_right_edge) ? // graph bot or right edge
						nullptr : UNLIKELY(j==0 && i==zone_node_count_y -1) ? // zone bot right corner
							&m_nodes[_node_id + zone_edge_h_corrector - zone_edge_v_corrector] : UNLIKELY(i==zone_node_count_y -1) ?// zone right edge
								&m_nodes[_node_id + zone_edge_h_corrector - 1] : UNLIKELY(j==0) ? // zone bot edge
									&m_nodes[_node_id - zone_edge_v_corrector + zone_node_count_x] : &m_nodes[_node_id + zone_node_count_x - 1] ;

					// mid left
					m_connectors[_current + 3] = UNLIKELY(_is_global_left_edge) ?
						nullptr : UNLIKELY(i==0) ?
							&m_nodes[_node_id - zone_edge_h_corrector] : &m_nodes[_node_id - zone_node_count_x];

					// mid right
					m_connectors[_current + 4] = UNLIKELY(_is_global_right_edge) ?
						nullptr : UNLIKELY(i==zone_node_count_y - 1) ?
							&m_nodes[_node_id + zone_edge_h_corrector] : &m_nodes[_node_id + zone_node_count_x];

					// top left
					m_connectors[_current + 5] = UNLIKELY(_is_global_top_edge || _is_global_left_edge) ?
						nullptr : UNLIKELY(j==zone_node_count_x - 1 && i==0) ? // zone top left corner
							&m_nodes[_node_id + zone_edge_v_corrector - zone_edge_h_corrector] : UNLIKELY(i==0) ? // zone left edge
								&m_nodes[_node_id - zone_edge_h_corrector + 1] : UNLIKELY(j==zone_node_count_x - 1) ? // zone top edge
									&m_nodes[_node_id + zone_edge_v_corrector - zone_node_count_x] : &m_nodes[_node_id - zone_node_count_x + 1];
					
					// top mid
					m_connectors[_current + 6] = UNLIKELY(_is_global_top_edge) ?
						nullptr : UNLIKELY(j==zone_node_count_x - 1) ?
							&m_nodes[_node_id + zone_edge_v_corrector] : &m_nodes[_node_id + 1];

					// top right
					m_connectors[_current + 7] = UNLIKELY(_is_global_top_edge || _is_global_right_edge) ?
						nullptr : UNLIKELY(j==zone_node_count_x - 1 && i==zone_node_count_y - 1) ? // zone top right corner
							&m_nodes[_node_id + zone_edge_v_corrector + zone_edge_h_corrector] : UNLIKELY(i==zone_node_count_y - 1) ? // zone right edge
								&m_nodes[_node_id + zone_edge_h_corrector + 1] : UNLIKELY(j==zone_node_count_x - 1) ? // zone top edge
									&m_nodes[_node_id + zone_edge_v_corrector + zone_node_count_x] : &m_nodes[_node_id + zone_node_count_x + 1];

					node.is_edge = UNLIKELY(
						_is_global_bot_edge  ||
						_is_global_top_edge  ||
						_is_global_left_edge ||
						_is_global_right_edge);

					node.is_corner = UNLIKELY(
						(_is_global_bot_edge && _is_global_left_edge)  ||
						(_is_global_bot_edge && _is_global_right_edge) ||
						(_is_global_top_edge && _is_global_left_edge)  ||
						(_is_global_top_edge && _is_global_right_edge));
				}
			}
		}
	}

private:
	friend class ATopologyTracer;
	friend class AStar< GridGraph<NodeT, PriorityT, HeuristicT, 
		GraphDims_NU, ZoneDims_NU, GraphDims_ZU, Connectors> >;
	NodeT m_nodes[node_count];
	NodeT * m_connectors[node_count*Connectors];
	PriorityT m_weights[node_count*Connectors];
	HeuristicT m_heuristic;
};

template<typename T> struct IsGraph : std::integral_constant<bool, false> {};
//template<template<typename...> class T, typename ...U> struct IsGraph<T<U...>> : std::integral_constant<bool, false> {};
template<typename ...T> struct IsGraph<GridGraph<T...>> : std::integral_constant<bool, true> {};