// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"
#include <type_traits>
#include <cstdint>
#include <tuple>
#include <limits>
#include <random>
#include <utility> 

#define DOUBLE2INT(i, d) \
    {double t = ((d) + 6755399441055744.0); i = *((int *)(&t));}

//inline uint16_t RoundU16(float a) {
//	uint16_t retval;
//
//	__asm fld a
//	__asm fistp retval
//
//	return retval;
//}

constexpr uint64_t
SqrtHelper(uint64_t x, uint64_t lo, uint64_t hi) {
	return lo == hi ? lo : ((x / ((lo + hi + 1) / 2) < ((lo + hi + 1) / 2))
		? SqrtHelper(x, lo, ((lo + hi + 1) / 2) - 1) : SqrtHelper(x, ((lo + hi + 1) / 2), hi));
}

constexpr uint64_t
CompileTimeSqrtU64(uint64_t x) {
	return SqrtHelper(x, 0, x / 2 + 1);
}

constexpr double
SqrtNewtonRaphson(double x, double curr, double prev) {
	return curr == prev
		? curr
		: SqrtNewtonRaphson(x, 0.5 * (curr + x / curr), curr);
}

constexpr double
CompileTimeSqrtD(double x) {
	return x >= 0 && x < std::numeric_limits<double>::infinity()
		? SqrtNewtonRaphson(x, x, 0)
		: std::numeric_limits<double>::quiet_NaN();
}

template <unsigned int N, typename T>
struct tuple_n_t {
	typedef decltype(std::tuple_cat(std::tuple<T>(), typename tuple_n_t<N - 1, T>::type())) type;
};

template <typename T>
struct tuple_n_t<0, T> {
	typedef decltype(std::tuple<>()) type;
};

template<std::size_t I = 0, typename FuncT, typename... Tp>
inline typename std::enable_if<I == sizeof...(Tp), void>::type
tuple_for_each_obj(std::tuple<Tp...> &, FuncT) {}

template<std::size_t I = 0, typename FuncT, typename... Tp>
inline typename std::enable_if < I < sizeof...(Tp), void>::type
tuple_for_each_obj(std::tuple<Tp...>& t, FuncT f) {
	f(std::get<I>(t));
	tuple_for_each_obj<I + 1, FuncT, Tp...>(t, f);
}

template<uint8_t S, uint8_t E, template<uint8_t> class C, bool Enabled = false>
struct TupleForRange {
	static_assert(S < E, "Invalid template parameters for TupleForRange");
	using type = decltype(
		std::tuple_cat(
			std::tuple< typename C<S>::type>(),
			typename TupleForRange<S + 1, E, C, S==E-2>::type()
		)
	);
};

template<uint8_t S, uint8_t E, template<uint8_t> class C>
struct TupleForRange<S, E, C, true> {
	using type = std::tuple<typename C<S>::type>;
};

template<class Functor, class Tuple, std::size_t I = 0, typename ...FuncArgs>
inline typename std::enable_if<I == std::tuple_size<Tuple>::value, void>::type
tuple_for_each_type(FuncArgs&&... func_args) {}

template<class Functor, class Tuple, std::size_t I = 0, typename ...FuncArgs>
inline typename std::enable_if < I < std::tuple_size<Tuple>::value, void>::type
tuple_for_each_type(FuncArgs&&... func_args) {
	Functor::calc<I, typename std::tuple_element<I, Tuple>::type>(func_args...);
	tuple_for_each_type<Functor, Tuple, I + 1>(func_args...);
}

template<int ...I>
struct IntSequence {};

template<>
struct IntSequence<> {
	static constexpr int x = 0;
	static constexpr int y = 0;
	static constexpr int z = 0;
	static constexpr int sum = 0;
	static constexpr int max = 0;
	static constexpr int min = 0;
	static constexpr int size = 0;
};

template<int I>
struct IntSequence<I> {
	static constexpr int x = I;
	static constexpr int y = 0;
	static constexpr int z = 0;
	static constexpr int sum = I;
	static constexpr int max = I;
	static constexpr int min = I;
	static constexpr int size = 1;
};

template<int I, int... Is>
struct IntSequence<I, Is...> {
	using _t = IntSequence<Is...>;
	static constexpr int x = I;
	static constexpr int y = _t::x;
	static constexpr int z = _t::y;
	static constexpr int sum = I + IntSequence<Is...>::sum;
	static constexpr int max = I > IntSequence<Is...>::max ?
		I : IntSequence<Is...>::max;
	static constexpr int min = I < IntSequence<Is...>::min ?
		I : IntSequence<Is...>::min;
	static constexpr int size = 1 + IntSequence<Is...>::size;
};

template<int I1, int I2>
using Vector2 = IntSequence<I1, I2>;
template<int I1, int I2, int I3>
using Vector3 = IntSequence<I1, I2, I3>;

#define VALIDATE(Validator, ...) typename std::enable_if<Validator<__VA_ARGS__>::value, int>::type
#define STRINGIFY(s) #s
#define LOG_VECTOR3(Vector3_) Vector3_::x, Vector3_::y, Vector3_::z
#define UE_LOG_VECTOR3(Vector_) UE_LOG(LogTemp, Log, TEXT(STRINGIFY(Vector_: (%d, %d, %d))), LOG_VECTOR3(Vector_));

template<typename T> struct IsIntSequence : std::integral_constant<bool, false> {};
template<int ...Is> struct IsIntSequence<IntSequence<Is...>> : std::integral_constant<bool, true> {};

template<typename T> struct IsVector3 : std::integral_constant<bool, false> {};
template<int X, int Y, int Z> struct IsVector3<Vector3<X, Y, Z>> : std::integral_constant<bool, true> {};
template<typename T> struct IsVector2 : std::integral_constant<bool, false> {};
template<int X, int Y> struct IsVector2<Vector2<X, Y>> : std::integral_constant<bool, true> {};

template<class Sequence1, class Sequence2,
	VALIDATE(IsIntSequence, Sequence1) = 0,
	VALIDATE(IsIntSequence, Sequence2) = 0>
	struct IsSameSize {
	static constexpr bool value = Sequence1::size == Sequence2::size;
};

template<int Index, class Sequence>
struct Indexer {};

template<int I, int... Is>
struct Indexer<0, IntSequence<I, Is...>> {
	static constexpr int value = I;
};

template<int Index, int I, int... Is>
struct Indexer<Index, IntSequence<I, Is...>> :
	public Indexer<Index - 1, IntSequence<Is...>> {
};

template<class First, class Second>
struct Append {
private:
	using _throw1 = VALIDATE(IsIntSequence, First);
	using _throw2 = VALIDATE(IsIntSequence, Second);
};

template<int ...Is, int ...Js>
struct Append<IntSequence<Is...>, IntSequence<Js...>> {
	using type = IntSequence<Is..., Js...>;
};

template<class Sequence>
struct PopBack {
private:
	using _throw = VALIDATE(IsIntSequence, Sequence);
};

template<int Last>
struct PopBack<IntSequence<Last>> {
	using type = IntSequence<>;
};

template<int H, int... Is>
struct PopBack<IntSequence<H, Is...>> {
	using type = typename Append<IntSequence<H>,
		typename PopBack<IntSequence<Is...> >::type >::type;
};

enum class Trim {
	Entry,
	Front,
	Back,
	Both,
	None
};

template<int Start, int End, class Sequence, Trim Complete = Trim::Entry>
struct Slice {
private:
	using _throw = VALIDATE(IsIntSequence, Sequence);
};

template<int Start, int End, int... Is>
struct Slice<Start, End, IntSequence<Is...>, Trim::None> {
	using type = IntSequence<Is...>;
};

template<int Start, int End, int Head, int... Is>
struct Slice<Start, End, IntSequence<Head, Is...>, Trim::Entry > {
	using type = typename Slice<
		Start,
		End,
		IntSequence<Head, Is...>,
		Start == 0 && End >= sizeof...(Is) + 1 ? Trim::None :
		Start == 0 ? Trim::Back :
		End >= sizeof...(Is) + 1 ? Trim::Front :
		Trim::Both
	>::type;
};

template<int Start, int End, int Head, int... Is>
struct Slice<Start, End, IntSequence<Head, Is...>, Trim::Front > {
	using type = typename Slice<
		Start - 1,
		sizeof...(Is),
		IntSequence<Is...>,
		Start == 1 ? Trim::None : Trim::Front
	>::type;
};

template<int End, int Head, int... Is>
struct Slice<0, End, IntSequence<Head, Is...>, Trim::Back > {
	using type = typename Slice<
		0,
		End,
		typename PopBack<IntSequence<Head, Is...>>::type,
		End >= sizeof...(Is) ? Trim::None : Trim::Back
	>::type;
};

template<int Start, int End, int Head, int... Is>
struct Slice<Start, End, IntSequence<Head, Is...>, Trim::Both > {
	static constexpr int size = sizeof...(Is);
	using type = typename Slice<
		Start - 1,
		End - 1,
		typename PopBack<IntSequence<Is...>>::type,
		Start == 1 && End >= sizeof...(Is) ? Trim::None :
		Start == 1 ? Trim::Back :
		End >= sizeof...(Is) ? Trim::Front :
		Trim::Both
	>::type;
};

template<class T> struct multiplies { static constexpr T calc(const T& lhs, const T& rhs) { return lhs * rhs; } };
template<class T> struct adds { static constexpr T calc(const T& lhs, const T& rhs) { return lhs + rhs; } };
template<class T> struct subtracts { static constexpr T calc(const T& lhs, const T& rhs) { return lhs - rhs; } };
template<class T> struct divides { static constexpr T calc(const T& lhs, const T& rhs) { return lhs / rhs; } };

template<class Sequence, int I, class Operation>
struct ScalarOperator {
private:
	using _throw = VALIDATE(IsIntSequence, Sequence);
};

template<int K, class Operation>
struct ScalarOperator<IntSequence<>, K, Operation> {
	using type = IntSequence<>;
};

template<int K, int I, class Operation>
struct ScalarOperator<IntSequence<I>, K, Operation> {
	using type = IntSequence<Operation::calc(I, K)>;
};

template<int K, int I, class Operation, int... Is>
struct ScalarOperator<IntSequence<I, Is...>, K, Operation> {
	using type = typename Append<IntSequence<Operation::calc(I, K)>,
		typename ScalarOperator<IntSequence<Is...>, K, Operation>::type>::type;
};

template<class Sequence, int K, VALIDATE(IsIntSequence, Sequence)=0>
struct ScalarMul : public ScalarOperator<Sequence, K, multiplies<int>> {};
template<class Sequence, int K, VALIDATE(IsIntSequence, Sequence)=0>
struct ScalarDiv : public ScalarOperator<Sequence, K, divides<int>> {};
template<class Sequence, int K, VALIDATE(IsIntSequence, Sequence)=0>
struct ScalarAdd : public ScalarOperator<Sequence, K, adds<int>> {};
template<class Sequence, int K, VALIDATE(IsIntSequence, Sequence)=0>
struct ScalarSub : public ScalarOperator<Sequence, K, subtracts<int>> {};

template<class Sequence1, class Sequence2, class Operation>
struct VectorOperator {
private:
	using _throw1 = VALIDATE(IsIntSequence, Sequence1);
	using _throw2 = VALIDATE(IsIntSequence, Sequence2);
	using _throw3 = VALIDATE(IsSameSize, Sequence1, Sequence2);
};

template<class Operation>
struct VectorOperator<IntSequence<>, IntSequence<>, Operation> {
	using type = IntSequence<>;
};

template<int I, int ...Is, class Operation, int J, int ...Js>
struct VectorOperator<IntSequence<I, Is...>, IntSequence<J, Js...>, Operation> {
	using type = typename Append<IntSequence<Operation::calc(I, J)>,
		typename VectorOperator<IntSequence<Is...>, IntSequence<Js...>, Operation>::type>::type;
};

template<class Sequence1, class Sequence2,
VALIDATE(IsSameSize, Sequence1, Sequence2) = 0>
struct VectorMul : public VectorOperator<Sequence1, Sequence2, multiplies<int>> {};

template<class Sequence1, class Sequence2,
VALIDATE(IsSameSize, Sequence1, Sequence2)=0>
struct VectorAddLD : public VectorOperator<Sequence1, Sequence2, adds<int>> {};

template<class Sequence1, class Sequence2,
VALIDATE(IsSameSize, Sequence1, Sequence2)=0>
struct VectorDiv : public VectorOperator<Sequence1, Sequence2, divides<int>> {};

template<class Sequence1, class Sequence2,
VALIDATE(IsSameSize, Sequence1, Sequence2)=0>
struct VectorSub : public VectorOperator<Sequence1, Sequence2, subtracts<int>> {};