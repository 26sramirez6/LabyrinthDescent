// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <queue>
#include <functional>
#include <algorithm>
#include "CoreMinimal.h"
#include "Util.h"
#include "GridGraph.h"



template<class GraphT>
class LABYRINTHDESCENT_API AStar
{
//private:
//	using _throw = VALIDATE(IsGraph, GraphT);
public:
	AStar() {};
	~AStar() {};

	using Node = typename GraphT::Node;
	using Priority = typename GraphT::Priority;
	using Qitem = std::pair<typename Priority, typename Node const *>;

	static void Search(GraphT const * _graph, 
		Node const * const _start,
		Node const * const _end,
		std::vector<Node const *>& path_) {

		std::priority_queue<Qitem, std::deque<Qitem>,
			std::greater<Qitem>> frontier;
		frontier.emplace(0, _start);
		
		// move to a map? a lot of stack allocation for a call
		bool visited[GraphT::node_count] = { false };
		Priority costs[GraphT::node_count] = { 0 };
		uint16_t parents[GraphT::node_count] = { 0 };
		Node const * current;
		while (!frontier.empty()) {
			current = frontier.top().second;
			const uint16 current_id = current->id;
			frontier.pop();

			if (current == _end) {
				break;
			}

			Node const * const * connectors = _graph->GetConnectors(current);
			for (uint16_t i = 0; i < GraphT::connectors; i++, connectors++) {
				Node const * const next = *connectors;
				const uint16 next_id = next->id;
				Priority cost_to_next = costs[next_id] + _graph->EdgeWeight(current, next);

				if (!visited[next_id] || cost_to_next < costs[next_id]) {
					costs[next_id] = cost_to_next;
					Priority estimated_cost_to_goal = cost_to_next + _graph->CalcHeuristic(current, next);
					frontier.emplace(estimated_cost_to_goal, next);
					parents[next_id] = current_id;
					visited[next_id] = true;
				}
			}
		}
		
		path_.emplace_back(_end);
		uint16_t current_id = parents[_end->id];
		uint16_t start_id = _start->id;
		while (LIKELY(current_id != start_id)) {
			path_.emplace_back(&_graph->m_nodes[current_id]);
			current_id = parents[current_id];
		}
		path_.emplace_back(_start);
		std::reverse(path_.begin(), path_.end());
	}
};
