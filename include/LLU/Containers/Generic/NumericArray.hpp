/**
 * @file
 * @brief
 */
#ifndef LLU_INCLUDE_LLU_CONTAINERS_GENERIC_NUMERICARRAY
#define LLU_INCLUDE_LLU_CONTAINERS_GENERIC_NUMERICARRAY

#include "LLU/Containers/Generic/Base.hpp"

namespace LLU {

	template<class PassingMode>
	class MContainer<MArgumentType::NumericArray, PassingMode>;

	/// MContainer specialization for MNumericArray is called GenericNumericArray
	template<class PassingMode>
	using GenericNumericArray = MContainer<MArgumentType::NumericArray, PassingMode>;

	/**
	 *  @brief Abstract class that defines a basic set of operations on a numeric array
	 */
	struct NumericArrayInterface {
		/**
		 * @brief   Get rank
		 * @return  number of dimensions in the array
		 * @see     <http://reference.wolfram.com/language/LibraryLink/ref/callback/MNumericArray_getRank.html>
		 */
		virtual mint getRank() const = 0;

		/**
		 * @brief   Get dimensions
		 * @return  raw pointer to dimensions of the array
		 * @see     <http://reference.wolfram.com/language/LibraryLink/ref/callback/MNumericArray_getDimensions.html>
		 */
		virtual mint const* getDimensions() const = 0;

		/**
		 * @brief   Get length
		 * @return  total number of elements
		 * @see     <http://reference.wolfram.com/language/LibraryLink/ref/callback/MNumericArray_getFlattenedLength.html>
		 */
		virtual mint getFlattenedLength() const = 0;

		/**
		 * @brief   Get the data type of this array
		 * @return  type of elements (see definition of \c numericarray_data_t)
		 * @see 	<http://reference.wolfram.com/language/LibraryLink/ref/callback/MNumericArray_getDataType.html>
		 */
		virtual numericarray_data_t type() const = 0;

		/**
		 * @brief   Get access to the raw data. Use with caution.
		 * @return  pointer to the raw data
		 * @see     <http://reference.wolfram.com/language/LibraryLink/ref/callback/MNumericArray_getData.html>
		 */
		virtual void* rawData() const = 0;
	};

	/**
	 * @brief   Simple, light-weight, non-owning wrappper over MNumericArray.
	 *
	 * Intended for use in functions that only need to access MNumericArray metadata, where it can alleviate the need for introducing template parameters
	 * for MNumericArray passing mode (like in GenericNumericArray) or data type (like in NumericArray class).
	 */
	class NumericArrayView : public NumericArrayInterface {
	public:
		NumericArrayView() = default;

		template<class Passing>
		/* implicit */ NumericArrayView(const GenericNumericArray<Passing>& gNA) : na {gNA.getContainer()} {}

		/// @copydoc NumericArrayInterface::getRank()
		mint getRank() const override {
			return LibraryData::NumericArrayAPI()->MNumericArray_getRank(na);
		}

		/// @copydoc NumericArrayInterface::getDimensions()
		mint const* getDimensions() const override {
			return LibraryData::NumericArrayAPI()->MNumericArray_getDimensions(na);
		}

		/// @copydoc NumericArrayInterface::getFlattenedLength()
		mint getFlattenedLength() const override {
			return LibraryData::NumericArrayAPI()->MNumericArray_getFlattenedLength(na);
		}

		/// @copydoc NumericArrayInterface::type()
		numericarray_data_t type() const override {
			return LibraryData::NumericArrayAPI()->MNumericArray_getType(na);
		}

		/// @copydoc NumericArrayInterface::rawData()
		void* rawData() const noexcept override {
			return LibraryData::NumericArrayAPI()->MNumericArray_getData(na);
		}
	private:
		MNumericArray na = nullptr;
	};

	/**
	 *  @brief  MContainer specialization for MNumericArray
	 *  @tparam PassingMode - passing policy
	 */
	template<class PassingMode>
	class MContainer<MArgumentType::NumericArray, PassingMode> : public NumericArrayInterface, public MContainerBase<MArgumentType::NumericArray, PassingMode> {
	public:
		/// Inherit constructors from MContainerBase
		using MContainerBase<MArgumentType::NumericArray, PassingMode>::MContainerBase;

		/**
		 * @brief   Default constructor, the MContainer does not manage any instance of MNumericArray.
		 */
		MContainer() = default;

		/**
		 * @brief   Create GenericNumericArray of given type and shape
		 * @param   type - new GenericNumericArray type
		 * @param   rank - new GenericNumericArray rank
		 * @param   dims - new GenericNumericArray dimensions
		 * @see     <http://reference.wolfram.com/language/LibraryLink/ref/callback/MNumericArray_new.html>
		 */
		MContainer(numericarray_data_t type, mint rank, const mint* dims) {
			RawContainer tmp {};
			if (LibraryData::NumericArrayAPI()->MNumericArray_new(type, rank, dims, &tmp)) {
				ErrorManager::throwException(ErrorName::NumericArrayNewError);
			}
			this->setContainer(tmp);
		}

		/**
		 * @brief   Create GenericNumericArray from another GenericNumericArray with different passing mode.
		 * @tparam  P - some passing mode
		 * @param   mc - different GenericNumericArray
		 */
		template<class P>
		explicit MContainer(const MContainer<MArgumentType::NumericArray, P>& mc) : Base(mc) {}

		MContainer(const MContainer& mc) = default;

		MContainer(MContainer&& mc) noexcept = default;

		MContainer& operator=(const MContainer&) = default;

		MContainer& operator=(MContainer&&) noexcept = default;

		/**
		 * @brief   Assign a GenericNumericArray with different passing mode.
		 * @tparam  P - some passing mode
		 * @param   mc - different GenericNumericArray
		 * @return  this
		 */
		template<class P>
		MContainer& operator=(const MContainer<MArgumentType::NumericArray, P>& mc) {
			Base::operator=(mc);
			return *this;
		}

		/// Destructor which triggers the appropriate cleanup action which depends on the PassingMode
		~MContainer() override {
			this->cleanup();
		};

		/**
		 * @brief   Convert this object to a new GenericNumericArray of given datatype, using specified conversion method
		 * @param   t - destination data type
		 * @param   method - conversion method
		 * @param   param - conversion method parameter (aka tolerance)
		 * @return  converted GenericNumericArray with Manual passing mode
		 * @see     <http://reference.wolfram.com/language/LibraryLink/ref/callback/MNumericArray_convertType.html>
		 */
		GenericNumericArray<Passing::Manual> convert(numericarray_data_t t, NA::ConversionMethod method, double param) const {
			MNumericArray newNA = nullptr;
			auto err = LibraryData::NumericArrayAPI()->MNumericArray_convertType(&newNA, this->getContainer(), t,
																				 static_cast<numericarray_convert_method_t>(method), param);
			if (err) {
				ErrorManager::throwException(ErrorName::NumericArrayConversionError, "Conversion to type " + std::to_string(static_cast<int>(t)) + " failed.");
			}
			return newNA;
		}

		/// @copydoc NumericArrayInterface::getRank()
		mint getRank() const override {
			return LibraryData::NumericArrayAPI()->MNumericArray_getRank(this->getContainer());
		}

		/// @copydoc NumericArrayInterface::getDimensions()
		mint const* getDimensions() const override {
			return LibraryData::NumericArrayAPI()->MNumericArray_getDimensions(this->getContainer());
		}

		/// @copydoc NumericArrayInterface::getFlattenedLength()
		mint getFlattenedLength() const override {
			return LibraryData::NumericArrayAPI()->MNumericArray_getFlattenedLength(this->getContainer());
		}

		/// @copydoc NumericArrayInterface::type()
		numericarray_data_t type() const override {
			return LibraryData::NumericArrayAPI()->MNumericArray_getType(this->getContainer());
		}

		/// @copydoc NumericArrayInterface::rawData()
		void* rawData() const noexcept override {
			return LibraryData::NumericArrayAPI()->MNumericArray_getData(this->getContainer());
		}

	private:
		using Base = MContainerBase<MArgumentType::NumericArray, PassingMode>;
		using RawContainer = typename Base::Container;

		/**
		 *   @copydoc   MContainerBase::clone()
		 *   @see 		<http://reference.wolfram.com/language/LibraryLink/ref/callback/MNumericArray_clone.html>
		 **/
		RawContainer cloneImpl() const override {
			RawContainer tmp {};
			if (LibraryData::NumericArrayAPI()->MNumericArray_clone(this->getContainer(), &tmp)) {
				ErrorManager::throwException(ErrorName::NumericArrayCloneError);
			}
			return tmp;
		}

		/**
		 * @copydoc MContainerBase::shareCount()
		 * @see 	<http://reference.wolfram.com/language/LibraryLink/ref/callback/MNumericArray_shareCount.html>
		 */
		mint shareCountImpl() const noexcept override {
			return LibraryData::NumericArrayAPI()->MNumericArray_shareCount(this->getContainer());
		}

		/**
		 *   @copydoc   MContainerBase::disown()
		 *   @see 		<http://reference.wolfram.com/language/LibraryLink/ref/callback/MNumericArray_disown.html>
		 **/
		void disownImpl() const noexcept override {
			LibraryData::NumericArrayAPI()->MNumericArray_disown(this->getContainer());
		}

		/**
		 *   @copydoc   MContainerBase::free()
		 *   @see 		<http://reference.wolfram.com/language/LibraryLink/ref/callback/MNumericArray_free.html>
		 **/
		void freeImpl() const noexcept override {
			LibraryData::NumericArrayAPI()->MNumericArray_free(this->getContainer());
		}

		/**
		 *   @copydoc   MContainerBase::pass
		 **/
		void passImpl(MArgument& res) const noexcept override {
			MArgument_setMNumericArray(res, this->getContainer());
		}
	};

}

#endif	  // LLU_INCLUDE_LLU_CONTAINERS_GENERIC_NUMERICARRAY
