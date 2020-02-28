// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <queue>
#include <functional>
#include <algorithm>
#include <type_traits>
#include <cassert>
#include "CoreMinimal.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Util.h"
#include "PathSettings.h"
#include "GridGraph.h"


template<class GraphT>
class LABYRINTHDESCENT_API AStar {
	//private:
	//	using _throw = VALIDATE(IsGraph, GraphT);
public:
	AStar() {};
	~AStar() {};

	using Node = typename GraphT::Node;
	using Priority = typename GraphT::Priority;
	using Qitem = std::pair<typename Priority, typename Node const *>;

private:
	template<typename Enabled>
	struct WaveHandle;

	template<>
	struct WaveHandle<std::true_type> {
		FORCEINLINE void
		AddToWave(Node const * node) { m_wave.push_back(node); }

		FORCEINLINE void
		DebugDrawWave(UWorld const * const _world) const {
			FVector point;
			for (auto node : m_wave) {
				node->ToVector(point);
				DrawDebugPoint(_world, point, 5.f, FColor::Blue, false, 5.f);
			}
		}
		std::vector<Node const *> m_wave;
	};

	template<>
	struct WaveHandle<std::false_type> {
		FORCEINLINE static void
		AddToWave(Node const *) { return; }

		FORCEINLINE static void
		DebugDrawWave(UWorld const * const) { return; }
	};

public:
	FORCEINLINE static bool
	Search(
		GraphT const * _graph,
		Node const * const _start,
		Node const * const _end,
		std::vector<Node const *>& path_) {
		return SearchImp<std::false_type>(_graph, _start, _end, path_, nullptr);
	}

	FORCEINLINE static bool
	Search(
		GraphT const * _graph,
		Node const * const _start,
		Node const * const _end,
		std::vector<Node const *>& path_,
		UWorld const * const _world) {
		return SearchImp<std::true_type>(_graph, _start, _end, path_, _world);
	}

private:

	template<typename WaveEnabled>
	static bool SearchImp(
		GraphT const * _graph,
		Node const * const _start,
		Node const * const _end,
		std::vector<Node const *>& path_,
		UWorld const * const _world = nullptr) {

		std::priority_queue<Qitem, std::deque<Qitem>,
			std::greater<Qitem>> frontier;

		WaveHandle<WaveEnabled> wave;
		frontier.emplace(0, _start);
		// move to a map? a lot of stack allocation for a call
		bool visited[GraphT::node_count] = { false };
		Priority costs[GraphT::node_count] = { 0 };
		uint16_t parents[GraphT::node_count] = { 0 };
		Node const * current;
		uint16_t const _end_id = _end->id;
		bool path_found = false;
		while (UNLIKELY(!frontier.empty())) {
			current = frontier.top().second;
			const uint16 _current_id = current->id;
			frontier.pop();

			if (UNLIKELY(_current_id == _end_id)) {
				path_found = true;
				break;
			}

			Node const * const * connectors = _graph->GetConnectors(current);
			
			wave.AddToWave(current);
			for (uint16_t i = 0; i < GraphT::connectors; i++) {
				Node const * const next = connectors[i];
				if (LIKELY(next)) {
					const uint16 next_id = next->id;
					//UE_LOG(LogTemp, Log, TEXT("processing neighbor i: %d, next id: %d"), i, next_id);
					Priority cost_to_next = costs[_current_id] + _graph->EdgeWeight(current, next);

					if ((!visited[next_id] || cost_to_next < costs[next_id]) &&
						next->is_reachable &&
						!next->is_edge) {
						costs[next_id] = cost_to_next;
						Priority estimated_cost_to_goal = cost_to_next + _graph->CalcHeuristic(next, _end);
						frontier.emplace(estimated_cost_to_goal, next);
						parents[next_id] = _current_id;
						visited[next_id] = true;
						wave.AddToWave(next);
					}
				}
			}
		}

		wave.DebugDrawWave(_world);
		if (path_found) {
			path_.emplace_back(_end);
			uint16_t current_id = parents[_end->id];
			uint16_t start_id = _start->id;
			uint16_t i = 0;
			while (LIKELY(current_id != start_id && ++i < GraphT::node_count)) {
				path_.emplace_back(&_graph->m_nodes[current_id]);
				current_id = parents[current_id];
			}
			path_.emplace_back(_start);
			std::reverse(path_.begin(), path_.end());
		}
#if LOG_PATH_DIAGNOSTICS
		else {
			UE_LOG(LogTemp, Log, TEXT("error, path not found"));
		}
#endif
		return path_found;
	}

private:
	constexpr static uint16_t m_max_frontier_size = 500;
};