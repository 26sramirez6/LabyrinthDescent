#include "Util.h"

int main() {
	constexpr bool t1 = IntSequence<1,2,3,7,4>::max==7;
	static_assert(t1, "t1 fail");

	static_assert(Vector3<4,5,1>::size==3, "t2 fail");

	static_assert(Vector3<5,5,10>::max==10, "t3 fail");

	static_assert(Vector3<4,5,6>::y==5, "t4 fail");

	static_assert(IntAtIndex<4, IntSequence<1,1,1,1,3>>::value==3, "t5 fail");
	
	using slice = typename Slice<1, 3, IntSequence<6,5,6,2,2>>::type;
	static_assert(slice::size==2, "");
	static_assert(IntAtIndex<0, slice>::value==5, "");
	static_assert(IntAtIndex<1, slice>::value==6, "");

	using v = Vector3<4,5,6>;
	static_assert(v::max*v::max==36,"");
	return 0;

}
