#pragma once
#include <cstdint>

struct GridNode {
	uint16_t id;
	int16_t x, y, z;
	bool is_corner, is_edge, is_reachable, is_off_topology;
	GridNode() {}
	GridNode(int16_t x_, int16_t y_, int16_t z_) : x(x_), y(y_), z(z_) {}
	FORCEINLINE void SetLocation(int16_t x_, int16_t y_, int16_t z_) { x = x_; y = y_; z = z_; }
	FORCEINLINE void ToVector(FVector& v) const { v.Set(x, y, z); }
	
};

template<typename T> struct IsGraphNode : std::integral_constant<bool, false> {};
template<> struct IsGraphNode<GridNode> : std::integral_constant<bool, true> {};