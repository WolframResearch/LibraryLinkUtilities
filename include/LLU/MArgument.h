/**
 * @file	MArgument.h
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	September 01, 2018
 * @brief	Template class and utilities to work with MArgument in type-safe manner.
 */

#ifndef LLUTILS_MARGUMENT_H
#define LLUTILS_MARGUMENT_H

#include <string>

#include "WolframLibrary.h"
#include "WolframIOLibraryFunctions.h"

#include "LLU/LibraryLinkError.h"
#include "LLU/Containers/LibDataHolder.h"

namespace LibraryLinkUtils {

	enum class MArgumentType {
		MArgument = MType_Undef,
		Boolean = MType_Boolean,
		Integer = MType_Integer,
		Real = MType_Real,
		Complex = MType_Complex,
		Tensor = MType_Tensor,
		SparseArray = MType_SparseArray,
		NumericArray = MType_NumericArray,
		Image = MType_Image,
		UTF8String = MType_UTF8String,
		DataStore = MType_DataStore
	};

	template<MArgumentType T>
	constexpr bool alwaysFalse = false;

	template<MArgumentType T>
	struct MType;

	template<> struct MType<MArgumentType::MArgument>   { using type = MArgument; const std::string name = "MArgument"; };
	template<> struct MType<MArgumentType::Boolean>     { using type = mbool; const std::string name = "Boolean"; };
	template<> struct MType<MArgumentType::Integer>     { using type = mint; const std::string name = "Integer"; };
	template<> struct MType<MArgumentType::Real>        { using type = mreal; const std::string name = "Real"; };
	template<> struct MType<MArgumentType::Complex>     { using type = mcomplex; const std::string name = "Complex"; };
	template<> struct MType<MArgumentType::Tensor>      { using type = MTensor; const std::string name = "Tensor"; };
	template<> struct MType<MArgumentType::DataStore>   { using type = DataStore; const std::string name = "DataStore"; };
	template<> struct MType<MArgumentType::SparseArray> { using type = MSparseArray; const std::string name = "SparseArray"; };
	template<> struct MType<MArgumentType::NumericArray> { using type = MNumericArray; const std::string name = "NumericArray"; };
	template<> struct MType<MArgumentType::Image>       { using type = MImage; const std::string name = "Image"; };
	template<> struct MType<MArgumentType::UTF8String>  { using type = char*; const std::string name = "UTF8String"; };

	template<MArgumentType T>
	using MType_t = typename MType<T>::type;

	template<MArgumentType T>
	class Argument : public LibDataHolder {
	public:
		using value_type = MType_t<T>;

	public:
		explicit Argument(MArgument& a) : arg(a) {}

		value_type& get();
		const value_type& get() const;
		value_type* getAddress() const;
		void set(value_type newValue);
		void addToDataStore(DataStore ds, const std::string& name, MArgumentType = T) const;

		static void addDataStoreNode(DataStore ds, const std::string& name, const value_type& val);
	private:
		MArgument& arg;
	};

	template<MArgumentType T>
	void Argument<T>::addToDataStore(DataStore ds, const std::string& name, MArgumentType) const {
		addDataStoreNode(ds, name, get());
	}

#define ARGUMENT_DEFINE_SPECIALIZATIONS_OF_MEMBER_FUNCTIONS(ArgType) \
	template<> auto Argument<MArgumentType::ArgType>::get() -> typename Argument::value_type&;\
	template<> auto Argument<MArgumentType::ArgType>::get() const -> const typename Argument::value_type&;\
	template<> void Argument<MArgumentType::ArgType>::addDataStoreNode(DataStore ds, const std::string& name, const value_type& val);\
	template<> auto Argument<MArgumentType::ArgType>::getAddress() const -> typename Argument::value_type*;\
	template<> void Argument<MArgumentType::ArgType>::set(typename Argument::value_type newValue);

	ARGUMENT_DEFINE_SPECIALIZATIONS_OF_MEMBER_FUNCTIONS(Boolean)
	ARGUMENT_DEFINE_SPECIALIZATIONS_OF_MEMBER_FUNCTIONS(Integer)
	ARGUMENT_DEFINE_SPECIALIZATIONS_OF_MEMBER_FUNCTIONS(Real)
	ARGUMENT_DEFINE_SPECIALIZATIONS_OF_MEMBER_FUNCTIONS(Complex)
	ARGUMENT_DEFINE_SPECIALIZATIONS_OF_MEMBER_FUNCTIONS(Tensor)
	ARGUMENT_DEFINE_SPECIALIZATIONS_OF_MEMBER_FUNCTIONS(DataStore)
	ARGUMENT_DEFINE_SPECIALIZATIONS_OF_MEMBER_FUNCTIONS(SparseArray)
	ARGUMENT_DEFINE_SPECIALIZATIONS_OF_MEMBER_FUNCTIONS(NumericArray)
	ARGUMENT_DEFINE_SPECIALIZATIONS_OF_MEMBER_FUNCTIONS(Image)
	ARGUMENT_DEFINE_SPECIALIZATIONS_OF_MEMBER_FUNCTIONS(UTF8String)

	template<> auto Argument<MArgumentType::MArgument>::get() -> typename Argument::value_type&;
	template<> auto Argument<MArgumentType::MArgument>::get() const -> const typename Argument::value_type&;
	template<> void Argument<MArgumentType::MArgument>::addDataStoreNode(DataStore ds, const std::string& name, const value_type& val) = delete;
	template<> auto Argument<MArgumentType::MArgument>::getAddress() const -> typename Argument::value_type*;
	template<> void Argument<MArgumentType::MArgument>::set(typename Argument::value_type newValue);
	template<> void Argument<MArgumentType::MArgument>::addToDataStore(DataStore ds, const std::string& name, MArgumentType) const;

#undef ARGUMENT_DEFINE_SPECIALIZATIONS_OF_MEMBER_FUNCTIONS

} // namespace LibraryLinkUtils

#endif //LLUTILS_MARGUMENT_H
