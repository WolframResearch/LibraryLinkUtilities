/**
 * @file    TabularColumn.hpp
 * @brief   Definition and implementation of generic TabularColumn wrapper.
 */

#ifndef LLU_CONTAINERS_GENERIC_DATAVECTOR_HPP
#define LLU_CONTAINERS_GENERIC_DATAVECTOR_HPP

#include <numeric>
#include <span>
#include <string_view>
#include <variant>

#include "LLU/Containers/Generic/Base.hpp"
#include "LLU/Containers/Generic/NumericArray.hpp"
#include "LLU/Containers/NumericArray.h"
#include "LLU/Containers/Views/NumericArray.hpp"
#include "LLU/UniquePtr.h"

namespace LLU {

	namespace DV {
		enum class Type {
			Numeric = 0,
			String,
			Boolean,
			Binary,
			FixedWidthBinary,
			Date,
			Time
		};

		struct StringData {
			std::string_view characters;
			std::span<mint> offsets;
		};

		struct BinaryData {
			GenericNumericArray array;
			std::span<mint> offsets;
		};

		struct DateData {
			GenericNumericArray array;
			mint granularity;
			mint precision;
			std::string time_zone;
		};

		struct TimeData {
			GenericNumericArray array;
			mint granularity;
			mint precision;
		};

		using Data = std::variant<GenericNumericArray, StringData, GenericNumericArray, BinaryData, GenericNumericArray, DateData, TimeData>;

		template<typename T>
		UniquePtr<mint[]> lengthsToOffsets(std::span<T> lengths) {
			const auto elem_count = lengths.size();
			auto offsets = makeUnique<mint[]>(elem_count + 1);
			offsets[0] = 0;
			std::partial_sum(lengths.begin(), lengths.end(), offsets.get() + 1);
			return offsets;
		}

		template<typename T>
		Int8Array exceptionalValuesAsMissing(const NumericArrayTypedView<T>& array) {
			if constexpr (std::is_floating_point_v<T>) {
				const auto elem_count = array.getFlattenedLength();
				Int8Array validity {static_cast<std::uint8_t>(true), MArrayDimensions {elem_count}};
				for (mint i = 0; i < elem_count; ++i) {
					if (std::isinf(array[i]) || std::isnan(array[i])) {
						validity[i] = 0;
					}
				}
				return validity;
			} else {
				return {};
			}
		}

		Int8Array exceptionalValuesAsMissing(const GenericNumericArray& array);
	}

	/**
	 *  @brief  MContainer specialization for TabularColumn, provides basic list interface for the underlying raw TabularColumn.
	 */
	template<>
	class MContainer<MArgumentType::TabularColumn> : public MContainerBase<MArgumentType::TabularColumn> {
	public:
		/// Inherit constructors from MContainerBase
		using MContainerBase<MArgumentType::TabularColumn>::MContainerBase;

		/**
		 * @brief   Default constructor, the MContainer does not manage any instance of TabularColumn.
		 */
		MContainer() = default;

		/**
		 * @brief   Create new MContainer wrapping a given raw TabularColumn
		 * @param   c - a TabularColumn
		 * @param   owner - who manages the memory the raw TabularColumn
		 * @note    An exception will be thrown if you try to create a Shared TabularColumn because LibraryLink does not allow for shared DataVectors.
		 */
		MContainer(Container c, Ownership owner);


		MContainer(DV::Type type, GenericNumericArray&& array, const Int8Array& validity = {});

		MContainer(mint str_count, UniquePtr<const char>&& string_data, UniquePtr<mint[]>&& offsets, const Int8Array& validity = {});

		MContainer(const std::vector<std::string_view>& string_data, const Int8Array& validity = {});

		MContainer(const Int8Array& boolean_data, const Int8Array& validity = {});

		MContainer(GenericNumericArray&& array, mint elem_count, UniquePtr<mint[]>&& offsets, const Int8Array& validity = {});

		MContainer(GenericNumericArray&& array, mint granularity, mint precision, const std::string& time_zone, const Int8Array& validity = {});

		MContainer(GenericNumericArray&& array, mint granularity, mint precision, const Int8Array& validity = {});
		/**
		 * @brief   Clone this MContainer, performs a deep copy of the underlying TabularColumn.
		 * @note    The cloned MContainer always belongs to the library (Ownership::Library) because LibraryLink has no idea of its existence.
		 * @return  new MContainer, by value
		 */
		MContainer clone() const {
			return MContainer {cloneContainer(), Ownership::Library};
		}

		/**
		 * @brief   Get the length of the TabularColumn.
		 * @return  total number of data elements in the TabularColumn
		 */
		mint length() const;

		std::size_t size() const;

		DV::Type type() const;

		mint missingCount() const;

		Int8Array validity() const;

		DV::Data viewData();

	private:
		/// Make a deep copy of the raw container
		Container cloneImpl() const override;

		/**
		 * @brief   Get a share count.
		 * @return  always 0 to indicate that TabularColumn cannot be shared
		 */
		mint shareCountImpl() const noexcept override {
			return 0;
		}

		/**
		 * @brief   Pass the internal container as result of a LibraryLink function.
		 * @param   res - MArgument which will hold the internal container of this MContainer
		 */
		void passImpl(MArgument& res) const noexcept override {
			//NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast): c-style cast used in a macro in WolframIOLibraryFunctions.h
			MArgument_setTabularColumn(res, this->getContainer());
		}
	};

	/// MContainer specialization for TabularColumn is called GenericDataList
	using DataVector = MContainer<MArgumentType::TabularColumn>;

}  // namespace LLU

#endif	  // LLU_CONTAINERS_GENERIC_DATAVECTOR_HPP
