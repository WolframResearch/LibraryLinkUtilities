/**
 * @file
 * @brief
 */

#ifndef LLU_INCLUDE_LLU_CONTAINERS_GENERIC_DATASTORE
#define LLU_INCLUDE_LLU_CONTAINERS_GENERIC_DATASTORE

#include "LLU/Containers/Generic/Base.hpp"
#include "LLU/Containers/Passing/Shared.hpp"

namespace LLU {

	template<class PassingMode>
	class MContainer<MArgumentType::DataStore, PassingMode>;

	template<class PassingMode>
	using GenericDataList = MContainer<MArgumentType::DataStore, PassingMode>;


	template<class PassingMode>
	class MContainer<MArgumentType::DataStore, PassingMode> : public MContainerBase<MArgumentType::DataStore, PassingMode> {
		static_assert(!std::is_same<PassingMode, Passing::Shared>::value, "DataStore cannot be Shared.");
	public:

		using Base = MContainerBase<MArgumentType::DataStore, PassingMode>;
		using RawContainer = typename Base::Container;

		using Base::Base;

		template<class P>
		MContainer(const MContainer<MArgumentType::DataStore, P>& mc) : Base(mc) {
		}

		MContainer(const MContainer& mc) = default;

		MContainer(MContainer&& mc) noexcept = default;

		MContainer& operator=(const MContainer& mc) = default;

		MContainer& operator=(MContainer&& mc) noexcept = default;

		template<class P>
		MContainer &operator=(const MContainer<MArgumentType::DataStore, P> &mc) {
			Base::operator=(mc);
			return *this;
		}

		~MContainer() {
			this->cleanup();
		};

		mint getLength() const {
			return LibraryData::DataStoreAPI()->DataStore_getLength(this->getContainer());
		}

		DataStoreNode getFirstNode() {
			return LibraryData::DataStoreAPI()->DataStore_getFirstNode(this->getContainer());
		};

		DataStoreNode getLastNode() {
			return LibraryData::DataStoreAPI()->DataStore_getLastNode(this->getContainer());
		};

	private:
		RawContainer cloneImpl() const override {
			return LibraryData::DataStoreAPI()->copyDataStore(this->getContainer());
		}

		mint shareCountImpl() const noexcept override {
			return 0;
		}

		void disownImpl() const noexcept override {
		}

		void freeImpl() const noexcept override {
			LibraryData::DataStoreAPI()->deleteDataStore(this->getContainer());
		}

		void passImpl(MArgument& res) const noexcept override {
			MArgument_setDataStore(res, this->abandonContainer());
		}
	};

}

#endif //LLU_INCLUDE_LLU_CONTAINERS_GENERIC_DATASTORE
