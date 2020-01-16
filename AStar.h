// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <queue>
#include <functional>
#include "CoreMinimal.h"
#include "Graph.h"

/**
 * 
 */

template<typename GraphT, typename NodeT, typename PriorityT>
class LABYRINTHDESCENT_API AStar
{
public:
	AStar();
	~AStar();

	using Qitem = std::pair<PriorityT, NodeT const * const>;

	void Search(const GraphT& _graph, 
		NodeT const * const _start,
		NodeT const * const _end,
		std::vector<NodeT * const>& path_, 
		std::vector<PriorityT>& costs_) {

		std::priority_queue<Qitem, std::deque<Qitem>, std::greater<Qitem>> frontier;
		frontier.emplace(0, start);
		path_.emplace_back(_start);
		costs_.push_back(0);
		NodeT* current;
		while (!frontier.empty()) {
			current = frontier.top().second;
			const uint16 current_id = current->id;
			frontier.pop();

			if (current == _end) {
				break;
			}

			NodeT* next = _graph.GetConnectors();
			for (unsigned int i = 0; i < GraphT::connector_size; i++, next++) {
				PriorityT new_cost = cost_so_far + _graph.EdgeWeight(current, next);
				const uint16 next_id = next->id;
				if (new_cost < costs_[next_id]) {
					costs_[next_id] = new_cost;
					PriorityT priority = new_cost + _graph.Heuristic(current, next);
					frontier.emplace()
				}
			}

		}

	}
};
