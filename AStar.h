// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <queue>
#include <functional>
#include "CoreMinimal.h"
#include "Util.h"
#include "GridGraph.h"

/**
 * 
 */

template<typename GraphT,
VALIDATE(IsGraph, GraphT)=0>
class LABYRINTHDESCENT_API AStar
{
public:
	AStar();
	~AStar();

	using Qitem = std::pair<GraphT::Priority, GraphT::Node const * const>;

	static void Search(const GraphT& _graph, 
		GraphT::Node const * const _start,
		GraphT::Node const * const _end,
		std::vector<GraphT::Node * const>& path_) {

		std::priority_queue<Qitem, std::deque<Qitem>,
			std::greater<Qitem>> frontier;
		frontier.emplace(0, _start);
		path_.emplace_back(_start);
		
		// move to a map? a lot of stack allocation for a call
		bool visited[GraphT::node_count] = { false };
		GraphT::PriorityT costs[GraphT::node_count] = { 0 };
		GraphT::Node* current;
		while (!frontier.empty()) {
			current = frontier.top().second;
			const uint16 current_id = current->id;
			frontier.pop();

			if (current == _end) {
				break;
			}

			GraphT::Node** connectors = _graph.GetConnectors(current);
			for (unsigned int i = 0; i < GraphT::connectors; i++, connectors++) {
				GraphT::Node * next = *connectors;
				const uint16 next_id = next->id;
				GraphT::PriorityT cost_to_next = costs_[next_id] + _graph.EdgeWeight(current, next);

				if (!visited[next_id] || cost_to_next < costs_[next_id]) {
					costs_[next_id] = cost_to_next;
					GraphT::PriorityT estimated_cost_to_goal = cost_to_next + _graph.Heuristic(current, next);
					frontier.emplace(estimated_cost_to_goal, next);
					visited[next_id] = true;
				}
			}
		}
	}
};
