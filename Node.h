#pragma once
#include <cstdint>

struct GridNode {
	uint16_t id;
	int x, y, z;
	bool is_corner, is_edge, is_reachable;
	GridNode() {}
	GridNode(int x_, int y_, int z_) : x(x_), y(y_), z(z_) {}
	inline void SetLocation(int x_, int y_, int z_) { x = x_; y = y_; z = z_; }
};

template<typename T> struct IsGraphNode : std::integral_constant<bool, false> {};
template<> struct IsGraphNode<GridNode> : std::integral_constant<bool, true> {};