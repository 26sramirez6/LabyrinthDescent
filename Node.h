#pragma once
#include <cstdint>
#include "CoreMinimal.h"


struct GridNode {	
	GridNode() {}
	GridNode(int16_t x_, int16_t y_, int16_t z_) : x(x_), y(y_), z(z_) {}
	FORCEINLINE void SetLocation(int16_t x_, int16_t y_, int16_t z_) { x = x_; y = y_; z = z_; }
	FORCEINLINE void ToVector(FVector& v) const { v.Set(x, y, z); }

	enum BitFieldSizes {
		IS_CORNER = 1,
		IS_EDGE = 1,
		IS_REACHABLE = 1,
		IS_OFF_TOPOLOGY = 1,
		ZONE_ID = 4
	};

	// bit fields
	uint16_t is_corner : IS_CORNER;
	uint16_t is_edge : IS_EDGE;
	uint16_t is_reachable : IS_REACHABLE;
	uint16_t is_off_topology : IS_OFF_TOPOLOGY;
	uint8_t zone_id : ZONE_ID;

	uint16_t id;
	int16_t x;
	int16_t y;
	int16_t z;
	
};

template<typename T> struct IsGraphNode : std::integral_constant<bool, false> {};
template<> struct IsGraphNode<GridNode> : std::integral_constant<bool, true> {};