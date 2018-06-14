/**
 * @file	MArgumentManager.h
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	6/07/2017
 *
 * @brief	Declaration of MArgumentManager class
 *
 */

#ifndef LLUTILS_MARGUMENTMANAGER_H_
#define LLUTILS_MARGUMENTMANAGER_H_

#include "WolframLibrary.h"

#include <complex>
#include <cstdint>
#include <limits>
#include <memory>
#include <string>
#include <vector>

#include "Image.h"
#include "LibDataHolder.h"
#include "LibraryLinkError.h"
#include "RawArray.h"
#include "Tensor.h"

namespace LibraryLinkUtils {

	/**
	 * @class	MArgumentManager
	 * @brief	Manages arguments exchanged between the paclet C++ code and LibraryLink interface.
	 *
	 * MArgumentManager provides a safe way to access MArguments received from LibraryLink and takes care of memory management both for in- and out- arguments.
	 * Using MArgumentManager one can perform generic operations on RawArrays, Tensors and Images independent of their data type.
	 *
	 * @todo	Make sure all MArgument passing modes ("Constant", "Shared", etc.) are correctly handled
	 **/
	class MArgumentManager : public LibDataHolder {
	public:

		/**
		 *   @brief         Constructor
		 *   @param[in]     Argc - number of MArguments provided
		 *   @param[in]     Args - MArguments provided
		 *   @param[in]		Res - reference to output MArgument
		 *   @throws        LLErrorName::MArgumentInitError - if static member libData is not initialized
		 **/
		MArgumentManager(mint Argc, MArgument* Args, MArgument& Res);

		/**
		 *   @brief         Constructor
		 *   @param[in]     ld - library data
		 *   @param[in]     Argc - number of MArguments provided
		 *   @param[in]     Args - MArguments provided
		 *   @param[in]		Res - reference to output MArgument
		 **/
		MArgumentManager(WolframLibraryData ld, mint Argc, MArgument* Args, MArgument& Res);

		/**
		 *   @brief Default destructor
		 **/
		virtual ~MArgumentManager() = default;

		/**
		 *   @brief         Get MArgument of type \b mbool at position \c index
		 *   @param[in]     index - position of desired MArgument in \c Args
		 *   @returns       MArgument of type \b bool at position \c index
		 *   @throws        LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 **/
		bool getBoolean(unsigned int index) const;

		/**
		 *   @brief         Set \c result as output MArgument
		 *   @param[in]     result - boolean value to be returned to LibraryLink
		 **/
		void setBoolean(bool result) const noexcept;

		/**
		 *   @brief         Get MArgument of type \b mreal at position \c index
		 *   @param[in]     index - position of desired MArgument in \c Args
		 *   @returns       MArgument of type \b double at position \c index
		 *   @throws        LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 **/
		double getReal(unsigned int index) const;

		/**
		 *   @brief         Set \c result as output MArgument
		 *   @param[in]     result - value of type \b double to be returned to LibraryLink
		 **/
		void setReal(double result) const noexcept;

		/**
		 *   @brief         Get MArgument of type \b mint at position \c index
		 *   @tparam		T - integral type to convert \b mint to
		 *   @param[in]     index - position of desired MArgument in \c Args
		 *   @returns       MArgument value at position \c index converted to \b T
		 *   @throws        LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 **/
		template<typename T>
		T getInteger(unsigned int index) const;

		/**
		 *   @brief         Set \c result as output MArgument
		 *   @param[in]     result - value of type \b mint to be returned to LibraryLink
		 *   @warning		\c result will be implicitly casted to \b mint with no overflow check
		 **/
		void setInteger(mint result) const noexcept;

		/**
		 *   @brief         Set \c result as output MArgument and check for overflow
		 *   @tparam		T - integral type to be casted to \b mint
		 *   @param[in]     result - value to be returned to LibraryLink
		 **/
		template<typename T>
		bool setMintAndCheck(T result) const noexcept;

		/**
		 *   @brief         Get MArgument of type \b mcomplex at position \c index
		 *   @param[in]     index - position of desired MArgument in \c Args
		 *   @returns       MArgument value at position \c index converted to \b std::complex<double>
		 *   @throws        LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 **/
		std::complex<double> getComplex(unsigned int index) const;

		/**
		 *   @brief         Set \c c as output MArgument
		 *   @param[in]     c - value of type \b std::complex<double> to be returned to LibraryLink
		 **/
		void setComplex(std::complex<double> c) const noexcept;

		/**
		 *   @brief         Get reference to MArgument of type \b "UTF8String" at position \c index
		 *   @param[in]     index - position of desired MArgument in \c Args
		 *   @returns       Reference to \b std::string which is copied from MArgument at position \c index, string itself is stored in MArgumentManager
		 *   @throws        LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 *
		 *   @warning		MArgument is copied to \b std::string only once so if you use the reference to modify the string you will not be able to get the original value back
		 **/
		std::string& getString(unsigned int index);

		/**
		 *   @brief         Set \c str as output MArgument
		 *   @param[in]     str - reference to \b std::string to be returned to LibraryLink
		 **/
		void setString(const std::string& str);

		/**
		 *   @overload
		 **/
		void setString(const char* str);

		/**
		 *   @brief         Set \c str as output MArgument
		 *   @param[in]     str - r-value reference to \b std::string to be returned to LibraryLink
		 **/
		void setString(std::string&& str);

		/**
		 *   @brief         Get MArgument of type MRawArray at position \c index
		 *   @tparam		T - type of data stored in RawArray
		 *   @param[in]     index - position of desired MArgument in \c Args
		 *   @returns       RawArray wrapper of MArgument at position \c index
		 *   @throws        LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 *   @see			RawArray<T>::RawArray(const MRawArray);
		 **/
		template<typename T>
		RawArray<T> getRawArray(unsigned int index) const;

		/**
		 *   @brief         Set MRawArray wrapped by \c ra as output MArgument
		 *   @tparam		T - RawArray data type
		 *   @param[in]     ra - reference to RawArray which should pass its internal MRawArray to LibraryLink
		 **/
		template<typename T>
		void setRawArray(RawArray<T>& ra);

		/**
		 *   @brief         Get type of MRawArray at position \c index in \c Args
		 *   @param[in]     index - position of desired MArgument in \c Args
		 *   @returns       MRawArray type
		 *   @throws        LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 **/
		rawarray_t getRawArrayType(unsigned int index) const;

		/**
		 *   @brief         Perform operation on RawArray argument at position \c index in \c Args
		 *   @tparam		Operator - any callable class
		 *   @tparam		OpArgs... - types of arguments of \c operator() in class \c Operator
		 *   @param[in]     index - position of MRawArray in \c Args
		 *   @param[in]     args - arguments of Operator::operator()
		 *   @throws        LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 *   @throws        LLErrorName::MArgumentRawArrayError - if MRawArray argument has incorrect type
		 *   @warning		Operator::operator() has to be a template that takes a const RawArray<T>& as first argument
		 **/
		template<class Operator, class ... OpArgs>
		void operateOnRawArray(unsigned int index, OpArgs&&... args);

		/**
		 *   @brief         Perform operation on RawArray argument at position \c index in \c Args
		 *   @tparam		Operator - any callable class
		 *   @param[in]     index - position of MRawArray in \c Args
		 *   @param[in]     op - callable object (possibly lambda) that takes only one argument - a RawArray
		 *   @throws        LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 *   @throws        LLErrorName::MArgumentRawArrayError - if MRawArray argument has incorrect type
		 **/
		template<class Operator>
		void operateOnRawArray(unsigned int index, Operator&& op);

		/**
		 *   @brief         Get MArgument of type MTensor at position \c index
		 *   @tparam		T - type of data stored in Tensor
		 *   @param[in]     index - position of desired MArgument in \c Args
		 *   @returns       Tensor wrapper of MArgument at position \c index
		 *   @throws        LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 *   @see			Tensor<T>::Tensor(const MTensor);
		 **/
		template<typename T>
		Tensor<T> getTensor(unsigned int index) const;

		/**
		 *   @brief         Set MTensor wrapped by \c ten as output MArgument
		 *   @tparam		T - Tensor data type
		 *   @param[in]     ten - reference to Tensor which should pass its internal MTensor to LibraryLink
		 **/
		template<typename T>
		void setTensor(Tensor<T>& ten);

		/**
		 *   @brief         Get type of MTensor at position \c index in \c Args
		 *   @param[in]     index - position of desired MArgument in \c Args
		 *   @returns       MTensor type
		 *   @throws        LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 **/
		unsigned char getTensorType(unsigned int index) const;

		/**
		 *   @brief         Perform operation on Tensor argument at position \c index in \c Args
		 *   @tparam		Operator - any callable class
		 *   @tparam		OpArgs... - types of arguments of \c operator() in class \c Operator
		 *   @param[in]     index - position of MTensor in \c Args
		 *   @param[in]     args - arguments of Operator::operator()
		 *   @throws        LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 *   @throws        LLErrorName::MArgumentTensorError - if MTensor argument has incorrect type
		 *   @warning		Operator::operator() has to be a template that takes a const Tensor<T>& as first argument
		 **/
		template<class Operator, class ... Args>
		void operateOnTensor(unsigned int index, Args&&... args);

		/**
		 *   @brief         Perform operation on Tensor argument at position \c index in \c Args
		 *   @tparam		Operator - any callable class
		 *   @param[in]     index - position of MTensor in \c Args
		 *   @param[in]     op - callable object (possibly lambda) that takes only one argument - a Tensor
		 *   @throws        LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 *   @throws        LLErrorName::MArgumentTensorError - if MTensor argument has incorrect type
		 **/
		template<class Operator>
		void operateOnTensor(unsigned int index, Operator&& op);

		/**
		 *   @brief         Get MArgument of type MImage at position \c index
		 *   @tparam		T - type of data stored in Image
		 *   @param[in]     index - position of desired MArgument in \c Args
		 *   @returns       Image wrapper of MArgument at position \c index
		 *   @throws        LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 *   @see			Image<T>::Image(const MImage ra);
		 **/
		template<typename T>
		Image<T> getImage(unsigned int index) const;

		/**
		 *   @brief         Set MImage wrapped by \c mi as output MArgument
		 *   @tparam		T - Image data type
		 *   @param[in]     mi - reference to Image which should pass its internal MImage to LibraryLink
		 **/
		template<typename T>
		void setImage(Image<T>& mi);

		/**
		 *   @brief         Get type of MImage at position \c index in \c Args
		 *   @param[in]     index - position of desired MArgument in \c Args
		 *   @returns       MImage type
		 *   @throws        LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 **/
		imagedata_t getImageType(unsigned int index) const;

		/**
		 *   @brief         Perform operation on Image argument at position \c index in \c Args
		 *   @tparam		Operator - any callable class
		 *   @tparam		OpArgs... - types of arguments of \c operator() in class \c Operator
		 *   @param[in]     index - position of MImage in \c Args
		 *   @param[in]     args - arguments of Operator::operator()
		 *   @throws        LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 *   @throws        LLErrorName::MArgumentImageError - if MImage argument has incorrect type
		 *   @warning		Operator::operator() has to be a template that takes a const Image<T>& as first argument
		 **/
		template<class Operator, class ... Args>
		void operateOnImage(unsigned int index, Args&&... args);

		/**
		 *   @brief         Perform operation on Image argument at position \c index in \c Args
		 *   @tparam		Operator - any callable class
		 *   @param[in]     index - position of MImage in \c Args
		 *   @param[in]     op - callable object (possibly lambda) that takes only one argument - an Image
		 *   @throws        LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 *   @throws        LLErrorName::MArgumentImageError - if MImage argument has incorrect type
		 **/
		template<class Operator>
		void operateOnImage(unsigned int index, Operator&& op);

	private:
		/**
		 *   @brief			Get MArgument at position \c index
		 *   @param[in]		index - position of desired MArgument in \c Args
		 *   @throws		LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 **/
		MArgument getArgs(unsigned int index) const;

		/// Here we store a string that was most recently returned to LibraryLink
		/// [LLDocs]: https://reference.wolfram.com/language/LibraryLink/tutorial/InteractionWithMathematica.html#262826223 "LibraryLink docs"
		/// @see [LibraryLink docs][LLDocs]
		static std::string stringResultBuffer;

		/// Max \b mint value
		static constexpr mint MINT_MAX = (std::numeric_limits<mint>::max)();

		/// Min \b mint value
		static constexpr mint MINT_MIN = (std::numeric_limits<mint>::min)();

		/// Number of input arguments expected from LibraryLink
		mint argc;

		/// "Array" of input arguments from LibraryLink
		MArgument *args;

		/// Output argument for LibraryLink
		MArgument& res;

		/// Structure to manage string arguments received from LibraryLink
		/// [LLDocs]: https://reference.wolfram.com/language/LibraryLink/tutorial/InteractionWithMathematica.html#262826223 "LibraryLink docs"
		/// @see [LibraryLink docs][LLDocs]
		std::vector<std::unique_ptr<std::string>> stringArgs;
	};

	template<typename T>
	T MArgumentManager::getInteger(unsigned int index) const {
		return static_cast<T>(MArgument_getInteger(getArgs(index)));
	}

	template<typename T>
	bool MArgumentManager::setMintAndCheck(T result) const noexcept {
		if (result >= MINT_MAX) {
			setInteger(MINT_MAX);
			return true;
		}
		else
			if (result <= MINT_MIN) {
				setInteger(MINT_MIN);
				return true;
			}
			else {
				setInteger(result);
				return false;
			}
	}

	template<typename T>
	RawArray<T> MArgumentManager::getRawArray(unsigned int index) const {
		return RawArray<T>(MArgument_getMRawArray(getArgs(index)));
	}

	template<typename T>
	void MArgumentManager::setRawArray(RawArray<T>& ra) {
		ra.passAsResult(res);
	}

	template<class Operator, class ... Args>
	void MArgumentManager::operateOnRawArray(unsigned int index,  Args&&... args) {
		Operator op;
		switch (getRawArrayType(index)) {
			case MRawArray_Type_Bit8:
				op(this->getRawArray<int8_t>(index), std::forward<Args>(args)...);
				break;
			case MRawArray_Type_Ubit8:
				op(this->getRawArray<uint8_t>(index), std::forward<Args>(args)...);
				break;
			case MRawArray_Type_Bit16:
				op(this->getRawArray<int16_t>(index), std::forward<Args>(args)...);
				break;
			case MRawArray_Type_Ubit16:
				op(this->getRawArray<uint16_t>(index), std::forward<Args>(args)...);
				break;
			case MRawArray_Type_Bit32:
				op(this->getRawArray<int32_t>(index), std::forward<Args>(args)...);
				break;
			case MRawArray_Type_Ubit32:
				op(this->getRawArray<uint32_t>(index), std::forward<Args>(args)...);
				break;
			case MRawArray_Type_Bit64:
				op(this->getRawArray<int64_t>(index), std::forward<Args>(args)...);
				break;
			case MRawArray_Type_Ubit64:
				op(this->getRawArray<uint64_t>(index), std::forward<Args>(args)...);
				break;
			case MRawArray_Type_Real32:
				op(this->getRawArray<float>(index), std::forward<Args>(args)...);
				break;
			case MRawArray_Type_Real64:
				op(this->getRawArray<double>(index), std::forward<Args>(args)...);
				break;
			case MRawArray_Type_Float_Complex:
				op(this->getRawArray<std::complex<float>>(index), std::forward<Args>(args)...);
				break;
			case MRawArray_Type_Double_Complex:
				op(this->getRawArray<std::complex<double>>(index), std::forward<Args>(args)...);
				break;
			default:
				ErrorManager::throwException(LLErrorName::MArgumentRawArrayError, "Incorrect type of RawArray argument. Argument index: " + std::to_string(index));
		}
	}

	template<class Operator>
	void MArgumentManager::operateOnRawArray(unsigned int index, Operator&& op) {
		switch (getRawArrayType(index)) {
			case MRawArray_Type_Bit8:
				op(this->getRawArray<int8_t>(index));
				break;
			case MRawArray_Type_Ubit8:
				op(this->getRawArray<uint8_t>(index));
				break;
			case MRawArray_Type_Bit16:
				op(this->getRawArray<int16_t>(index));
				break;
			case MRawArray_Type_Ubit16:
				op(this->getRawArray<uint16_t>(index));
				break;
			case MRawArray_Type_Bit32:
				op(this->getRawArray<int32_t>(index));
				break;
			case MRawArray_Type_Ubit32:
				op(this->getRawArray<uint32_t>(index));
				break;
			case MRawArray_Type_Bit64:
				op(this->getRawArray<int64_t>(index));
				break;
			case MRawArray_Type_Ubit64:
				op(this->getRawArray<uint64_t>(index));
				break;
			case MRawArray_Type_Real32:
				op(this->getRawArray<float>(index));
				break;
			case MRawArray_Type_Real64:
				op(this->getRawArray<double>(index));
				break;
			case MRawArray_Type_Float_Complex:
				op(this->getRawArray<std::complex<float>>(index));
				break;
			case MRawArray_Type_Double_Complex:
				op(this->getRawArray<std::complex<double>>(index));
				break;
			default:
				ErrorManager::throwException(LLErrorName::MArgumentRawArrayError, "Incorrect type of RawArray argument. Argument index: " + std::to_string(index));
		}
	}

	template<typename T>
	Tensor<T> MArgumentManager::getTensor(unsigned int index) const {
		return Tensor<T>(MArgument_getMTensor(getArgs(index)));
	}

	template<typename T>
	void MArgumentManager::setTensor(Tensor<T>& ten) {
		ten.passAsResult(res);
	}


	template<class Operator, class... Args>
	void MArgumentManager::operateOnTensor(unsigned int index, Args&&... args) {
		Operator op;
		switch (getTensorType(index)) {
			case MType_Integer:
				op(this->getTensor<mint>(index), std::forward<Args>(args)...);
				break;
			case MType_Real:
				op(this->getTensor<double>(index), std::forward<Args>(args)...);
				break;
			case MType_Complex:
				op(this->getTensor<std::complex<double>>(index), std::forward<Args>(args)...);
				break;
			default:
				ErrorManager::throwException(LLErrorName::MArgumentTensorError, "Incorrect type of Tensor argument. Argument index: " + std::to_string(index));
		}
	}


	template<class Operator>
	void MArgumentManager::operateOnTensor(unsigned int index, Operator&& op) {
		switch (getTensorType(index)) {
			case MType_Integer:
				op(this->getTensor<mint>(index));
				break;
			case MType_Real:
				op(this->getTensor<double>(index));
				break;
			case MType_Complex:
				op(this->getTensor<std::complex<double>>(index));
				break;
			default:
				ErrorManager::throwException(LLErrorName::MArgumentTensorError, "Incorrect type of Tensor argument. Argument index: " + std::to_string(index));
		}
	}


	template<typename T>
	Image<T> MArgumentManager::getImage(unsigned int index) const {
		return Image<T>(MArgument_getMImage(getArgs(index)));
	}

	template<typename T>
	void MArgumentManager::setImage(Image<T>& mi) {
		mi.passAsResult(res);
	}

	template<class Operator, class... Args>
	void MArgumentManager::operateOnImage(unsigned int index, Args&&... args) {
		Operator op;
		switch (getImageType(index)) {
			case MImage_Type_Bit:
				op(this->getImage<int8_t>(index), std::forward<Args>(args)...);
				break;
			case MImage_Type_Bit8:
				op(this->getImage<uint8_t>(index), std::forward<Args>(args)...);
				break;
			case MImage_Type_Bit16:
				op(this->getImage<uint16_t>(index), std::forward<Args>(args)...);
				break;
			case MImage_Type_Real32:
				op(this->getImage<float>(index), std::forward<Args>(args)...);
				break;
			case MImage_Type_Real:
				op(this->getImage<double>(index), std::forward<Args>(args)...);
				break;
			default:
				ErrorManager::throwException(LLErrorName::MArgumentImageError, "Incorrect type of Image argument. Argument index: " + std::to_string(index));
		}
	}

	template<class Operator>
	void MArgumentManager::operateOnImage(unsigned int index, Operator&& op) {
		switch (getImageType(index)) {
			case MImage_Type_Bit:
				op(this->getImage<int8_t>(index));
				break;
			case MImage_Type_Bit8:
				op(this->getImage<uint8_t>(index));
				break;
			case MImage_Type_Bit16:
				op(this->getImage<uint16_t>(index));
				break;
			case MImage_Type_Real32:
				op(this->getImage<float>(index));
				break;
			case MImage_Type_Real:
				op(this->getImage<double>(index));
				break;
			default:
				ErrorManager::throwException(LLErrorName::MArgumentImageError, "Incorrect type of Image argument. Argument index: " + std::to_string(index));
		}
	}

} /* namespace LibraryLinkUtils */


#endif /* LLUTILS_MARGUMENTMANAGER_H_ */
