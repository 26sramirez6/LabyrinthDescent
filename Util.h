// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

template<int ...I>
struct IntSequence {};

template<>
struct IntSequence<> {
	static constexpr int sum = 0;
	static constexpr int max = 0;
	static constexpr int min = 0;
	static constexpr int size = 0;
};

template<int I>
struct IntSequence<I> {
	static constexpr int sum = I;
	static constexpr int max = I;
	static constexpr int min = I;
	static constexpr int size = 1;
};

template<int I, int... Is>
struct IntSequence<I, Is...> {
	static constexpr int sum = I + IntSequence<Is...>::sum;
	static constexpr int max = I > IntSequence<Is...>::max ?
		I : IntSequence<Is...>::max;
	static constexpr int min = I < IntSequence<Is...>::min ?
		I : IntSequence<Is...>::min;
	static constexpr int size = 1 + IntSequence<Is...>::size;
};

template<int Index, class Sequence>
struct IntAtIndex {};

template<int I, int... Is>
struct IntAtIndex<0, IntSequence<I, Is...>> {
	static constexpr int value = I;
};

template<int Index, int I, int... Is>
struct IntAtIndex<Index, IntSequence<I, Is...>> :
	public IntAtIndex<Index - 1, IntSequence<Is...>> {
};

template<class First, class Second>
struct Append;

template<int... Ts, int... Us>
struct Append<IntSequence<Ts...>, IntSequence<Us...>> {
	using type = IntSequence<Ts..., Us...>;
};

template<class Sequence>
struct PopBack {};

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
struct Slice;

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

template<int... Is>
struct Slice<0, sizeof...(Is), IntSequence<Is...>, Trim::None > {
	using type = IntSequence<Is...>;
};

template<int Start, int Head, int... Is>
struct Slice<Start, sizeof...(Is) + 1, IntSequence<Head, Is...>, Trim::Front > {
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