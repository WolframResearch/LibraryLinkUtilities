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

namespace LLU {

	template<>
	class MContainer<MArgumentType::DataStore>;

	using TypedArgument =
	std::variant<std::monostate, bool, mint, double, std::complex<double>, MContainer<MArgumentType::Tensor>, MSparseArray,
		MContainer<MArgumentType::NumericArray>, MContainer<MArgumentType::Image>, std::string_view, MContainer<MArgumentType::DataStore>>;

	using PrimitiveArgument =
	std::variant<std::monostate, mbool, mint, mreal, mcomplex, MTensor, MSparseArray, MNumericArray, MImage, char*, DataStore>;

	/**
	 *
	 * @tparam VariantType
	 * @tparam T
	 * @tparam index
	 * @return
	 * @see     https://stackoverflow.com/questions/52303316/get-index-by-type-in-stdvariant
	 */
	template<typename VariantType, typename T, std::size_t index = 0>
	constexpr std::size_t variant_index() {
		if constexpr (index >= std::variant_size_v<VariantType>) {
			return index;
		} else if(std::is_same_v<std::variant_alternative_t<index, VariantType>, T>) {
			return index;
		} else {
			return variant_index<VariantType, T, index + 1>();
		}
	}

	template<typename T>
	constexpr MArgumentType typedArgumentIndex = static_cast<MArgumentType>(variant_index<TypedArgument, T>());

	template<typename T>
	constexpr MArgumentType primitiveArgumentIndex = static_cast<MArgumentType>(variant_index<PrimitiveArgument, T>());

	template<typename T>
	constexpr bool isTypedArgument = (variant_index<TypedArgument, T>() < std::variant_size_v<TypedArgument>);

	template<typename T>
	constexpr bool isPrimitiveArgument = (variant_index<PrimitiveArgument, T>() < std::variant_size_v<PrimitiveArgument>);

	template<MArgumentType T>
	using PrimitiveType = std::variant_alternative_t<static_cast<size_t>(T), PrimitiveArgument>;

	template<MArgumentType T>
	using WrapperType = std::variant_alternative_t<static_cast<size_t>(T), TypedArgument>;

	TypedArgument fromMArgument(MArgument m, MArgumentType t);

	void toMArgument(const TypedArgument& tma, MArgument& res);

	template<MArgumentType T>
	WrapperType<T> toWrapperType(const PrimitiveType<T>& value) {
		if constexpr (T == MArgumentType::Complex) {
			return {value->ri[0], value->ri[1]};
		} else if constexpr (T == MArgumentType::UTF8String) {
			return {value};
		} else if constexpr (isContainerType<T> && T != MArgumentType::SparseArray) {
			return {value, Ownership::LibraryLink};
		} else {
			return value;
		}
	}

	template<MArgumentType T>
	PrimitiveType<T> toPrimitiveType(const WrapperType<T>& value) {
		if constexpr (T == MArgumentType::Complex) {
			return {value.real(), value.imag()};
		} else if constexpr (T == MArgumentType::UTF8String) {
			return const_cast<char*>(value.data());
		} else if constexpr (isContainerType<T> && T != MArgumentType::SparseArray) {
			return value.abandonContainer();
		} else {
			return value;
		}
	}
}

#endif	  // LIBRARYLINKUTILITIES_TYPEDMARGUMENT_H
