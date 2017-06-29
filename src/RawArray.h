/**
 * @file	RawArray.h
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	18/04/2017
 *
 * @brief	Templated C++ wrapper for MRawArray
 *
 */
#ifndef LLUTILS_RAWARRAY_H_
#define LLUTILS_RAWARRAY_H_

#include "WolframLibrary.h"
#include "WolframRawArrayLibrary.h"

#include <type_traits>
#include <vector>

#include "LibraryLinkError.hpp"
#include "MArray.hpp"

namespace LibraryLinkUtils {

	/**
	 * @class RawArray
	 * @brief This is a class template, where template parameter T is the type of data elements. RawArray is derived from MArray.
	 *
	 * RawArray<> classes automate creation and deletion of MRawArrays.
	 * They are strongly typed (no void* to underlying memory) and almost all functions from \<algorithms\> can be used on RawArray.
	 *
	 * @tparam	T - type of underlying data
	 */
	template<typename T>
	class RawArray: public MArray<T> {

		///	Short name for exception class
		using LibraryLinkError = LibraryLinkUtils::LibraryLinkError<LLErrorCode>;

	public:

		/**
		 *   @brief         Constructs flat RawArray based on a vector of elements
		 *   @param[in]     v - const& to vector of elements of type \b T
		 *   @throws		see RawArray<T>::RawArray(T init, const std::vector<mint>&)
		 *
		 *   @note			It is user's responsibility to make sure that length of v fits into mint!
		 **/
		RawArray(const std::vector<T>& v);

		/// @copydoc RawArray::RawArray(const std::vector<T>&)
		RawArray(std::initializer_list<T> v);

		/**
		 *   @brief         Constructs the RawArray of given shape with all elements initialized to given value
		 *   @param[in]     init - value of type \b T to initialize all elements of the RawArray
		 *   @param[in]     dims - const& to vector of dimensions
		 *   @tparam		U - any type convertible to mint
		 *   @throws		see RawArray<T>::createInternal() and MArray<T>::MArray(const std::vector<U>&)
		 **/
		template<class Container, typename = typename std::enable_if<std::is_convertible<typename Container::value_type, mint>::value>::type>
		RawArray(T init, Container&& dims);

		/**
		 *   @brief         Constructs the RawArray of given shape with all elements initialized to given value
		 *   @param[in]     init - value of type \b T to initialize all elements of the RawArray
		 *   @param[in]     dims - initializer list with RawArray dimensions
		 *   @throws		see RawArray<T>::createInternal() and MArray<T>::MArray(const std::vector<U>&)
		 **/
		RawArray(T init, std::initializer_list<mint> dims);

		/**
		 *   @brief         Constructs the RawArray of given shape with element values taken from a given vector
		 *   @param[in]     v - const& to vector of values of type \b T to initialize all elements of the RawArray
		 *   @param[in]     dims - const& to vector of dimensions
		 *   @tparam		U - any type convertible to mint
		 *   @throws		LLErrorCode::RawArrayNewError - if number of elements in \c v does not match total RawArray size indicated by \c dims
		 *   @throws		see RawArray<T>::createInternal() and MArray<T>::MArray(const std::vector<U>&)
		 **/
		template<class Container, typename = typename std::enable_if<std::is_convertible<typename Container::value_type, mint>::value>::type>
		RawArray(const std::vector<T>& v, Container&& dims);

		/**
		 *   @brief         Constructs the RawArray of given shape with element values taken from a given vector
		 *   @param[in]     v - const& to vector of values of type \b T to initialize all elements of the RawArray
		 *   @param[in]     dims - initializer list with RawArray dimensions
		 *   @throws		LLErrorCode::RawArrayNewError - if number of elements in \c v does not match total RawArray size indicated by \c dims
		 *   @throws		see RawArray<T>::createInternal() and MArray<T>::MArray(const std::vector<U>&)
		 **/
		RawArray(const std::vector<T>& v, std::initializer_list<mint> dims);

		/**
		 *   @brief         Constructs RawArray based on MRawArray
		 *   @param[in]     ra - LibraryLink structure to be wrapped
		 *   @throws        LLErrorCode::RawArrayInitError - if structure with WolframRawArrayLibrary functions is not initialized
		 *   @throws		LLErrorCode::RawArrayTypeError - if template parameter \b T does not match MRawArray data type
		 **/
		RawArray(const MRawArray ra);

		/**
		 *   @brief         Move constructor
		 *   @param[in]     ra2 - rvalue reference to a RawArray of matching type
		 **/
		RawArray(RawArray<T> && ra2);

		/**
		 *   @brief	Free internal MRawArray if necessary
		 **/
		virtual ~RawArray();


		/**
		 *   @brief	Return matching type from rawarray_t enum
		 **/
		static rawarray_t getType() noexcept {
			return type;
		}

	private:
		/**
		 *   @brief Return raw pointer to underlying data
		 **/
		T* getData() const noexcept override {
			return static_cast<T*>(this->raFuns->MRawArray_getData(internalRA));
		}

		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::indexError()
		 *   @throws 	LibraryLinkError(LLErrorCode::RawArrayIndexError)
		 **/
		void indexError() const override {
			throw LibraryLinkError(LLErrorCode::RawArrayIndexError);
		}

		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::initError()
		 *   @throws 	LibraryLinkError(LLErrorCode::RawArrayInitError)
		 **/
		void initError() const override {
			throw LibraryLinkError(LLErrorCode::RawArrayInitError);
		}

		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::sizeError()
		 *   @throws 	LibraryLinkError(LLErrorCode::RawArraySizeError)
		 **/
		void sizeError() const override {
			throw LibraryLinkError(LLErrorCode::RawArraySizeError);
		}


		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::createInternal(). It creates a new flat MRawArray.
		 *   @throws 	LibraryLinkError(LLErrorCode::RawArrayNewError)
		 *
		 **/
		void createInternal() override {
			if (this->raFuns->MRawArray_new(type, this->depth, this->dimensionsData(), &internalRA))
				throw LibraryLinkError(LLErrorCode::RawArrayNewError);
		}

		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::freeInternal(). It frees internal MRawArray.
		 **/
		void freeInternal() noexcept override {
			this->raFuns->MRawArray_free(internalRA);
		}

		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::passInternal(MArgument&). Passes internal MRawArray as result to LibraryLink.
		 **/
		void passInternal(MArgument& res) noexcept override {
			MArgument_setMRawArray(res, internalRA);
		}

		/// MRawArray type matching template parameter T
		static const rawarray_t type;

		/// Internal container
		MRawArray internalRA = nullptr;
	};

	template<typename T>
	RawArray<T>::RawArray(const std::vector<T>& v) :
			RawArray<T>(v, { static_cast<mint>(v.size()) }) {
	}

	template<typename T>
	RawArray<T>::RawArray(std::initializer_list<T> v) :
			RawArray<T>(v, { static_cast<mint>(v.size()) }) {
	}

	template<typename T>
	template<class Container, typename>
	RawArray<T>::RawArray(T init, Container&& dims) : MArray<T>(dims) {
		createInternal();
		this->arrayOwnerQ = true;
		std::fill(this->begin(), this->end(), init);
	}

	template<typename T>
	RawArray<T>::RawArray(T init, std::initializer_list<mint> dims) : MArray<T>(dims) {
		createInternal();
		this->arrayOwnerQ = true;
		std::fill(this->begin(), this->end(), init);
	}


	template<typename T>
	template<class Container, typename>
	RawArray<T>::RawArray(const std::vector<T>& v, Container&& dims) : MArray<T>(dims) {
		createInternal();
		if (v.size() != this->flattenedLength)
			LibraryLinkError(LLErrorCode::RawArrayNewError, "Input data vector does not match specified dimensions");
		this->arrayOwnerQ = true;
		std::copy(std::begin(v), std::end(v), this->begin());
	}

	template<typename T>
	RawArray<T>::RawArray(const std::vector<T>& v, std::initializer_list<mint> dims) : MArray<T>(dims) {
		std::cout << "RawArray of depth = " << this->depth << " and length " << this->flattenedLength << std::endl;
		createInternal();
		if (v.size() != this->flattenedLength)
			LibraryLinkError(LLErrorCode::RawArrayNewError, "Input data vector does not match specified dimensions");
		this->arrayOwnerQ = true;
		std::copy(std::begin(v), std::end(v), this->begin());
	}

	template<typename T>
	RawArray<T>::RawArray(const MRawArray ra) {
		if (!this->raFuns)
			initError();
		if (type != this->raFuns->MRawArray_getType(ra))
			throw LibraryLinkError(LLErrorCode::RawArrayTypeError);
		this->arrayOwnerQ = false;
		internalRA = ra;
		this->depth = this->raFuns->MRawArray_getRank(internalRA);
		this->flattenedLength = this->raFuns->MRawArray_getFlattenedLength(internalRA);
		const mint* rawDims = this->raFuns->MRawArray_getDimensions(internalRA);
		this->dims.assign(rawDims, rawDims + this->rank());
		this->fillOffsets();
	}

	template<typename T>
	RawArray<T>::RawArray(RawArray<T> && t2) :
			MArray<T>(std::move(t2)) {
		this->internalRA = t2.internalRA;
		t2.internalRA = nullptr;
		t2.arrayOwnerQ = false;
	}

	template<typename T>
	RawArray<T>::~RawArray() {
		if (this->arrayOwnerQ)
			this->freeInternal();
	}
} /* namespace LibraryLinkUtils */

#endif /* LLUTILS_RAWARRAY_H_ */
