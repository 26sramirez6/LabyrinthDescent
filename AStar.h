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

	using std::pair<PriorityT, NodeT*> = Qitem;

	void Search(const GraphT& _graph, 
		const NodeT& _start,
		const NodeT& _end, 
		std::vector<NodeT>& path_, 
		std::vector<PriorityT>& costs_) {

		std::priority_queue<Qitem, std::deque<Qitem>, std::greater<Qitem>> frontier;
		frontier.emplace(0, &start);
		path_.emplace_back(_start);
		costs_.push_back(0);
		NodeT* current;
		PriorityT cost_so_far = 0;
		while (!frontier.empty()) {
			current = frontier.top().second;
			frontier.pop();

			if (current == *_end) {
				break;
			}

			NodeT* next = _graph.GetConnectors();
			for (unsigned int i = 0; i < GraphT::connector_size; i++, next++) {
				PriorityT new_cost = cost_so_far + _graph.Cost(current, next);
				if (costs_) {

				}
			}

		}

	}
};
