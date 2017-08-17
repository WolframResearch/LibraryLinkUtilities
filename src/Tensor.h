/** 
 * @file	Tensor.h
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	6/07/2017
 *
 * @brief	Templated C++ wrapper for MTensor
 *
 */
#ifndef LLUTILS_TENSOR_H_
#define LLUTILS_TENSOR_H_

#include <initializer_list>
#include <type_traits>

#include "WolframLibrary.h"

#include "LibraryLinkError.h"
#include "MArray.hpp"

namespace LibraryLinkUtils {

	/**
	 * @class Tensor
	 * @brief This is a class template, where template parameter T is the type of data elements. Tensor is derived from MArray.
	 *
	 * Tensor<> classes automate creation and deletion of MTensors.
	 * They are strongly typed (no void* to underlying memory) and almost all functions from \<algorithms\> can be used on Tensor.
	 *
	 * @tparam	T - type of underlying data
	 */
	template<typename T>
	class Tensor: public MArray<T> {
	public:
		/**
		 *	@brief Default constructor. There is no internal MTensor stored.
		 **/
		Tensor() = default;

		/**
		 *   @brief         Constructs flat Tensor based on a container of elements
		 *   @param[in]     v - container
		 *   @tparam		Container - any container with elements convertible to type \b T
		 *   @throws		see Tensor<T>::Tensor(InputIt, InputIt, std::initializer_list<mint>)
		 *
		 *   @warning		It is user's responsibility to make sure that length of v fits into mint!
		 **/
		template<typename Container>
		Tensor(Container&& v);

		/**
		 *   @brief         Constructs flat Tensor based on a list of elements
		 *   @param[in]     v - initializer list with Tensor elements
		 *   @throws		see Tensor<T>::Tensor(InputIt, InputIt, std::initializer_list<mint>)
		 *
		 *   @warning		It is user's responsibility to make sure that length of v fits into mint!
		 **/
		Tensor(std::initializer_list<T> v);

		/**
		 *   @brief         Constructs flat Tensor with elements from range [first, last)
		 *   @param[in]     first - iterator to the beginning of range
		 *   @param[in]		last - iterator past the end of range
		 *   @tparam		InputIt - any iterator conforming to InputIterator concept
		 *   @throws		see Tensor<T>::Tensor(InputIt, InputIt, std::initializer_list<mint>)
		 *
		 *   @warning		It is user's responsibility to make sure that length of range fits into mint!
		 *   @note			Be aware that efficiency of this constructor strongly depends on whether the InputIt is also a RandomAccessIterator
		 **/
		template<class InputIt>
		Tensor(InputIt first, InputIt last);

		/**
		 *   @brief         Constructs the Tensor of given shape with all elements initialized to given value
		 *   @param[in]     init - value of type \b T to initialize all elements of the Tensor
		 *   @param[in]     dims - container with Tensor dimensions
		 *   @tparam		Container - any type of container that has member \b value_type and this type is convertible to mint
		 *   @throws		see Tensor<T>::createInternal() and MArray<T>::MArray(Container&&)
		 **/
		template<class Container, typename = typename std::enable_if<std::is_convertible<typename Container::value_type, mint>::value>::type>
		Tensor(T init, Container&& dims);

		/**
		 *   @brief         Constructs the Tensor of given shape with all elements initialized to given value
		 *   @param[in]     init - value of type \b T to initialize all elements of the Tensor
		 *   @param[in]     dims - initializer list with Tensor dimensions
		 *   @throws		see Tensor<T>::createInternal() and MArray<T>::MArray(std::initializer_list<mint>)
		 **/
		Tensor(T init, std::initializer_list<mint> dims);

		/**
		 *   @brief         Constructs the Tensor of given shape with elements from range [first, last)
		 *   @param[in]     first - iterator to the beginning of range
		 *   @param[in]		last - iterator past the end of range
		 *   @param[in]     dims - container with Tensor dimensions
		 *   @tparam		Container - any type of container that has member \b value_type and this type is convertible to mint
		 *   @throws		LLErrorCode::TensorNewError - if number of elements in \c v does not match total Tensor size indicated by \c dims
		 *   @throws		see Tensor<T>::createInternal() and MArray<T>::MArray(Container&&)
		 **/
		template<class InputIt, class Container, typename = typename std::enable_if<std::is_convertible<typename Container::value_type, mint>::value>::type>
		Tensor(InputIt first, InputIt last, Container&& dims);

		/**
		 *   @brief         Constructs the Tensor of given shape with elements from range [first, last)
		 *   @param[in]     first - iterator to the beginning of range
		 *   @param[in]		last - iterator past the end of range
		 *   @param[in]     dims - initializer list with Tensor dimensions
		 *   @throws		LLErrorCode::TensorNewError - if number of elements in \c v does not match total Tensor size indicated by \c dims
		 *   @throws		see Tensor<T>::createInternal() and MArray<T>::MArray(const std::vector<U>&)
		 **/
		template<class InputIt>
		Tensor(InputIt first, InputIt last, std::initializer_list<mint> dims);

		/**
		 *   @brief         Constructs Tensor based on MTensor
		 *   @param[in]     v - LibraryLink structure to be wrapped
		 *   @throws        LLErrorCode::TensorInitError - if WolframLibraryData structure is not initialized
		 *   @throws		LLErrorCode::TensorTypeError - if template parameter \b T does not match MTensor data type
		 **/
		Tensor(const MTensor v);

		/**
		 *   @brief         Copy constructor
		 *   @param[in]     t2 - const reference to a Tensor of matching type
		 **/
		Tensor(const Tensor<T>& t2);

		/**
		 *   @brief         Move constructor
		 *   @param[in]     t2 - rvalue reference to a Tensor of matching type
		 **/
		Tensor(Tensor<T>&& t2);

		/**
		 *   @brief         Copy-assignment operator
		 *   @param[in]     t2 - const reference to a Tensor of matching type
		 **/
		void operator=(const Tensor<T>& t2);

		/**
		 *   @brief         Move-assignment operator
		 *   @param[in]     t2 - rvalue reference to a Tensor of matching type
		 **/
		void operator=(Tensor<T>&& t2);

		/**
		 *   @brief	Free internal MTensor if necessary
		 **/
		virtual ~Tensor();

		/**
		 *   @brief	Return matching type of MTensor
		 **/
		unsigned char getType() const noexcept {
			return type;
		}

		/**
		 *   @brief 	Return internal MTensor. You should rarely need this function.
		 *   @warning 	Do not modify MTensor returned from this function!
		 **/
		MTensor getInternal() const noexcept {
			return internalMT;
		}

		/**
		 *   @brief 	Return share count of internal MTensor.
		 *   Use this to manually manage shared MTensors.
		 *
		 *   @see 		<http://reference.wolfram.com/language/LibraryLink/ref/callback/MTensor_shareCount.html>
		 *   @note		LibraryLink does not provide any way to check whether MTensor was passed or will be returned as "Shared".
		 *   Therefore passing or returning MTensors as "Shared" is discouraged and if you do that you are responsible for managing MTensor memory.
		 **/
		mint shareCount() const noexcept {
			if (internalMT == nullptr)
				return 0;
			return this->libData->MTensor_shareCount(internalMT);
		}

		/**
		 *   @brief 	Disown internal MTensor that is shared with Mathematica.
		 *   Use this to manually manage shared MTensors.
		 *
		 *   @see 		<http://reference.wolfram.com/language/LibraryLink/ref/callback/MTensor_disown.html>
		 *   @note		LibraryLink does not provide any way to check whether MTensor was passed or will be returned as "Shared".
		 *   Therefore passing or returning MTensors as "Shared" is discouraged and if you do that you are responsible for managing MTensor memory.
		 **/
		void disown() const noexcept {
			this->libData->MTensor_disown(internalMT);
		}

	private:
		/**
		 *   @brief Return raw pointer to underlying data
		 **/
		T* getData() const noexcept override;

		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::indexError()
		 *   @throws 	LibraryLinkError(LLErrorCode::TensorIndexError)
		 **/
		void indexError() const override {
			ErrorManager::throwException(LLErrorCode::TensorIndexError);
		}

		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::initError()
		 *   @throws 	LibraryLinkError(LLErrorCode::TensorInitError)
		 **/
		void initError() const override {
			ErrorManager::throwException(LLErrorCode::TensorInitError);
		}

		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::sizeError()
		 *   @throws 	LibraryLinkError(LLErrorCode::TensorSizeError)
		 **/
		void sizeError() const override {
			ErrorManager::throwException(LLErrorCode::TensorSizeError);
		}


		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::createInternal(). It creates a new flat MTensor.
		 *   @throws 	LibraryLinkError(LLErrorCode::TensorNewError)
		 *
		 *   @see 		<http://reference.wolfram.com/language/LibraryLink/ref/callback/MTensor_new.html>
		 *
		 **/
		void createInternal() override {
			if (this->libData->MTensor_new(type, this->depth, this->dimensionsData(), &internalMT))
				ErrorManager::throwException(LLErrorCode::TensorNewError);
		}

		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::freeInternal(). It frees internal MTensor.
		 *   @see 		<http://reference.wolfram.com/language/LibraryLink/ref/callback/MTensor_free.html>
		 **/
		void freeInternal() noexcept override {
			if (internalMT == nullptr)
				return;
			if (shareCount() > 0)
				disown();
			if (this->arrayOwnerQ)
				this->libData->MTensor_free(internalMT);
		}

		/**
		 *   @brief 	Sub-class implementation of virtual void MArray<T>::passInternal(MArgument&). Passes internal MTensor as result to LibraryLink.
		 **/
		void passInternal(MArgument& res) noexcept override {
			MArgument_setMTensor(res, internalMT);
		}

		/// MTensor type matching template parameter T
		static const unsigned char type;

		/// Internal container
		MTensor internalMT = nullptr;
	};

	template<typename T>
	template<typename Container>
	Tensor<T>::Tensor(Container&& v) :
			Tensor<T>(std::begin(v), std::end(v), { static_cast<mint>(v.size()) }) {
	}

	template<typename T>
	Tensor<T>::Tensor(std::initializer_list<T> v) :
			Tensor<T>(std::begin(v), std::end(v), { static_cast<mint>(v.size()) }) {
	}


	template<typename T>
	template<class InputIt>
	Tensor<T>::Tensor(InputIt first, InputIt last)  :
			Tensor<T>(first, last, { static_cast<mint>(std::distance(first, last)) }) {
	}

	template<typename T>
	template<class Container, typename>
	Tensor<T>::Tensor(T init, Container&& dims) : MArray<T>(std::forward<Container>(dims)) {
		createInternal();
		this->arrayOwnerQ = true;
		std::fill(this->begin(), this->end(), init);
	}

	template<typename T>
	Tensor<T>::Tensor(T init, std::initializer_list<mint> dims) : MArray<T>(dims) {
		createInternal();
		this->arrayOwnerQ = true;
		std::fill(this->begin(), this->end(), init);
	}


	template<typename T>
	template<class InputIt, class Container, typename>
	Tensor<T>::Tensor(InputIt first, InputIt last, Container&& dims) : MArray<T>(std::forward<Container>(dims)) {
		createInternal();
		if (std::distance(first, last) != this->flattenedLength)
			ErrorManager::throwException(LLErrorCode::TensorNewError, "Length of data range does not match specified dimensions");
		this->arrayOwnerQ = true;
		std::copy(first, last, this->begin());
	}

	template<typename T>
	template<class InputIt>
	Tensor<T>::Tensor(InputIt first, InputIt last, std::initializer_list<mint> dims) : MArray<T>(dims) {
		createInternal();
		if (std::distance(first, last) != this->flattenedLength)
			ErrorManager::throwException(LLErrorCode::TensorNewError, "Length of data range does not match specified dimensions");
		this->arrayOwnerQ = true;
		std::copy(first, last, this->begin());
	}

	template<typename T>
	Tensor<T>::Tensor(const MTensor t) {
		if (!this->libData)
			initError();
		if (type != this->libData->MTensor_getType(t))
			ErrorManager::throwException(LLErrorCode::TensorTypeError);
		this->arrayOwnerQ = false;
		internalMT = t;
		this->depth = this->libData->MTensor_getRank(internalMT);
		this->flattenedLength = this->libData->MTensor_getFlattenedLength(internalMT);
		const mint* rawDims = this->libData->MTensor_getDimensions(internalMT);
		this->dims.assign(rawDims, rawDims + this->rank());
		this->fillOffsets();
	}


	template<typename T>
	Tensor<T>::Tensor(const Tensor<T>& t2) : MArray<T>(t2) {
		if (this->libData->MTensor_clone(t2.internalMT, &this->internalMT)) {
			ErrorManager::throwException(LLErrorCode::TensorCloneError);
		}
		this->arrayOwnerQ = true;
	}

	template<typename T>
	Tensor<T>::Tensor(Tensor<T>&& t2) : MArray<T>(std::move(t2)) {
		this->internalMT = t2.internalMT;
		t2.internalMT = nullptr;
		t2.arrayOwnerQ = false;
	}


	template<typename T>
	void Tensor<T>::operator=(const Tensor<T>& t2) {
		MArray<T>::operator=(t2);
		this->freeInternal();
		if (this->libData->MTensor_clone(t2.internalMT, &this->internalMT)) {
			ErrorManager::throwException(LLErrorCode::TensorCloneError);
		}
		this->arrayOwnerQ = true;
	}

	template<typename T>
	void Tensor<T>::operator=(Tensor<T>&& t2) {
		MArray<T>::operator=(std::move(t2));
		this->freeInternal();
		this->internalMT = t2.internalMT;
		this->arrayOwnerQ = t2.arrayOwnerQ;
		t2.internalMT = nullptr;
		t2.arrayOwnerQ = false;
	}

	template<typename T>
	Tensor<T>::~Tensor() {
		this->freeInternal();
	}
} /* namespace LibraryLinkUtils */

#endif /* LLUTILS_TENSOR_H_ */
