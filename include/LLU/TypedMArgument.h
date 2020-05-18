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

/**
 * @namespace
 * @brief   Namespace for functionality related to arguments passed from LibraryLink to library functions and their types.
 */
namespace LLU::Argument {

	/**
	 * @namespace
	 * @brief   Namespace defining C++ types corresponding to primitive LibraryLink argument types.
	 */
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

	/// C++ wrapper over LibraryLink's MArgument, which is a plain union
	using TypedArgument = std::variant<std::monostate, Typed::Boolean, Typed::Integer, Typed::Real, Typed::Complex, Typed::Tensor, Typed::SparseArray,
									   Typed::NumericArray, Typed::Image, Typed::UTF8String, Typed::DataStore>;
	namespace Typed {
		using Any = TypedArgument;
	}

	/**
	 * @brief   Index of a wrapper type in the TypedArgument variant or std::variant_size_v<TypedArgument> otherwise.
	 * @tparam  T - any type
	 */
	template<typename T>
	constexpr MArgumentType WrapperIndex = static_cast<MArgumentType>(variant_index<TypedArgument, T>());

	/**
	 * @brief   Type trait to determine if given type is a "wrapper type" (i.e. if it is a member of TypedArgument variant)
	 * @tparam  T - any type
	 */
	template<typename T>
	constexpr bool WrapperQ = std::is_same_v<T, TypedArgument> || (variant_index<TypedArgument, T>() < std::variant_size_v<TypedArgument>);

	/**
	 * @brief   Get wrapper type corresponding to primitive LibraryLink argument type T
	 * @tparam  T - an enumerated value of type MArgumentType
	 */
	template<MArgumentType T>
	using WrapperType = std::conditional_t<T == MArgumentType::MArgument, TypedArgument, std::variant_alternative_t<static_cast<size_t>(T), TypedArgument>>;

	/**
	 * @brief   Get a TypeArgument from plain MArgument and an enum describing the active union member.
	 * @param   m - MArgument
	 * @param   t - index of the active member of the MArgument m
	 * @return  a TypedArgument whose active member is a wrapper of the active member of m
	 * @notice  When creating a wrapper of a LibraryLink container, no deep copy is being made, so in a way this function
	 *          acts more like a cast than a conversion.
	 */
	TypedArgument fromMArgument(MArgument m, MArgumentType t);

	/**
	 * @brief       Assign current value of given TypedArgument to given MArgument.
	 * @param[in]   tma - a TypedArgument
	 * @param[out]  res - raw MArgument to be assigned to
	 * @notice      When extracting the underlying raw value from a wrapper type, no deep copy is being made, so in a way this function
	 *              acts more like a cast than a conversion.
	 */
	void toMArgument(const TypedArgument& tma, MArgument& res);

	/**
	 * @brief   Convert a value of one of the LibraryLink argument type to its wrapper
	 * @tparam  T - MArgument type index
	 * @param   value - a value of the LibraryLink argument type corresponding to T (e.g. MArgumentType::Complex corresponds to mcomplex type)
	 * @return  LLU wrapper of given value (e.g. MTensor -> GenericTensor, mcomplex -> std::complex<double>, mbool -> bool, etc.)
	 */
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

	/**
	 * @brief   Convert value of one of the wrapper types (see LLU::Argument::Typed namespace) to the corresponding LibraryLink type
	 * @tparam  T - MArgument type index
	 * @param   value - a value of the wrapper type corresponding to T
	 * @return  plain LibraryLink type value extracted from its wrapper (e.g. GenericTensor -> MTensor, std::complex<double> -> mcomplex, bool -> mbool, etc.)
	 */
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
