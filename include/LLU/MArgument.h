/**
 * @file	MArgument.h
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	September 01, 2018
 * @brief	Template class and utilities to work with MArgument in type-safe manner.
 */

#ifndef LLUTILS_MARGUMENT_H
#define LLUTILS_MARGUMENT_H

#include <string>

#include "LLU/LibraryData.h"

namespace LLU {

	/**
	 * @brief Strongly type enum with possible types of data stored in MArgument.
	 */
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

	/**
	 * @brief Helper template variables that is always false. Useful in metaprogramming.
	 */
	template<MArgumentType T>
	constexpr bool alwaysFalse = false;

	/**
	 * @brief 	Structure that binds given MArgumentType to the actual C++ type it represents.
	 * @tparam 	T - any value of type MArgumentType
	 */
	template<MArgumentType T>
	struct MType;

	/// @cond
	template<>
	struct MType<MArgumentType::MArgument> {
		using type = MArgument;
		const std::string name = "MArgument";
	};
	template<>
	struct MType<MArgumentType::Boolean> {
		using type = mbool;
		const std::string name = "Boolean";
	};
	template<>
	struct MType<MArgumentType::Integer> {
		using type = mint;
		const std::string name = "Integer";
	};
	template<>
	struct MType<MArgumentType::Real> {
		using type = mreal;
		const std::string name = "Real";
	};
	template<>
	struct MType<MArgumentType::Complex> {
		using type = mcomplex;
		const std::string name = "Complex";
	};
	template<>
	struct MType<MArgumentType::Tensor> {
		using type = MTensor;
		const std::string name = "Tensor";
	};
	template<>
	struct MType<MArgumentType::DataStore> {
		using type = DataStore;
		const std::string name = "DataStore";
	};
	template<>
	struct MType<MArgumentType::SparseArray> {
		using type = MSparseArray;
		const std::string name = "SparseArray";
	};
	template<>
	struct MType<MArgumentType::NumericArray> {
		using type = MNumericArray;
		const std::string name = "NumericArray";
	};
	template<>
	struct MType<MArgumentType::Image> {
		using type = MImage;
		const std::string name = "Image";
	};
	template<>
	struct MType<MArgumentType::UTF8String> {
		using type = char*;
		const std::string name = "UTF8String";
	};
	/// @endcond

	/// Type alias for convenience
	template<MArgumentType T>
	using MType_t = typename MType<T>::type;

	/**
	 * @class	Argument
	 * @brief	Small class that wraps a reference to MArgument and provides proper API to work with this MArgument.
	 * @tparam 	T - any value of type MArgumentType
	 */
	template<MArgumentType T>
	class Argument {
	public:
		/// This is the actual type of data stored in \c arg
		using value_type = MType_t<T>;

	public:
		/**
		 * @brief 	Construct Argument from a reference to MArgument
		 * @param 	a - reference to MArgument
		 */
		explicit Argument(MArgument& a) : arg(a) {}

		/**
		 * @brief 	Get the value stored in MArgument
		 * @return	Reference to the value stored in MArgument
		 */
		value_type& get();

		/**
		 * @brief 	Get the read-only value stored in MArgument
		 * @return 	Const reference to the value stored in MArgument
		 */
		const value_type& get() const;

		/**
		 * @brief 	Get address of the value stored in MArgument. Every MArgument actually stores a pointer.
		 * @return	Pointer to the value stored in MArgument
		 */
		value_type* getAddress() const;

		/**
		 * @brief 	Set new value of type T in MArgument. Memory management is entirely user's responsibility.
		 * @param 	newValue - new value to be written to MArgument \c arg
		 */
		void set(value_type newValue);

		/**
		 * @brief 	Add \c arg to the DataStore ds inside a node named \c name
		 * The optional parameter should only be used by explicit specialization of this function for T equal to MArgumentType::MArgument
		 * @param 	ds - DataStore with values of type T
		 * @param 	name - name for the new node in the DataStore
		 */
		void addToDataStore(DataStore ds, const std::string& name, MArgumentType = T) const;

		/**
		 * @brief 	Add \c val to the DataStore \c ds inside a node named \c name
		 * This is a static method because there is no MArgument involved.
		 * @param 	ds - DataStore with values of type T
		 * @param 	name - name for the new node in the DataStore
		 * @param 	val - value of the new node in the DataStore
		 */
		static void addDataStoreNode(DataStore ds, const std::string& name, const value_type& val);

	private:
		MArgument& arg;
	};

	template<MArgumentType T>
	void Argument<T>::addToDataStore(DataStore ds, const std::string& name, MArgumentType) const {
		addDataStoreNode(ds, name, get());
	}

	/* Explicit specialization for member functions of Argument class */

#define ARGUMENT_DEFINE_SPECIALIZATIONS_OF_MEMBER_FUNCTIONS(ArgType)                                                       \
	template<>                                                                                                             \
	auto Argument<MArgumentType::ArgType>::get()->typename Argument::value_type&;                                          \
	template<>                                                                                                             \
	auto Argument<MArgumentType::ArgType>::get() const->const typename Argument::value_type&;                              \
	template<>                                                                                                             \
	void Argument<MArgumentType::ArgType>::addDataStoreNode(DataStore ds, const std::string& name, const value_type& val); \
	template<>                                                                                                             \
	auto Argument<MArgumentType::ArgType>::getAddress() const->typename Argument::value_type*;                             \
	template<>                                                                                                             \
	void Argument<MArgumentType::ArgType>::set(typename Argument::value_type newValue);

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

	template<>
	auto Argument<MArgumentType::MArgument>::get() -> typename Argument::value_type&;
	template<>
	auto Argument<MArgumentType::MArgument>::get() const -> const typename Argument::value_type&;
	template<>
	void Argument<MArgumentType::MArgument>::addDataStoreNode(DataStore ds, const std::string& name, const value_type& val) = delete;
	template<>
	auto Argument<MArgumentType::MArgument>::getAddress() const -> typename Argument::value_type*;
	template<>
	void Argument<MArgumentType::MArgument>::set(typename Argument::value_type newValue);
	template<>
	void Argument<MArgumentType::MArgument>::addToDataStore(DataStore ds, const std::string& name, MArgumentType) const;

#undef ARGUMENT_DEFINE_SPECIALIZATIONS_OF_MEMBER_FUNCTIONS

}	 // namespace LLU

#endif	  // LLUTILS_MARGUMENT_H
