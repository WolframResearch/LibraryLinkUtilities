/**
 * @file	DataVector.cpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	March 11, 2024
 * @brief
 */

#include "LLU/Containers/Generic/DataVector.hpp"

#include <numeric>

namespace LLU {

	namespace DV {
		std::string typeName(Type t) {
			switch (t) {
				case Type::Numeric: return "Numeric";
				case Type::String: return "String";
				case Type::Boolean: return "Boolean";
				case Type::Binary: return "Binary";
				case Type::FixedWidthBinary: return "FixedWidthBinary";
				case Type::Date: return "Date";
				case Type::Time: return "Time";
			}
			return "<Unknown>";
		}

		LLU::Int8Array exceptionalValuesAsMissing(const GenericNumericArray& array) {
			return LLU::asTypedNumericArray(array, [](const auto& typedNA) {
				return exceptionalValuesAsMissing(typedNA);
			});
		}
	}

	void checkAPICall(error_t status) {
		if (status) {
			ErrorManager::throwException(ErrorName::DVAPIError, status);
		}
	}

	GenericDataVector::MContainer(Container c, Ownership owner) : MContainerBase {c, owner} {
		if (owner == Ownership::Shared) {
			ErrorManager::throwException(ErrorName::DVShared);
		}
	}

	GenericDataVector::MContainer(DV::Type type, GenericNumericArray&& array, const Int8Array& validity) {
		const auto* api = LibraryData::DataVectorAPI();
		auto internal = array.abandonContainer(); // now the NumericArray belongs to the DataVector and DataVector must free it when it dies
		Container result;
		if (type == DV::Type::Numeric) {
			checkAPICall(api->DataVector_newNumeric(&internal, validity.getContainer(), &result));
		} else if (type == DV::Type::FixedWidthBinary) {
			checkAPICall(api->DataVector_newFixedWidthBinary(&internal, validity.getContainer(), &result));
		} else {
			ErrorManager::throwException(ErrorName::DVConstructorType, DV::typeName(type));
		}
		reset(result);
	}

	GenericDataVector::MContainer(mint str_count, UniquePtr<const char>&& string_data, UniquePtr<mint[]>&& offsets, const Int8Array& validity) {
		const auto* api = LibraryData::DataVectorAPI();
		Container result;
		checkAPICall(api->DataVector_newString(str_count, string_data.release(), offsets.release(), validity.getContainer(), &result));
		reset(result);
	}

	GenericDataVector::MContainer(const std::vector<std::string_view>& string_data, const Int8Array& validity) {
		const auto str_count = string_data.size();
		auto offsets = LLU::makeUnique<mint[]>(str_count + 1);

		offsets[0] = 0;
		auto offsets_span = std::span<mint> {offsets.get() + 1, str_count};
		std::transform_inclusive_scan(string_data.begin(), string_data.end(), offsets_span.begin(), std::plus<mint>{}, [](auto sv) { return sv.length(); });

		const auto total_chars = offsets_span.back();
		auto characters = LLU::makeUnique<char[]>(std::max(total_chars, static_cast<mint>(1))); // allocate a dummy 1-byte block if there are no characters
		offsets_span = std::span<mint> {offsets.get(), str_count};
		for (std::size_t i = 0; i < str_count; ++i) {
			std::copy(string_data[i].begin(), string_data[i].end(), characters.get() + offsets_span[i]);
		}
		const auto* api = LibraryData::DataVectorAPI();
		Container result;
		checkAPICall(api->DataVector_newString(str_count, characters.release(), offsets.release(), validity.getContainer(), &result));
		reset(result);
	}

	GenericDataVector::MContainer(GenericNumericArray&& array, mint elem_count, UniquePtr<mint[]>&& offsets, const Int8Array& validity) {
		const auto* api = LibraryData::DataVectorAPI();
		auto internal = array.abandonContainer();
		Container result;
		checkAPICall(api->DataVector_newBinary(elem_count, &internal, offsets.release(), validity.getContainer(), &result));
		reset(result);
	}

	GenericDataVector::MContainer(const Int8Array& boolean_data, const Int8Array& validity) {
		const auto* api = LibraryData::DataVectorAPI();
		Container result;
		checkAPICall(api->DataVector_newBoolean(boolean_data.getContainer(), validity.getContainer(), &result));
		reset(result);
	}

	GenericDataVector::MContainer(GenericNumericArray&& array, mint granularity, mint precision, const std::string& time_zone, const Int8Array& validity) {
		const auto* api = LibraryData::DataVectorAPI();
		auto internal = array.abandonContainer();
		const char* zone = time_zone.empty()? nullptr : time_zone.c_str();
		Container result;
		checkAPICall(api->DataVector_newDate(&internal, granularity, precision, zone, validity.getContainer(), &result));
		reset(result);
	}

	GenericDataVector::MContainer(GenericNumericArray&& array, mint granularity, mint precision, const Int8Array& validity) {
		const auto* api = LibraryData::DataVectorAPI();
		auto internal = array.abandonContainer();
		Container result;
		checkAPICall(api->DataVector_newTime(&internal, granularity, precision, validity.getContainer(), &result));
		reset(result);
	}

	mint GenericDataVector::length() const {
		mint length;
		checkAPICall(LibraryData::DataVectorAPI()->DataVector_getLength(this->getContainer(), &length));
		return length;
	}

	std::size_t GenericDataVector::size() const {
		return static_cast<std::size_t>(length());
	}

	auto GenericDataVector::cloneImpl() const -> Container {
		Container newDataVector;
		checkAPICall(LibraryData::DataVectorAPI()->DataVector_clone(this->getContainer(), &newDataVector));
		return newDataVector;
	}


	DV::Type GenericDataVector::type() const {
		const auto* api = LibraryData::DataVectorAPI();
		if (True == api->DataVector_numericTypeQ(getContainer())) {
			return DV::Type::Numeric;
		}
		if (True == api->DataVector_stringTypeQ(getContainer())) {
			return DV::Type::String;
		}
		if (True == api->DataVector_booleanTypeQ(getContainer())) {
			return DV::Type::Boolean;
		}
		if (True == api->DataVector_binaryTypeQ(getContainer())) {
			return DV::Type::Binary;
		}
		if (True == api->DataVector_fixedWidthBinaryTypeQ(getContainer())) {
			return DV::Type::FixedWidthBinary;
		}
		if (True == api->DataVector_dateTypeQ(getContainer())) {
			return DV::Type::Date;
		}
		if (True == api->DataVector_timeTypeQ(getContainer())) {
			return DV::Type::Time;
		}
		ErrorManager::throwException(ErrorName::TypeError);
	}

	mint GenericDataVector::missingCount() const {
		mint count;
		checkAPICall(LibraryData::DataVectorAPI()->DataVector_getMissingCount(this->getContainer(), &count));
		return count;
	}

	Int8Array GenericDataVector::validity() const {
		MNumericArray validity;
		checkAPICall(LibraryData::DataVectorAPI()->DataVector_getValidity(this->getContainer(), &validity));
		return Int8Array {validity, Ownership::Library};
	}

	DV::Data GenericDataVector::viewData() {
		const auto* api = LibraryData::DataVectorAPI();
		const auto dv_type = type();
		switch (dv_type) {
			case DV::Type::Numeric: {
				MNumericArray raw_result;
				checkAPICall(api->DataVector_getDataNumeric(this->getContainer(), &raw_result));
				constexpr auto variant_index = std::in_place_index<static_cast<std::size_t>(DV::Type::Numeric)>;
				// MNumericArray shares memory with DataVector.
				return DV::Data {variant_index, GenericNumericArray {raw_result, Ownership::LibraryLink}};
			}
			case DV::Type::String: {
				const auto num_offsets = size() + 1;
				char* raw_data;
				mint* offsets;
				checkAPICall(api->DataVector_getDataString(this->getContainer(), &raw_data, &offsets));
				DV::StringData result;
				result.offsets = {offsets, num_offsets};
				const auto total_char_count = static_cast<std::size_t>(result.offsets.back());
				result.characters = {raw_data, total_char_count};
				return DV::Data {std::move(result)};
			}
			case DV::Type::Boolean: {
				constexpr auto variant_index = std::in_place_index<static_cast<std::size_t>(DV::Type::Boolean)>;
				MNumericArray raw_data;
				checkAPICall(api->DataVector_getDataBoolean(this->getContainer(), &raw_data));
				// raw_result is a copy of DataVector data, so the caller is responsible for freeing it
				return DV::Data {variant_index, Int8Array {raw_data, Ownership::Library}};
			}
			case DV::Type::Binary: {
				MNumericArray raw_data;
				mint* offsets;
				checkAPICall(api->DataVector_getDataBinary(this->getContainer(), &raw_data, &offsets));
				DV::BinaryData result {{raw_data, Ownership::LibraryLink}, {offsets, static_cast<std::size_t>(length() + 1)}};
				return DV::Data {std::move(result)};
			}
			case DV::Type::FixedWidthBinary: {
				MNumericArray raw_result;
				checkAPICall(api->DataVector_getDataFixedWidthBinary(this->getContainer(), &raw_result));
				constexpr auto variant_index = std::in_place_index<static_cast<std::size_t>(DV::Type::FixedWidthBinary)>;
				// MNumericArray shares memory with DataVector.
				return DV::Data {variant_index, GenericNumericArray {raw_result, Ownership::LibraryLink}};
			}
			case DV::Type::Date: {
				MNumericArray raw_data;
				char* raw_time_zone;
				DV::DateData result;
				checkAPICall(api->DataVector_getDataDate(this->getContainer(), &raw_data, &result.granularity, &result.precision, &raw_time_zone));
				result.array = GenericNumericArray {raw_data, Ownership::LibraryLink};
				result.time_zone = raw_time_zone? raw_time_zone : "";
				return DV::Data {std::move(result)};
			}
			case DV::Type::Time: {
				MNumericArray raw_data;
				DV::TimeData result;
				checkAPICall(api->DataVector_getDataTime(this->getContainer(), &raw_data, &result.granularity, &result.precision));
				result.array = GenericNumericArray {raw_data, Ownership::LibraryLink};
				return DV::Data {std::move(result)};
			}
		}
		ErrorManager::throwException(ErrorName::TypeError);
	}
}