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

	void checkAPICall(errcode_t status) {
		if (status) {
			ErrorManager::throwException(ErrorName::DVAPIError, status);
		}
	}

	DataVector::MContainer(Container c, Ownership owner) : MContainerBase {c, owner} {
		if (owner == Ownership::Shared) {
			ErrorManager::throwException(ErrorName::DVShared);
		}
	}

	DataVector::MContainer(DV::Type type, GenericNumericArray&& array, const Int8Array& validity) {
		const auto* api = LibraryData::TabularAPI();
		auto internal = array.abandonContainer(); // now the NumericArray belongs to the TabularColumn and TabularColumn must free it when it dies
		Container result;
		if (type == DV::Type::Numeric) {
			checkAPICall(api->TabularColumn_newNumeric(internal, validity.getContainer(), &result));
		} else if (type == DV::Type::FixedWidthBinary) {
			checkAPICall(api->TabularColumn_newFixedWidthByteArray(internal, validity.getContainer(), &result));
		} else {
			ErrorManager::throwException(ErrorName::DVConstructorType, DV::typeName(type));
		}
		reset(result);
	}

	DataVector::MContainer(mint str_count, UniquePtr<const char>&& string_data, UniquePtr<mint[]>&& offsets, const Int8Array& validity) {
		const auto* api = LibraryData::TabularAPI();
		Container result;
		checkAPICall(api->TabularColumn_newString(str_count, string_data.release(), offsets.release(), validity.getContainer(), &result));
		reset(result);
	}

	DataVector::MContainer(const std::vector<std::string_view>& string_data, const Int8Array& validity) {
		const auto str_count = string_data.size();
		auto offsets = LLU::makeUnique<mint[]>(str_count + 1);

		offsets[0] = 0;
		auto offsets_span = std::span<mint> {offsets.get() + 1, str_count};
		std::transform_inclusive_scan(string_data.begin(), string_data.end(), offsets_span.begin(), std::plus<mint>{}, [](auto sv) { return sv.length(); },
									  static_cast<mint>(0));

		const auto total_chars = offsets_span.back();
		auto characters = LLU::makeUnique<char[]>(std::max(total_chars, static_cast<mint>(1))); // allocate a fake 1-byte block if there are no characters
		offsets_span = std::span<mint> {offsets.get(), str_count};
		for (std::size_t i = 0; i < str_count; ++i) {
			std::copy(string_data[i].begin(), string_data[i].end(), characters.get() + offsets_span[i]);
		}
		const auto* api = LibraryData::TabularAPI();
		Container result;
		checkAPICall(api->TabularColumn_newString(static_cast<mint>(str_count), characters.release(), offsets.release(), validity.getContainer(), &result));
		reset(result);
	}

	DataVector::MContainer(UInt8Array&& array, mint elem_count, UniquePtr<mint[]>&& offsets, const Int8Array& validity) {
		const auto* api = LibraryData::TabularAPI();
		auto internal = array.abandonContainer();
		Container result;
		checkAPICall(api->TabularColumn_newByteArray(elem_count, internal, offsets.release(), validity.getContainer(), &result));
		reset(result);
	}

	DataVector::MContainer(const Int8Array& boolean_data, const Int8Array& validity) {
		const auto* api = LibraryData::TabularAPI();
		Container result;
		checkAPICall(api->TabularColumn_newBoolean(boolean_data.getContainer(), validity.getContainer(), &result));
		reset(result);
	}

	DataVector::MContainer(GenericNumericArray&& array, mint granularity, mint precision, const std::string& time_zone, const Int8Array& validity) {
		const auto* api = LibraryData::TabularAPI();
		auto internal = array.abandonContainer();
		const char* zone = time_zone.empty()? nullptr : time_zone.c_str();
		Container result;
		checkAPICall(api->TabularColumn_newDate(internal, granularity, precision, zone, validity.getContainer(), &result));
		reset(result);
	}

	DataVector::MContainer(GenericNumericArray&& array, mint granularity, mint precision, const Int8Array& validity) {
		const auto* api = LibraryData::TabularAPI();
		auto internal = array.abandonContainer();
		Container result;
		checkAPICall(api->TabularColumn_newTime(internal, granularity, precision, validity.getContainer(), &result));
		reset(result);
	}

	mint DataVector::length() const {
		mint length;
		checkAPICall(LibraryData::TabularAPI()->TabularColumn_getLength(this->getContainer(), &length));
		return length;
	}

	std::size_t DataVector::size() const {
		return static_cast<std::size_t>(length());
	}

	auto DataVector::cloneImpl() const -> Container {
		Container newDataVector;
		checkAPICall(LibraryData::TabularAPI()->TabularColumn_clone(this->getContainer(), &newDataVector));
		return newDataVector;
	}


	DV::Type DataVector::type() const {
		const auto* api = LibraryData::TabularAPI();
		if (True == api->TabularColumn_numericTypeQ(getContainer())) {
			return DV::Type::Numeric;
		}
		if (True == api->TabularColumn_stringTypeQ(getContainer())) {
			return DV::Type::String;
		}
		if (True == api->TabularColumn_booleanTypeQ(getContainer())) {
			return DV::Type::Boolean;
		}
		if (True == api->TabularColumn_byteArrayTypeQ(getContainer())) {
			return DV::Type::Binary;
		}
		if (True == api->TabularColumn_fixedWidthByteArrayTypeQ(getContainer())) {
			return DV::Type::FixedWidthBinary;
		}
		if (True == api->TabularColumn_dateTypeQ(getContainer())) {
			return DV::Type::Date;
		}
		if (True == api->TabularColumn_timeTypeQ(getContainer())) {
			return DV::Type::Time;
		}
		ErrorManager::throwException(ErrorName::TypeError);
	}

	mint DataVector::missingCount() const {
		mint count;
		checkAPICall(LibraryData::TabularAPI()->TabularColumn_getMissingCount(this->getContainer(), &count));
		return count;
	}

	Int8Array DataVector::validity() const {
		MNumericArray validity;
		checkAPICall(LibraryData::TabularAPI()->TabularColumn_getValidity(this->getContainer(), &validity));
		return Int8Array {validity, Ownership::Library};
	}

	DV::Data DataVector::viewData() {
		const auto* api = LibraryData::TabularAPI();
		const auto dv_type = type();
		switch (dv_type) {
			case DV::Type::Numeric: {
				void* raw_data;
				numericarray_data_t data_type;
				checkAPICall(api->TabularColumn_getDataNumeric(this->getContainer(), &raw_data, &data_type));
				return DV::Data {DV::NumericData {.raw_data = raw_data, .type = data_type, .length = size()}};
			}
			case DV::Type::String: {
				const auto num_offsets = size() + 1;
				char* raw_data;
				mint* offsets;
				checkAPICall(api->TabularColumn_getDataString(this->getContainer(), &raw_data, &offsets));
				DV::StringData result;
				result.offsets = {offsets, num_offsets};
				const auto total_char_count = static_cast<std::size_t>(result.offsets.back());
				result.characters = {raw_data, total_char_count};
				return DV::Data {result};
			}
			case DV::Type::Boolean: {
				MNumericArray raw_data;
				checkAPICall(api->TabularColumn_getDataBoolean(this->getContainer(), &raw_data));
				// raw_data is a copy of TabularColumn data, so the caller is responsible for freeing it
				return DV::Data {Int8Array {raw_data, Ownership::Library}};
			}
			case DV::Type::Binary: {
				std::uint8_t* raw_data;
				mint* offsets;
				checkAPICall(api->TabularColumn_getDataByteArray(this->getContainer(), &raw_data, &offsets));
				DV::BinaryData result {.array = {raw_data, size()}, .offsets = {offsets, static_cast<std::size_t>(length() + 1)}};
				return DV::Data {result};
			}
			case DV::Type::FixedWidthBinary: {
				std::uint8_t* raw_data;
				mint width;
				checkAPICall(api->TabularColumn_getDataFixedWidthByteArray(this->getContainer(), &raw_data, &width));
				return DV::Data {DV::FixedWidthBinaryData {.array = {raw_data, size()}, .width = width}};
			}
			case DV::Type::Date: {
				void* raw_data;
				numericarray_data_t data_type;
				char* raw_time_zone;
				DV::DateData result;
				checkAPICall(
					api->TabularColumn_getDataDate(this->getContainer(), &raw_data, &data_type, &result.granularity, &result.precision, &raw_time_zone));
				result.numeric_data = {.raw_data = raw_data, .type = data_type, .length = size()};
				result.time_zone = raw_time_zone ? raw_time_zone : "";
				return DV::Data {std::move(result)};
			}
			case DV::Type::Time: {
				void* raw_data;
				numericarray_data_t data_type;
				DV::TimeData result {};
				checkAPICall(api->TabularColumn_getDataTime(this->getContainer(), &raw_data, &data_type, &result.granularity, &result.precision));
				result.numeric_data = {.raw_data = raw_data, .type = data_type, .length = size()};
				return DV::Data {result};
			}
		}
		ErrorManager::throwException(ErrorName::TypeError);
	}
}