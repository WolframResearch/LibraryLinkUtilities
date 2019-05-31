/**
 * @file	MArgument.cpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	September 03, 2018
 * @brief	Specializations of Argument class template members.
 */

#include "LLU/MArgument.h"

#include "LLU/ErrorLog/ErrorManager.h"

namespace LibraryLinkUtils {

#define ARGUMENT_DEFINE_SPECIALIZATIONS_OF_MEMBER_FUNCTIONS(ArgType, MArgGetPrefix, MArgSetPrefix, DSAdd) \
	template<> auto Argument<MArgumentType::ArgType>::get() -> typename Argument::value_type& {\
		return MArgGetPrefix##ArgType(arg);\
	}\
	template<> auto Argument<MArgumentType::ArgType>::get() const -> const typename Argument::value_type& {\
		return MArgGetPrefix##ArgType(arg);\
	}\
	template<> void Argument<MArgumentType::ArgType>::addDataStoreNode(DataStore ds, const std::string& name, const value_type& val) {\
		ioFuns->DataStore_##DSAdd(ds, const_cast<char*>(name.c_str()), val);\
	}\
	template<> auto Argument<MArgumentType::ArgType>::getAddress() const -> typename Argument::value_type* {\
		return MArgGetPrefix##ArgType##Address(arg);\
	}\
	template<> void Argument<MArgumentType::ArgType>::set(typename Argument::value_type newValue) {\
		MArgSetPrefix##ArgType(arg, newValue);\
	}

	ARGUMENT_DEFINE_SPECIALIZATIONS_OF_MEMBER_FUNCTIONS(Boolean, MArgument_get, MArgument_set, addNamedBoolean)
	ARGUMENT_DEFINE_SPECIALIZATIONS_OF_MEMBER_FUNCTIONS(Integer, MArgument_get, MArgument_set, addNamedInteger)
	ARGUMENT_DEFINE_SPECIALIZATIONS_OF_MEMBER_FUNCTIONS(Real, MArgument_get, MArgument_set, addNamedReal)
	ARGUMENT_DEFINE_SPECIALIZATIONS_OF_MEMBER_FUNCTIONS(Complex, MArgument_get, MArgument_set, addNamedComplex)
	ARGUMENT_DEFINE_SPECIALIZATIONS_OF_MEMBER_FUNCTIONS(DataStore, MArgument_get, MArgument_set, addNamedDataStore)
	ARGUMENT_DEFINE_SPECIALIZATIONS_OF_MEMBER_FUNCTIONS(UTF8String, MArgument_get, MArgument_set, addNamedString)
	ARGUMENT_DEFINE_SPECIALIZATIONS_OF_MEMBER_FUNCTIONS(Tensor, MArgument_getM, MArgument_setM, addNamedMTensor)
	ARGUMENT_DEFINE_SPECIALIZATIONS_OF_MEMBER_FUNCTIONS(SparseArray, MArgument_getM, MArgument_setM, addNamedMSparseArray)
	ARGUMENT_DEFINE_SPECIALIZATIONS_OF_MEMBER_FUNCTIONS(NumericArray, MArgument_getM, MArgument_setM, addNamedMNumericArray)
	ARGUMENT_DEFINE_SPECIALIZATIONS_OF_MEMBER_FUNCTIONS(Image, MArgument_getM, MArgument_setM, addNamedMImage)

#undef ARGUMENT_DEFINE_SPECIALIZATIONS_OF_MEMBER_FUNCTIONS


	template<> auto Argument<MArgumentType::MArgument>::get() -> typename Argument::value_type& {
		return arg;
	}
	template<> auto Argument<MArgumentType::MArgument>::get() const -> const typename Argument::value_type& {
		return arg;
	}
	template<> auto Argument<MArgumentType::MArgument>::getAddress() const -> typename Argument::value_type* {
		return &arg;
	}
	template<> void Argument<MArgumentType::MArgument>::set(typename Argument::value_type newValue) {
		arg = newValue;
	}
	template<> void Argument<MArgumentType::MArgument>::addToDataStore(DataStore ds, const std::string& name, MArgumentType T) const {
		switch(T) {
			case MArgumentType::MArgument:
				ErrorManager::throwException(LLErrorName::ArgumentAddNodeMArgument);
			case MArgumentType::Boolean:
				Argument<MArgumentType::Boolean>(arg).addToDataStore(ds, name);
				break;
			case MArgumentType::Integer:
				Argument<MArgumentType::Integer>(arg).addToDataStore(ds, name);
				break;
			case MArgumentType::Real:
				Argument<MArgumentType::Real>(arg).addToDataStore(ds, name);
				break;
			case MArgumentType::Complex:
				Argument<MArgumentType::Complex>(arg).addToDataStore(ds, name);
				break;
			case MArgumentType::DataStore:
				Argument<MArgumentType::DataStore>(arg).addToDataStore(ds, name);
				break;
			case MArgumentType::UTF8String:
				Argument<MArgumentType::UTF8String>(arg).addToDataStore(ds, name);
				break;
			case MArgumentType::Tensor:
				Argument<MArgumentType::Tensor>(arg).addToDataStore(ds, name);
				break;
			case MArgumentType::SparseArray:
				Argument<MArgumentType::SparseArray>(arg).addToDataStore(ds, name);
				break;
			case MArgumentType::NumericArray:
				Argument<MArgumentType::NumericArray>(arg).addToDataStore(ds, name);
				break;
			case MArgumentType::Image:
				Argument<MArgumentType::Image>(arg).addToDataStore(ds, name);
				break;
		}
	}

}