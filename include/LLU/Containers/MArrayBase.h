/**
 * @file	MArrayBase.h
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	4/09/2017
 *
 * @brief	Base class for all C++ wrappers of LibraryLink containers
 *
 */

#ifndef LLUTILS_MARRAYBASE_H_
#define LLUTILS_MARRAYBASE_H_

#include "WolframLibrary.h"

#include <initializer_list>
#include <type_traits>
#include <vector>

#include "LLU/ErrorLog/ErrorManager.h"
#include "LLU/Utilities.hpp"

namespace LLU {

	/**
	 * @class MArrayBase
	 * @brief This is the base class for all LLU containers.
	 *
	 * It mostly carries meta-information about container's size and dimensions, as these are independent of the data type.
	 *
	 */
	class MArrayBase {
	public:

		/**
		 *	@brief Default constructor
		 **/
		MArrayBase() = default;

		/**
		 * 	@brief		Constructs uninitialized container with given dimensions
		 *	@param[in]	dims - list of MArray dimensions
		 *	@throws		LLErrorName::DimensionsError - if \c dims are invalid
		 *	@throws		LLErrorName::FunctionError - if any of Wolfram*Library structures was not initialized
		 **/
		MArrayBase(std::initializer_list<mint> dims);

		/**
		 * 	@brief		Constructs uninitialized container with given dimensions
		 *	@param[in]	dims - pointer to the memory where consecutive dimensions are stored
		 *	@param[in]	rank - length of the \p dims array
		 *	@throws		LLErrorName::DimensionsError - if \c dims are invalid
		 *	@throws		LLErrorName::FunctionError - if any of Wolfram*Library structures was not initialized
		 **/
		MArrayBase(const mint* dims, mint rank);

		/**
		 * 	@brief		Constructs uninitialized container with given dimensions
		 *	@param[in]	dims - container with MArray dimensions
		 *	@tparam		Container - any type of container that has member \b value_type and this type is convertible to mint
		 *	@throws		LLErrorName::DimensionsError - if \c dims are invalid
		 *	@throws		LLErrorName::FunctionError - if any of Wolfram*Library structures was not initialized
		 **/
		template<
			class Container,
			typename = disable_if_same_or_derived<MArrayBase, Container>,
			typename = typename std::enable_if_t<std::is_integral<typename std::remove_reference_t<Container>::value_type>::value>
		>
		explicit MArrayBase(Container&& dims);

		/**
		 *	@brief Get container rank
		 **/
		mint rank() const noexcept {
			return depth;
		}

		/**
		 *	@brief Get raw pointer to container dimensions
		 **/
		const mint* dimensionsData() const noexcept {
			return dims.data();
		}

		/**
		 *	@brief Get container dimensions in the form of const& to \b std::vector
		 **/
		const std::vector<mint>& dimensions() const noexcept {
			return dims;
		}

		/**
		 *	@brief 		Get single dimension
		 *	@param[in]	dim - index of desired dimension
		 *	@throws		indexError() - if \c dim is out-of-bounds
		 **/
		mint dimension(mint dim) const {
			if (dim >= rank() || dim < 0)
				indexError();
			return dims[static_cast<decltype(dims)::size_type>(dim)];
		}

		/**
		 *	@brief Get total number of elements in the container
		 **/
		mint size() const noexcept {
			return flattenedLength;
		}

		/**
		 *	@brief Check whether container is empty
		 **/
		mint empty() const noexcept {
			return flattenedLength == 0;
		}

		/**
		 *	@brief Default destructor
		 **/
		virtual ~MArrayBase() = default;

	protected:

		/// Total number of elements in the container
		mint flattenedLength = 0;

		/// Container rank
		mint depth = 0;

		/// Container dimensions
		std::vector<mint> dims;

		/// This helps to convert coordinates \f$ (x_1, \ldots, x_n) \f$ in multidimensional MArray to the corresponding index in a flat list of elements
		std::vector<mint> offsets;

		/// Populate \c offsets member
		void fillOffsets();

		/**
		 *	@brief 		Convert coordinates of an element in a multidimensional MArray to the corresponding index in a flat list of elements
		 *	@param[in]	indices - vector with coordinates of desired data element
		 **/
		mint getIndex(const std::vector<mint>& indices) const;

		/**
		 *	@brief 		Check if given coordinates are valid for this container
		 *	@param[in]	indices - vector with coordinates of desired data element
		 *	@throws		indexError() - if \c indices are out-of-bounds
		 **/
		void checkIndices(const std::vector<mint>& indices) const;

		/**
		 *   @brief 	Purely virtual method for throwing exception concerning index validity within the container
		 **/
		virtual void indexError() const = 0;

		/**
		 *   @brief 	Throw exception relating to container size
		 *   @throws 	LibraryLinkError(LLErrorName::DimensionsError)
		 **/
		virtual void sizeError() const {
			ErrorManager::throwException(ErrorName::DimensionsError);
		}

	private:

		/**
		 *	@brief 		Check if container size will fit into \b mint
		 *	@param[in]	v - a container
		 *	@throws		LLErrorName::DimensionsError - if \c v is too big
		 **/
		template<typename Container>
		mint checkContainerSize(Container&& v) const;

		/**
		 *	@brief 		Check if initializer list size will fit into \b mint
		 *	@param[in]	v - an initializer list
		 *	@throws		LLErrorName::DimensionsError - if \c v is too big
		 **/
		mint checkContainerSize(std::initializer_list<mint> v) const;

		/// Calculate total array length based on current value of dims
		mint totalLengthFromDims() const noexcept;
	};

	template<class Container, typename, typename>
	MArrayBase::MArrayBase(Container&& dimensions) {
		depth = checkContainerSize(std::forward<Container>(dimensions));
		auto dimsOk = std::all_of(std::begin(dimensions), std::end(dimensions), [](typename std::remove_reference_t<Container>::value_type d) {
			return (d > 0) && (d <= (std::numeric_limits<mint>::max)());
		});
		if (!dimsOk)
			ErrorManager::throwExceptionWithDebugInfo(ErrorName::DimensionsError, "Invalid input vector with array dimensions");
		dims.reserve(depth);
		std::copy(std::begin(dimensions), std::end(dimensions), std::back_inserter(dims));
		flattenedLength = totalLengthFromDims();
		fillOffsets();
	}

	template<class Container>
	mint MArrayBase::checkContainerSize(Container&& v) const {
		if (v.size() > (std::numeric_limits<mint>::max)())
			sizeError();
		return static_cast<mint>(v.size());
	}

} /* namespace LLU */

#endif /* LLUTILS_MARRAYBASE_H_ */
