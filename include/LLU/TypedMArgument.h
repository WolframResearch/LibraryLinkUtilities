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

#include <LLU/Containers/Generic/DataStore.hpp>
#include <LLU/MArgument.h>

namespace LLU {

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
	constexpr bool isTypedArgument = (variant_index<TypedArgument, T>() < std::variant_size_v<TypedArgument>);

	TypedArgument fromMArgument(MArgument m, MArgumentType t);

	MArgument toMArgument(const TypedArgument& tma);
}

#endif	  // LIBRARYLINKUTILITIES_TYPEDMARGUMENT_H
