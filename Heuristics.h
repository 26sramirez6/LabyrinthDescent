#pragma once

#include "Node.h"

template<typename NodeT>
struct Manhattan {
	inline int calc(NodeT* current, NodeT* next) const {
		return std::abs(current->x - next->x) +
			std::abs(current->y - next->y) +
			std::abs(current->z - next->z);
	}
};

template<typename T> struct IsHeuristic : std::integral_constant<bool, false> {};
template<typename NodeT> struct IsHeuristic<Manhattan<NodeT>> : std::integral_constant<bool, true> {};