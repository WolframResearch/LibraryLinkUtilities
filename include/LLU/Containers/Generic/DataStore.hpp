/**
 * @file    DataStore.hpp
 * @brief   Definition and implementation of generic DataStore wrapper.
 */

#ifndef LLU_INCLUDE_LLU_CONTAINERS_GENERIC_DATASTORE
#define LLU_INCLUDE_LLU_CONTAINERS_GENERIC_DATASTORE

#include "LLU/Containers/Generic/Base.hpp"

namespace LLU {

	template<>
	class MContainer<MArgumentType::DataStore>;

	/// MContainer specialization for DataStore is called GenericDataList
	using GenericDataList = MContainer<MArgumentType::DataStore>;

	/**
	 *  @brief  MContainer specialization for DataStore
	 *  @tparam PassingMode - passing policy
	 */
	template<>
	class MContainer<MArgumentType::DataStore> : public MContainerBase<MArgumentType::DataStore> {
	public:
		/// Inherit constructors from MContainerBase
		using MContainerBase<MArgumentType::DataStore>::MContainerBase;

		/**
		 * @brief   Default constructor, the MContainer does not manage any instance of DataStore.
		 */
		MContainer() = default;

		template<Ownership PassingMode = Ownership::Library>
		/* implicit */ MContainer(Container c) : MContainerBase<PassingMode> {c} {
			static_assert(PassingMode != Ownership::Shared, "DataStore cannot be Shared.");
		}

		MContainer(const MContainer& mc) = default;

		MContainer(MContainer&& mc) noexcept = default;

		MContainer& operator=(const MContainer& mc) = default;

		MContainer& operator=(MContainer&& mc) noexcept = default;

		/// Destructor which triggers the appropriate cleanup action which depends on the PassingMode
		~MContainer() override = default;

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
		using Base = MContainerBase<MArgumentType::DataStore>;
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
		 * @brief   Pass the raw container as result of a library function.
		 * @param   res - MArgument that will store the result of library function
		 */
		void passImpl(MArgument& res) const noexcept override {
			MArgument_setDataStore(res, this->getContainer());
		}
	};

}

#endif	  // LLU_INCLUDE_LLU_CONTAINERS_GENERIC_DATASTORE
