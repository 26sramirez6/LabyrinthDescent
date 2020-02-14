#pragma once
#include <cstdint>
#include "Node.h"

template<typename NodeT>
struct Manhattan {
	FORCEINLINE int16_t calc(NodeT const * current, NodeT const * next) const {
		return std::abs(current->x - next->x) +
			std::abs(current->y - next->y) +
			std::abs(current->z - next->z);
	}
};

template<typename NodeT>
struct Chebyshev {
	FORCEINLINE int16_t calc(NodeT const * current, NodeT const * next) const { 
		const uint16_t _dx = std::abs(current->x - next->x);
		const uint16_t _dy = std::abs(current->y - next->y);
		return (_dx + _dy) + -(_dx > _dy ? _dy : _dx);
	}
};


template<typename T> struct IsHeuristic : std::integral_constant<bool, false> {};
template<typename NodeT> struct IsHeuristic<Manhattan<NodeT>> : std::integral_constant<bool, true> {};
template<typename NodeT> struct IsHeuristic<Chebyshev<NodeT>> : std::integral_constant<bool, true> {};