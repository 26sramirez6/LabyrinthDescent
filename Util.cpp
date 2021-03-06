#include "Util.h"
#include <iostream>
#include <cstdint>
#include <limits>

struct Node {
//bool a, b, c, d;
enum sizes {
as = 1,
bs = 1,
vs = 32
};

uint16_t a: 1;//sizes::as;
uint16_t b: 1;//sizes::bs;
uint32_t v: 32;//sizes::vs;
uint16_t id;
int16_t x,y,z;
uint16_t zone;
};



template<class Center, 
VALIDATE(IsVector3, Center)=0>
struct Test {
	static constexpr unsigned size = Center::size;
};


template<class S1, class S2,
	VALIDATE(IsSameSize, S1, S2)=0>
	struct Test2 {
		static constexpr unsigned size = S1::size;
};


int main() {
	//std::cout << sizeof (Node) << std::endl;
	//Node n;
	uint16_t count = 1 << 4;
	std::cout << count << std::endl;
	//std::cout << sizes::as << std::endl;
	//std::cout << sizes::bs << std::endl;

//	std::cout << n.g << n.a << n.b << n.c << n.d << std::endl;
	//std::cout << std::numeric_limits<uint8_t>::max() << std::endl;
	//std::cout << sizeof(Node::v) << std::endl;
	constexpr bool t1 = IntSequence<1,2,3,7,4>::max==7;
	static_assert(t1, "t1 fail");

	static_assert(Vector3<4,5,1>::size==3, "t2 fail");

	static_assert(Vector3<5,5,10>::max==10, "t3 fail");

	static_assert(Vector3<4,5,6>::y==5, "t4 fail");

	static_assert(Indexer<4, IntSequence<1,1,1,1,3>>::value==3, "t5 fail");
	
	using slice = typename Slice<1, 3, IntSequence<6,5,6,2,2>>::type;
	static_assert(slice::size==2, "");
	static_assert(Indexer<0, slice>::value==5, "");
	static_assert(Indexer<1, slice>::value==6, "");

	using v = Vector3<4,5,6>;
	static_assert(v::max*v::max==36,"");

	using z = Test<Vector3<1,8,2>>;
	static_assert(z::size==3, "");
	
	static_assert(IsIntSequence<v>::value, "");
	
	

	using x = ScalarMul<v, 4>::type;
	static_assert(Indexer<2, x>::value==24, "");
	static_assert(x::sum==60, "");

	//static_assert(AreSameSize<x,v>::value, "");
	static_assert(Test2<IntSequence<1, 2>, IntSequence<2, 2>>::size == 2, "");

	using q = VectorMul<IntSequence<4,5,6>, IntSequence<16, 20, 24>>::type;
	static_assert(Indexer<0, q>::value == 64, "");
	static_assert(Indexer<1, q>::value == 100, "");
	static_assert(Indexer<2, q>::value == 144, "");

	static_assert(IsSameSize<Vector2<3, 2>, IntSequence<1, 1>>::value, "");
	using b = VectorMul<x,v>::type;
	static_assert(Indexer<0, b>::value == 64, "");
	static_assert(Indexer<1, b>::value == 100, "");
	static_assert(Indexer<2, b>::value == 144, "");
	static_assert(b::x == 64, "");

	return 0;
}
