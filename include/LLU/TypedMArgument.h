/**
 * @file	TypedMArgument.h
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	April 28, 2020
 * @brief
 */
#ifndef LIBRARYLINKUTILITIES_TYPEDMARGUMENT_H
#define LIBRARYLINKUTILITIES_TYPEDMARGUMENT_H

#include <complex>
#include <string_view>
#include <variant>

#include <LLU/Containers/Generic/Image.hpp>
#include <LLU/Containers/Generic/NumericArray.hpp>
#include <LLU/Containers/Generic/Tensor.hpp>
#include <LLU/MArgument.h>
#include <LLU/Utilities.hpp>

template<>
class LLU::MContainer<LLU::MArgumentType::DataStore>;

namespace LLU::Argument {

	namespace Typed {
		using Boolean = bool;
		using Integer = mint;
		using Real = double;
		using Complex = std::complex<double>;
		using Tensor = MContainer<MArgumentType::Tensor>;
		using SparseArray = MSparseArray;
		using NumericArray = MContainer<MArgumentType::NumericArray>;
		using Image = MContainer<MArgumentType::Image>;
		using UTF8String = std::string_view;
		using DataStore = MContainer<MArgumentType::DataStore>;
	}

	using TypedArgument = std::variant<std::monostate, Typed::Boolean, Typed::Integer, Typed::Real, Typed::Complex, Typed::Tensor, Typed::SparseArray,
									   Typed::NumericArray, Typed::Image, Typed::UTF8String, Typed::DataStore>;
	namespace Typed {
		using Any = TypedArgument;
	}


	template<typename T>
	constexpr MArgumentType WrapperIndex = static_cast<MArgumentType>(variant_index<TypedArgument, T>());

	template<typename T>
	constexpr bool WrapperQ = std::is_same_v<T, TypedArgument> || (variant_index<TypedArgument, T>() < std::variant_size_v<TypedArgument>);

	template<MArgumentType T>
	using WrapperType = std::conditional_t<T == MArgumentType::MArgument, TypedArgument, std::variant_alternative_t<static_cast<size_t>(T), TypedArgument>>;

	TypedArgument fromMArgument(MArgument m, MArgumentType t);

	void toMArgument(const TypedArgument& tma, MArgument& res);

	template<MArgumentType T>
	WrapperType<T> toWrapperType(const CType<T>& value) {
		if constexpr (T == MArgumentType::Complex) {
			return {value->ri[0], value->ri[1]};
		} else if constexpr (T == MArgumentType::UTF8String) {
			return {value};
		} else if constexpr (ContainerTypeQ<T> && T != MArgumentType::SparseArray) {
			return {value, Ownership::LibraryLink};
		} else {
			return value;
		}
	}

	template<MArgumentType T>
	CType<T> toPrimitiveType(const WrapperType<T>& value) {
		if constexpr (T == MArgumentType::Complex) {
			return {value.real(), value.imag()};
		} else if constexpr (T == MArgumentType::UTF8String) {
			return const_cast<char*>(value.data());
		} else if constexpr (ContainerTypeQ<T> && T != MArgumentType::SparseArray) {
			return value.abandonContainer();
		} else {
			return value;
		}
	}
}

#endif	  // LIBRARYLINKUTILITIES_TYPEDMARGUMENT_H
