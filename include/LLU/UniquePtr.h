/**
 * @file	UniquePtr.h
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	March 18, 2024
 * @brief
 */
#pragma once

#include <memory>
#include <type_traits>
#include <utility>

#include "LibraryData.h"

namespace LLU {
	namespace detail {
		template<class T>
		struct Deleter {
			static_assert(std::is_trivial_v<T> && std::is_standard_layout_v<T>, "In LLU::UniquePtr<T> T must be a trivial, standard layout type.");

			using pointer = T*;

			Deleter() noexcept = default;

			template<typename U, typename = typename std::enable_if_t<std::is_convertible_v<U*, T*>>>
			Deleter(const Deleter<U>& /*other*/) noexcept {}

			void operator()(pointer p) {
				LibraryData::API()->WL_free(p);
			}
		};

		template<class T>
		struct Deleter<T[]> {
			static_assert(std::is_trivial_v<T> && std::is_standard_layout_v<T>, "In LLU::UniquePtr<T[]> T must be a trivial, standard layout type.");

			Deleter() noexcept = default;

			template<typename U, typename = typename std::enable_if_t<std::is_convertible_v<U (*)[], T (*)[]>>>
			Deleter(const Deleter<U[]>& /*other*/) noexcept {}

			template<typename U, typename = typename std::enable_if_t<std::is_convertible_v<U (*)[], T (*)[]>>>
			void operator()(U* p) {
				LibraryData::API()->WL_free(p);
			}
		};
	}	 // namespace detail

	/// wrt::UniquePtr<T> is a std::unique_ptr to an object of type T that was created in memory allocated with Wolfram's allocator.
	template<class T>
	using UniquePtr = std::unique_ptr<T, detail::Deleter<T>>;

	/**
	 * @brief   Create an object on the heap and a unique pointer to it using wrt::Allocator.
	 * @tparam  T - type of the object to be created
	 * @tparam  Args - types of arguments for the object creation
	 * @param   args - arguments for the object's constructor
	 * @return  a unique pointer to the newly created object which will use Wolfram's deallocation function when destroying
	 * the owned object
	 */
	template<class T>
	std::enable_if_t<!std::is_array<T>::value, UniquePtr<T>> makeUnique() {
		auto* raw_memory = static_cast<T*>(LibraryData::API()->WL_malloc(sizeof(T)));
		if (!raw_memory) {
			ErrorManager::throwException(ErrorName::MemoryError);
		}
		return UniquePtr<T>(raw_memory, detail::Deleter<T> {});
	}

	/**
	 * @brief   Create an array of n value-initialized objects in a dynamically allocated storage.
	 * @tparam  T - unbounded array type (e.g. mint[] is fine and will result in a wrt::UniquePtr to an array of mints, but mint[7] is not allowed)
	 * @param   n - number of objects in the array to create
	 * @return  a unique pointer to the newly created array of objects
	 */
	template<class T>
	std::enable_if_t<std::is_unbounded_array_v<T>, UniquePtr<T>> makeUnique(std::size_t n) {
		using ElementType = std::remove_extent_t<T>;
		auto* raw_memory = static_cast<ElementType*>(LibraryData::API()->WL_malloc(n * sizeof(ElementType)));
		if (!raw_memory) {
			ErrorManager::throwException(ErrorName::MemoryError);
		}
		return UniquePtr<T>(raw_memory);
	}

	// For consistency with std::make_unique disallow creating a unique pointer from a known-bound array type
	template<class T, class... Args>
	std::enable_if_t<std::is_bounded_array_v<T>> makeUnique(Args&&...) = delete;

}
