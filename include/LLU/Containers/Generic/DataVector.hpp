/**
 * @file    DataVector.hpp
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

		struct NumericData {
			void* raw_data;
			numericarray_data_t type;
			std::size_t length;
		};

		struct StringData {
			std::string_view characters;
			std::span<mint> offsets;
		};

		struct BinaryData {
			std::span<std::uint8_t> array;
			std::span<mint> offsets;
		};

		struct FixedWidthBinaryData {
			std::span<std::uint8_t> array;
			mint width;
		};

		struct DateData {
			NumericData numeric_data;
			mint granularity;
			mint precision;
			std::string time_zone;
		};

		struct TimeData {
			NumericData numeric_data;
			mint granularity;
			mint precision;
		};

		using Data = std::variant<NumericData, StringData, Int8Array, BinaryData, FixedWidthBinaryData, DateData, TimeData>;

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

		/**
		 * Create a new DataVector of numeric type or a fixed-width binary type
		 * @param type - data type (either Numeric of FixedWidthBinary)
		 * @param array - values to be stored in the DataVector
		 * @param validity - missing values array
		 */
		MContainer(DV::Type type, GenericNumericArray&& array, const Int8Array& validity = {});

		/**
		 * Create a new DataVector of string type from a flat list of characters and an offset array
		 * @param str_count - number of data elements
		 * @param string_data - list of characters, no copies made, ownership passed to the DataVector
		 * @param offsets - array of offsets for each string, ownership passed to the DataVector
		 * @param validity - missing values array
		 */
		MContainer(mint str_count, UniquePtr<const char>&& string_data, UniquePtr<mint[]>&& offsets, const Int8Array& validity = {});

		/**
		 * Create a new DataVector of string type from a vector of string views
		 * @param string_data
		 * @param validity - missing values array
		 */
		MContainer(const std::vector<std::string_view>& string_data, const Int8Array& validity = {});

		/**
		 * Create a new DataVector of boolean type
		 * @param boolean_data - array of boolean values (True = 1, False = 0)
		 * @param validity - missing values array
		 */
		MContainer(const Int8Array& boolean_data, const Int8Array& validity = {});

		/**
		 * Create a new DataVector of binary type (a list of byte arrays)
		 * @param array - byte values in a flat buffer, no copies made, data will be moved to the underlying structure
		 * @param elem_count - number of byte arrays
		 * @param offsets - offset of each byte array in the flat buffer
		 * @param validity - missing values array
		 */
		MContainer(GenericNumericArray&& array, mint elem_count, UniquePtr<mint[]>&& offsets, const Int8Array& validity = {});

		/**
		 * Create a new DataVector of date type
		 * @param array - date values, no copies made, data will be moved to the underlying structure
		 * @param granularity - date granularity
		 * @param precision - date precision
		 * @param time_zone - time zone
		 * @param validity - missing values array
		 */
		MContainer(GenericNumericArray&& array, mint granularity, mint precision, const std::string& time_zone, const Int8Array& validity = {});

		/**
		 * Create a new DataVector of time type
		 * @param array - time values, no copies made, data will be moved to the underlying structure
		 * @param granularity - time granularity
		 * @param precision - time precision
		 * @param validity - missing values array
		 */
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
		 * @brief   Get the length of the DataVector as mint
		 * @return  total number of data elements in the DataVector
		 */
		mint length() const;

		/**
		 * @brief   Get the length of the DataVector as std::size_t
		 * @return  total number of data elements in the DataVector
		 */
		std::size_t size() const;

		/// Get DataVector type as an enum
		DV::Type type() const;

		/// Get the number of missing elements in the DataVector
		mint missingCount() const;

		/// Get the validity array (for each element: 0 -> missing, 1 -> present)
		Int8Array validity() const;

		/// See the underlying data without making any copies
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
			MArgument_setTabularColumn(res, this->getContainer());
		}
	};

	/// MContainer specialization for TabularColumn is called GenericDataList
	using DataVector = MContainer<MArgumentType::TabularColumn>;

}  // namespace LLU

#endif	  // LLU_CONTAINERS_GENERIC_DATAVECTOR_HPP
