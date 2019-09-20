/**
 * @file    DataStore.hpp
 * @brief   Definition and implementation of generic DataStore wrapper.
 */

#ifndef LLU_INCLUDE_LLU_CONTAINERS_GENERIC_DATASTORE
#define LLU_INCLUDE_LLU_CONTAINERS_GENERIC_DATASTORE

#include "LLU/Containers/Generic/Base.hpp"
#include "LLU/Containers/Passing/Shared.hpp"

namespace LLU {

	template<class PassingMode>
	class MContainer<MArgumentType::DataStore, PassingMode>;

	/// MContainer specialization for DataStore is called GenericDataList
	template<class PassingMode>
	using GenericDataList = MContainer<MArgumentType::DataStore, PassingMode>;

	/**
	 *  @brief  MContainer specialization for DataStore
	 *  @tparam PassingMode - passing policy
	 */
	template<class PassingMode>
	class MContainer<MArgumentType::DataStore, PassingMode> : public MContainerBase<MArgumentType::DataStore, PassingMode> {
		static_assert(!std::is_same<PassingMode, Passing::Shared>::value, "DataStore cannot be Shared.");

	public:
		/// Inherit constructors from MContainerBase
		using MContainerBase<MArgumentType::DataStore, PassingMode>::MContainerBase;

		/**
		 * @brief   Default constructor, the MContainer does not manage any instance of DataStore.
		 */
		MContainer() = default;

		/**
		 * @brief   Create GenericDataList from another GenericDataList with different passing mode.
		 * @tparam  P - some passing mode
		 * @param   mc - different GenericDataList
		 */
		template<class P>
		explicit MContainer(const MContainer<MArgumentType::DataStore, P>& mc) : Base(mc) {}

		MContainer(const MContainer& mc) = default;

		MContainer(MContainer&& mc) noexcept = default;

		MContainer& operator=(const MContainer& mc) = default;

		MContainer& operator=(MContainer&& mc) noexcept = default;

		/**
		 * @brief   Assign a GenericDataList with different passing mode.
		 * @tparam  P - some passing mode
		 * @param   mc - different GenericDataList
		 * @return  this
		 */
		template<class P>
		MContainer& operator=(const MContainer<MArgumentType::DataStore, P>& mc) {
			Base::operator=(mc);
			return *this;
		}

		/// Destructor which triggers the appropriate cleanup action which depends on the PassingMode
		~MContainer() {
			this->cleanup();
		};

		/**
		 * @brief   Get the length of the DataStore.
		 * @return  total number of nodes in the DataStore
		 */
		mint getLength() const {
			return LibraryData::DataStoreAPI()->DataStore_getLength(this->getContainer());
		}

		/**
		 * @brief   Get the first node of the DataStore.
		 * @return  first node, if it doesn't exists the behavior is undefined
		 */
		DataStoreNode getFirstNode() {
			return LibraryData::DataStoreAPI()->DataStore_getFirstNode(this->getContainer());
		};

		/**
		 * @brief   Get the last node of the DataStore.
		 * @return  last node, if it doesn't exists the behavior is undefined
		 */
		DataStoreNode getLastNode() {
			return LibraryData::DataStoreAPI()->DataStore_getLastNode(this->getContainer());
		};

	private:
		using Base = MContainerBase<MArgumentType::DataStore, PassingMode>;
		using RawContainer = typename Base::Container;

		/// Make a deep copy of the raw container
		RawContainer cloneImpl() const override {
			return LibraryData::DataStoreAPI()->copyDataStore(this->getContainer());
		}

		/**
		 * @brief   Get a share count.
		 * @return  always 0 to indicate that DataStore cannot be shared
		 */
		mint shareCountImpl() const noexcept override {
			return 0;
		}

		/**
		 * @brief   Disowning does nothing for DataStore as it cannot be shared.
		 */
		void disownImpl() const noexcept override {}

		/**
		 * @brief   Free the underlying raw container.
		 */
		void freeImpl() const noexcept override {
			LibraryData::DataStoreAPI()->deleteDataStore(this->getContainer());
		}

		/**
		 * @brief   Pass the raw container as result of a library function.
		 * @param   res - MArgument that will store the result of library function
		 */
		void passImpl(MArgument& res) const noexcept override {
			MArgument_setDataStore(res, this->getContainer());
		}
	};

}

#endif	  // LLU_INCLUDE_LLU_CONTAINERS_GENERIC_DATASTORE
