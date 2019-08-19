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

	template<class PassingMode>
	using GenericNumericArray = MContainer<MArgumentType::NumericArray, PassingMode>;


	template<class PassingMode>
	class MContainer<MArgumentType::NumericArray, PassingMode> : public MContainerBase<MArgumentType::NumericArray, PassingMode> {
	public:
		using Base = MContainerBase<MArgumentType::NumericArray, PassingMode>;
		using RawContainer = typename Base::Container;

		using Base::Base;

		MContainer(numericarray_data_t type, mint rank, const mint* dims) {
			RawContainer tmp {};
			if (LibraryData::NumericArrayAPI()->MNumericArray_new(type, rank, dims, &tmp)) {
				ErrorManager::throwException(ErrorName::NumericArrayNewError);
			}
			this->setContainer(tmp);
		}

		template<class P>
		MContainer(const MContainer<MArgumentType::NumericArray, P>& mc) : Base(mc) {
		}

		MContainer(const MContainer& mc) = default;

		MContainer(MContainer&& mc) noexcept = default;

		template<class P>
		MContainer& operator=(const MContainer<MArgumentType::NumericArray, P>& mc) {
			Base::operator=(mc);
			return *this;
		}

		MContainer& operator=(const MContainer&) = default;

		MContainer& operator=(MContainer&&) noexcept = default;

		~MContainer() override {
			this->cleanup();
		};

		GenericNumericArray<Passing::Manual> convert(numericarray_data_t t, NA::ConversionMethod method, double param) const {
			MNumericArray newNA = nullptr;
			auto err = LibraryData::NumericArrayAPI()->MNumericArray_convertType(
					&newNA, this->getContainer(), t,
					static_cast<numericarray_convert_method_t>(method), param
			);
			if (err) {
				ErrorManager::throwException(ErrorName::NumericArrayConversionError, "Conversion to type " + std::to_string(static_cast<int>(t)) + " failed.");
			}
			return newNA;
		}

		mint getRank() const noexcept {
			return LibraryData::NumericArrayAPI()->MNumericArray_getRank(this->getContainer());
		}

		mint const* getDimensions() const {
			return LibraryData::NumericArrayAPI()->MNumericArray_getDimensions(this->getContainer());
		}

		mint getFlattenedLength() const {
			return LibraryData::NumericArrayAPI()->MNumericArray_getFlattenedLength(this->getContainer());
		}

		mint type() const {
			return LibraryData::NumericArrayAPI()->MNumericArray_getType(this->getContainer());
		}

	private:

		RawContainer cloneImpl() const override {
			RawContainer tmp {};
			if (LibraryData::NumericArrayAPI()->MNumericArray_clone(this->getContainer(), &tmp)) {
				ErrorManager::throwException(ErrorName::NumericArrayCloneError);
			}
			return tmp;
		}

		mint shareCountImpl() const noexcept override {
			return LibraryData::NumericArrayAPI()->MNumericArray_shareCount(this->getContainer());
		}

		void disownImpl() const noexcept override {
			LibraryData::NumericArrayAPI()->MNumericArray_disown(this->getContainer());
		}

		void freeImpl() const noexcept override {
			LibraryData::NumericArrayAPI()->MNumericArray_free(this->getContainer());
		}

		void passImpl(MArgument& res) const noexcept override {
			MArgument_setMNumericArray(res, this->abandonContainer());
		}
	};

}

#endif //LLU_INCLUDE_LLU_CONTAINERS_GENERIC_NUMERICARRAY
