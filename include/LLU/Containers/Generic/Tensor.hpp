/**
 * @file
 * @brief
 */

#ifndef LLU_INCLUDE_LLU_CONTAINERS_GENERIC_TENSOR
#define LLU_INCLUDE_LLU_CONTAINERS_GENERIC_TENSOR

#include "LLU/Containers/Generic/Base.hpp"

namespace LLU {
	template<class PassingMode>
	class MContainer<MArgumentType::Tensor, PassingMode>;

	template<class PassingMode>
	using GenericTensor = MContainer<MArgumentType::Tensor, PassingMode>;

	template<class PassingMode>
	class MContainer<MArgumentType::Tensor, PassingMode> : public MContainerBase<MArgumentType::Tensor, PassingMode> {
	public:
		using Base = MContainerBase<MArgumentType::Tensor, PassingMode>;
		using RawContainer = typename Base::Container;

		using Base::Base;

		/**
		 * @brief   Default constructor, the MContainer does not manage any instance of MTensor.
		 */
		MContainer() = default;

		MContainer(mint type, mint rank, const mint* dims) {
			RawContainer tmp {};
			if (LibraryData::API()->MTensor_new(type, rank, dims, &tmp)) {
				ErrorManager::throwException(ErrorName::TensorNewError);
			}
			this->setContainer(tmp);
		}

		template<class P>
		MContainer(const MContainer<MArgumentType::Tensor, P>& mc) : Base(mc) {
		}

		MContainer(const MContainer& mc) = default;

		MContainer(MContainer&& mc) noexcept = default;

		MContainer& operator=(const MContainer&) = default;

		template<class P>
		MContainer& operator=(const MContainer<MArgumentType::Tensor, P>& mc) {
			Base::operator=(mc);
			return *this;
		}

		MContainer& operator=(MContainer&& mc) noexcept = default;

		~MContainer() {
			this->cleanup();
		};

		mint getRank() const noexcept {
			return LibraryData::API()->MTensor_getRank(this->getContainer());
		}

		mint const* getDimensions() const {
			return LibraryData::API()->MTensor_getDimensions(this->getContainer());
		}

		mint getFlattenedLength() const {
			return LibraryData::API()->MTensor_getFlattenedLength(this->getContainer());
		}

		mint type() const {
			return LibraryData::API()->MTensor_getType(this->getContainer());
		}

	private:
		/**
		 *
		 * @return
		 * @see 	<http://reference.wolfram.com/language/LibraryLink/ref/callback/MTensor_shareCount.html>
		 */
		mint shareCountImpl() const noexcept override {
			return LibraryData::API()->MTensor_shareCount(this->getContainer());
		}

		/**
		 *   @brief 	Disown internal MTensor that is shared with Mathematica.
		 *
		 *   @see 		<http://reference.wolfram.com/language/LibraryLink/ref/callback/MTensor_disown.html>
		 **/
		void disownImpl() const noexcept override {
			LibraryData::API()->MTensor_disown(this->getContainer());
		}

		void freeImpl() const noexcept override {
			LibraryData::API()->MTensor_free(this->getContainer());
		}

		void passImpl(MArgument& res) const noexcept override {
			MArgument_setMTensor(res, this->abandonContainer());
		}

		RawContainer cloneImpl() const override {
			RawContainer tmp {};
			if (LibraryData::API()->MTensor_clone(this->getContainer(), &tmp)) {
				ErrorManager::throwException(ErrorName::TensorCloneError);
			}
			return tmp;
		}
	};

}

#endif //LLU_INCLUDE_LLU_CONTAINERS_GENERIC_TENSOR
