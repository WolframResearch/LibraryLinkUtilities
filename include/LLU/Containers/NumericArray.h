/**
 * @file	NumericArray.h
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	6/07/2017
 *
 * @brief	Templated C++ wrapper for MNumericArray
 *
 */
#ifndef LLUTILS_NUMERICARRAY_H_
#define LLUTILS_NUMERICARRAY_H_

#include <type_traits>
#include <initializer_list>

#include "WolframLibrary.h"
#include "WolframNumericArrayLibrary.h"

#include "LLU/ErrorLog/ErrorManager.h"
#include "MArray.hpp"
#include "LLU/Utilities.hpp"

namespace LibraryLinkUtils {

	namespace NA {
		/**
		 * @brief Possible methods of handling out-of-range data when converting a NumericArray to different type.
		 */
		enum class ConversionMethod {
			Check = MNumericArray_Convert_Check,
			ClipCheck = MNumericArray_Convert_Clip_Check,
            Coerce = MNumericArray_Convert_Coerce,
			ClipCoerce = MNumericArray_Convert_Clip_Coerce,
			Round = MNumericArray_Convert_Round,
			ClipRound = MNumericArray_Convert_Clip_Round,
			Scale = MNumericArray_Convert_Scale,
			ClipScale = MNumericArray_Convert_Clip_Scale,
		};
	}

	/**
	 * @class NumericArray
	 * @brief This is a class template, where template parameter T is the type of data elements. NumericArray is derived from MArray.
	 *
	 * NumericArray<> classes automate creation and deletion of MNumericArrays.
	 * They are strongly typed (no void* to underlying memory) and almost all functions from \<algorithms\> can be used on NumericArray.
	 *
	 * @tparam	T - type of underlying data
	 */
	template<typename T>
	class NumericArray: public MArray<T> {
	public:
		/**
		 *	@brief Default constructor. There is no internal MNumericArray stored.
		 **/
		NumericArray() = default;

		/**
		 *   @brief         Constructs flat NumericArray based on a container of elements
		 *   @param[in]     v - container
		 *   @tparam		Container - any container with elements convertible to type \b T
		 *   @throws		see NumericArray<T>::NumericArray(InputIt, InputIt, std::initializer_list<mint>)
		 *
		 *   @warning		It is user's responsibility to make sure that length of v fits into mint!
		 **/
		template<typename Container, typename = disable_if_same_or_derived<NumericArray<T>, Container>>
		NumericArray(Container&& v);

		/**
		 *   @brief         Constructs flat NumericArray based on a list of elements
		 *   @param[in]     v - initializer list with NumericArray elements
		 *   @throws		see NumericArray<T>::NumericArray(InputIt, InputIt, std::initializer_list<mint>)
		 *
		 *   @warning		It is user's responsibility to make sure that length of v fits into mint!
		 **/
		NumericArray(std::initializer_list<T> v);

		/**
		 *   @brief         Constructs flat NumericArray with elements from range [first, last)
		 *   @param[in]     first - iterator to the beginning of range
		 *   @param[in]		last - iterator past the end of range
		 *   @tparam		InputIt - any iterator conforming to InputIterator concept
		 *   @throws		see NumericArray<T>::NumericArray(InputIt, InputIt, std::initializer_list<mint>)
		 *
		 *   @warning		It is user's responsibility to make sure that length of range fits into mint!
		 *   @note			Be aware that efficiency of this constructor strongly depends on whether the InputIt is also a RandomAccessIterator
		 **/
		template<class InputIt, typename = enable_if_input_iterator<InputIt>>
		NumericArray(InputIt first, InputIt last);

		/**
		 *   @brief         Constructs the NumericArray of given shape with all elements initialized to given value
		 *   @param[in]     init - value of type \b T to initialize all elements of the NumericArray
		 *   @param[in]     dims - container with NumericArray dimensions
		 *   @tparam		Container - any type of container that has member \b value_type and this type is convertible to mint
		 *   @throws		see NumericArray<T>::createInternal() and MArray<T>::MArray(Container&&)
		 **/
		template<class Container, typename = enable_if_integral_elements<Container>>
		NumericArray(T init, Container&& dims);

		/**
		 *   @brief         Constructs the NumericArray of given shape with all elements initialized to given value
		 *   @param[in]     init - value of type \b T to initialize all elements of the NumericArray
		 *   @param[in]     dims - initializer list with NumericArray dimensions
		 *   @throws		see NumericArray<T>::createInternal() and MArray<T>::MArray(std::initializer_list<mint>)
		 **/
		NumericArray(T init, std::initializer_list<mint> dims);

		/**
		 *   @brief         Constructs the NumericArray of given shape with elements from range [first, last)
		 *   @param[in]     first - iterator to the beginning of range
		 *   @param[in]		last - iterator past the end of range
		 *   @param[in]     dims - container with NumericArray dimensions
		 *   @tparam		Container - any type of container that has member \b value_type and this type is convertible to mint
		 *   @throws		LLErrorName::NumericArrayNewError - if number of elements in \c v does not match total NumericArray size indicated by \c dims
		 *   @throws		see NumericArray<T>::createInternal() and MArray<T>::MArray(Container&&)
		 **/
		template<
			class InputIt,
			class Container,
			typename = enable_if_integral_elements<Container>,
			typename = enable_if_input_iterator<InputIt>
		>
		NumericArray(InputIt first, InputIt last, Container&& dims);

		/**
		 *   @brief         Constructs the NumericArray of given shape with elements from range [first, last)
		 *   @param[in]     first - iterator to the beginning of range
		 *   @param[in]		last - iterator past the end of range
		 *   @param[in]     dims - initializer list with NumericArray dimensions
		 *   @throws		LLErrorName::NumericArrayNewError - if number of elements in \c v does not match total NumericArray size indicated by \c dims
		 *   @throws		see NumericArray<T>::createInternal() and MArray<T>::MArray(const std::vector<U>&)
		 **/
		template<class InputIt, typename = enable_if_input_iterator<InputIt>>
		NumericArray(InputIt first, InputIt last, std::initializer_list<mint> dims);

		/**
		 *   @brief         Constructs NumericArray based on MNumericArray
		 *   @param[in]     na - LibraryLink structure to be wrapped
		 *   @throws        LLErrorName::NumericArrayInitError - if structure with WolframNumericArrayLibrary functions is not initialized
		 *   @throws		LLErrorName::NumericArrayTypeError - if template parameter \b T does not match MNumericArray data type
		 **/
		NumericArray(const MNumericArray na);

		/**
		 *   @brief         Create NumericArray from NumericArray of different type
		 *   @tparam		U - data type of the NumericArray to be converted
		 *   @param[in]     other - const reference to a NumericArray any type
		 *   @param[in]		method - conversion method to be used
		 *   @param[in]     tolerance - undocumented parameter for NumericArray conversion that became part of the public interface
		 **/
		template<typename U>
		NumericArray(const NumericArray<U>& other, NA::ConversionMethod method, double tolerance);

		/**
		 *   @brief         Copy constructor
		 *   @param[in]     other - const reference to a NumericArray of matching type
		 **/
		NumericArray(const NumericArray<T>& other);
		
		/**
		 *   @brief         Move constructor
		 *   @param[in]     other - rvalue reference to a NumericArray of matching type
		 **/
		NumericArray(NumericArray<T>&& other);


		/**
		 *   @brief         Copy-assignment operator
		 *   @param[in]     other - const reference to a NumericArray of matching type
		 **/
		void operator=(const NumericArray<T>& other);

		/**
		 *   @brief         Move-assignment operator
		 *   @param[in]     other - rvalue reference to a NumericArray of matching type
		 **/
		void operator=(NumericArray<T>&& other);


		/**
		 *    @brief	Free internal MNumericArray if necessary
		 **/
		virtual ~NumericArray();


		/**
		 *   @brief	Return matching type from numericarray_data_t enum
		 **/
		static numericarray_data_t getType() noexcept {
			return type;
		}

		/**
		 *   @brief 	Return internal MNumericArray. You should rarely need this function.
		 *   @warning 	Do not modify MNumericArray returned from this function!
		 **/
		MNumericArray getInternal() const noexcept {
			return internalNA;
		}

		/**
		 *   @brief 	Return share count of internal MNumericArray.
		 *   Use this to manually manage shared MNumericArrays.
		 *
		 *   @note		LibraryLink does not provide any way to check whether MNumericArray was passed or will be returned as "Shared".
		 *   Therefore passing or returning MNumericArrays as "Shared" is discouraged and if you do that you are responsible for managing MNumericArray memory.
		 **/
		mint shareCount() const noexcept {
			if (internalNA == nullptr)
				return 0;
			return this->naFuns->MNumericArray_shareCount(internalNA);
		}

		/**
		 *   @brief 	Disown internal MNumericArray that is shared with Mathematica.
		 *   Use this to manually manage shared MNumericArrays.
		 *
		 *   @note		LibraryLink does not provide any way to check whether MNumericArray was passed or will be returned as "Shared".
		 *   Therefore passing or returning MNumericArrays as "Shared" is discouraged and if you do that you are responsible for managing MNumericArray memory.
		 **/
		void disown() const noexcept {
			this->naFuns->MNumericArray_disown(internalNA);
		}

	private:
		/**
		 *   @brief Return raw pointer to underlying data
		 **/
		T* getData() const noexcept override {
			return static_cast<T*>(this->naFuns->MNumericArray_getData(internalNA));
		}

		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::indexError()
		 *   @throws 	LibraryLinkError(LLErrorName::NumericArrayIndexError)
		 **/
		void indexError() const override {
			ErrorManager::throwException(LLErrorName::NumericArrayIndexError);
		}

		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::initError()
		 *   @throws 	LibraryLinkError(LLErrorName::NumericArrayInitError)
		 **/
		void initError() const override {
			ErrorManager::throwException(LLErrorName::NumericArrayInitError);
		}

		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::sizeError()
		 *   @throws 	LibraryLinkError(LLErrorName::NumericArraySizeError)
		 **/
		void sizeError() const override {
			ErrorManager::throwException(LLErrorName::NumericArraySizeError);
		}


		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::createInternal(). It creates a new flat MNumericArray.
		 *   @throws 	LibraryLinkError(LLErrorName::NumericArrayNewError)
		 *
		 **/
		void createInternal() override {
			if (this->naFuns->MNumericArray_new(type, this->depth, this->dimensionsData(), &internalNA))
				ErrorManager::throwException(LLErrorName::NumericArrayNewError);
		}

		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::freeInternal(). It frees internal MNumericArray.
		 **/
		void freeInternal() noexcept override {
			if (internalNA == nullptr)
				return;
			if (shareCount() > 0)
				disown();
			if (this->isOwner())
				this->naFuns->MNumericArray_free(internalNA);
		}

		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::passInternal(MArgument&). Passes internal MNumericArray as result to LibraryLink.
		 **/
		void passInternal(MArgument& res) noexcept override {
			MArgument_setMNumericArray(res, internalNA);
		}

		/**
		 * @brief 	Set \p na as internal container and extract properties like dimensions, depth and total length.
		 * @param 	na - MNumericArray of matching type
		 */
		void extractPropertiesFromInternal(const MNumericArray na);

		/// MNumericArray type matching template parameter T
		static const numericarray_data_t type;

		/// Internal container
		MNumericArray internalNA = nullptr;
	};



	template<typename T>
	template<typename Container, typename>
	NumericArray<T>::NumericArray(Container&& v) :
			NumericArray<T>(std::begin(v), std::end(v), { static_cast<mint>(v.size()) }) {
	}

	template<typename T>
	NumericArray<T>::NumericArray(std::initializer_list<T> v) :
			NumericArray<T>(std::begin(v), std::end(v), { static_cast<mint>(v.size()) }) {
	}

	template<typename T>
	template<class InputIt, typename>
	NumericArray<T>::NumericArray(InputIt first, InputIt last) :
			NumericArray<T>(first, last, { static_cast<mint>(std::distance(first, last)) }) {
	}

	template<typename T>
	template<class Container, typename>
	NumericArray<T>::NumericArray(T init, Container&& dims) : MArray<T>(std::forward<Container>(dims)) {
		createInternal();
		this->setOwner(true);
		std::fill(this->begin(), this->end(), init);
	}

	template<typename T>
	NumericArray<T>::NumericArray(T init, std::initializer_list<mint> dims) : MArray<T>(dims) {
		createInternal();
		this->setOwner(true);
		std::fill(this->begin(), this->end(), init);
	}

	template<typename T>
	template<class InputIt, class Container, typename, typename>
	NumericArray<T>::NumericArray(InputIt first, InputIt last, Container&& dims) : MArray<T>(std::forward<Container>(dims)) {
		createInternal();
		if (std::distance(first, last) != this->flattenedLength)
			ErrorManager::throwExceptionWithDebugInfo(LLErrorName::NumericArrayNewError, "Length of data range does not match specified dimensions");
		this->setOwner(true);
		std::copy(first, last, this->begin());
	}

	template<typename T>
	template<class InputIt, typename>
	NumericArray<T>::NumericArray(InputIt first, InputIt last, std::initializer_list<mint> dims) : MArray<T>(dims) {
		createInternal();
		if (std::distance(first, last) != this->flattenedLength)
			ErrorManager::throwExceptionWithDebugInfo(LLErrorName::NumericArrayNewError, "Length of data range does not match specified dimensions");
		this->setOwner(true);
		std::copy(first, last, this->begin());
	}

	template<typename T>
	void NumericArray<T>::extractPropertiesFromInternal(const MNumericArray na) {
		internalNA = na;
		this->depth = this->naFuns->MNumericArray_getRank(internalNA);
		this->flattenedLength = this->naFuns->MNumericArray_getFlattenedLength(internalNA);
		const mint* rawDims = this->naFuns->MNumericArray_getDimensions(internalNA);
		this->dims.assign(rawDims, rawDims + this->rank());
		this->fillOffsets();
	}

	template<typename T>
	NumericArray<T>::NumericArray(const MNumericArray na) {
		if (!this->naFuns)
			initError();
		if (type != this->naFuns->MNumericArray_getType(na))
			ErrorManager::throwException(LLErrorName::NumericArrayTypeError);
		this->setOwner(false);
		extractPropertiesFromInternal(na);
	}

	template<typename T>
	template<typename U>
	NumericArray<T>::NumericArray(const NumericArray<U>& other, NA::ConversionMethod method, double tolerance) : MArray<T>(other)  {
		if (!this->naFuns) {
			initError();
		}
		MNumericArray newInternal = nullptr;
		auto status = this->naFuns->MNumericArray_convertType(&newInternal, other.getInternal(), type, static_cast<numericarray_convert_method_t>(method), tolerance);
		if (status) { // 0 means OK, but no way to learn anything more specific if conversion failed
			ErrorManager::throwExceptionWithDebugInfo(LLErrorName::NumericArrayConversionError,
					"Conversion from type " + std::to_string(static_cast<int>(other.getType())) + " to " + std::to_string(static_cast<int>(type)) + " failed.");
		}
		this->setOwner(true);
		extractPropertiesFromInternal(newInternal);
	}

	template<typename T>
	NumericArray<T>::NumericArray(const NumericArray<T>& other) : MArray<T>(other) {
		if (this->naFuns->MNumericArray_clone(other.internalNA, &this->internalNA)) {
			ErrorManager::throwException(LLErrorName::NumericArrayCloneError);
		}
		this->setOwner(true);
	}

	template<typename T>
	NumericArray<T>::NumericArray(NumericArray<T>&& other) :
			MArray<T>(std::move(other)) {
		this->internalNA = other.internalNA;
		other.internalNA = nullptr;
		other.setOwner(false);
	}


	template<typename T>
	void NumericArray<T>::operator=(const NumericArray<T>& other) {
		MArray<T>::operator=(other);
		this->freeInternal();
		if (this->naFuns->MNumericArray_clone(other.internalNA, &this->internalNA)) {
			ErrorManager::throwException(LLErrorName::NumericArrayCloneError);
		}
		this->setOwner(true);
	}

	template<typename T>
	void NumericArray<T>::operator=(NumericArray<T>&& other) {
		MArray<T>::operator=(std::move(other));
		this->freeInternal();
		this->internalNA = other.internalNA;
		this->setOwner(other.isOwner());
		other.internalNA = nullptr;
		other.setOwner(false);
	}

	template<typename T>
	NumericArray<T>::~NumericArray() {
		this->freeInternal();
	}
} /* namespace LibraryLinkUtils */

#endif /* LLUTILS_NUMERICARRAY_H_ */
