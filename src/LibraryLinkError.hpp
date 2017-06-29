/**
 * @file	LibraryLinkError.hpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	20/04/2017
 *
 * @brief	Error class and error codes used by LibraryLink Utilities classes
 *
 */
#ifndef LLUTILS_LIBRARYLINKERROR_H_
#define LLUTILS_LIBRARYLINKERROR_H_

#include <stdexcept>
#include <string>

namespace LibraryLinkUtils {

	/// Error codes used in LibraryLink Utilities
	enum class LLErrorCode : short {

		// Original LibraryLink error codes:
		VersionError = 7,	///< same as LIBRARY_VERSION_ERROR
		FunctionError = 6,	///< same as LIBRARY_FUNCTION_ERROR
		MemoryError = 5,	///< same as LIBRARY_MEMORY_ERROR
		NumericalError = 4,	///< same as LIBRARY_NUMERICAL_ERROR
		DimensionsError = 3,///< same as LIBRARY_DIMENSIONS_ERROR
		RankError = 2,		///< same as LIBRARY_RANK_ERROR
		TypeError = 1,		///< same as LIBRARY_TYPE_ERROR
		NoError = 0,		///< same as LIBRARY_NO_ERROR

		// Reserved for use in paclets:
		// -1
		// ...
		// -100

		// MArgument errors: [-101 : -200]
		MArgumentInitError = -101,		///< MArgumentManager construction failed
		MArgumentIndexError = -102,		///< wrong argument index
		MArgumentRawArrayError = -103,	///< error involving RawArray argument
		MArgumentTensorError = -104,	///< error involving Tensor argument
		MArgumentImageError = -105,		///< error involving Image argument

		// RawArray errors: [-201 : -300]
		RawArrayInitError = -201,	///< construction of RawArray failed
		RawArrayNewError = -202,	///< creating new MRawArray failed
		RawArrayCloneError = -203,	///< MRawArray cloning failed
		RawArrayTypeError = -204,	///< MRawArray type mismatch
		RawArraySizeError = -205,	///< wrong assumption about RawArray size
		RawArrayIndexError = -206,	///< trying to access non-existing element

		// MTensor errors: [-301 : -400]
		TensorInitError = -301,		///< Tensor construction failed
		TensorNewError = -302,		///< creating new MTensor failed
		TensorCloneError = -303,	///< MTensor cloning failed
		TensorTypeError = -304,		///< Tensor type mismatch
		TensorSizeError = -305,		///< wrong assumption about Tensor size
		TensorIndexError = -306,	///< trying to access non-existing element

		// MImage errors: [-401 : -500]
		ImageInitError = -401,		///< Image construction failed
		ImageNewError = -402,		///< creating new MImage failed
		ImageCloneError = -403,		///< MImage cloning failed
		ImageTypeError = -404,		///< Image type mismatch
		ImageSizeError = -405,		///< wrong assumption about Image size
		ImageIndexError = -406,		///< trying to access non-existing element

	};

	/**
	 * @class	LibraryLinkError
	 * @brief	This is a class template, where template parameter is an enumerated type containing (possibly user-defined) error codes.
	 * @tparam	Enum - enumerated type with error codes
	 * @note	Within LibraryLink Utilities \b Enum class will always be LLErrorCode
	 **/
	template<class Enum>
	class LibraryLinkError: public std::runtime_error {
	public:
		/**
		 *   @brief         Constructs an exception with given error code and predefined error message
		 *   @param[in]     which - error code of template type \b Enum
		 *   @warning		Default error message assumes that the value of error code will fit into \b int
		 **/
		LibraryLinkError(Enum which) :
				LibraryLinkError(which, "Error " + std::to_string(static_cast<int>(which)) + " (see paclet documentation for details)") {

		}

		/**
		 *   @brief         Constructs an exception with given error code and given error message
		 *   @param[in]     which - error code of template type \b Enum
		 *   @param[in]		errMsg - short explanation why the error occurred
		 **/
		LibraryLinkError(Enum which, const std::string& errMsg) :
				std::runtime_error(errMsg), err(which) {

		}

		/**
		 *   @brief Get the value of error code
		 **/
		Enum which() const noexcept {
			return err;
		}

	private:
		/// Error code
		Enum err;
	};

} /* namespace LibraryLinkUtils */

#endif /* LLUTILS_LIBRARYLINKERROR_H_ */
