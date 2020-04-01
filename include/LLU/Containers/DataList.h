/**
 * @file	DataList.h
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	September 01, 2018
 * @brief	Definition of templated DataStore wrapper called DataList.
 */

#ifndef LLUTILS_DATALIST_H
#define LLUTILS_DATALIST_H
#include <cstddef>
#include <iostream>
#include <iterator>
#include <list>
#include <type_traits>
#include <utility>

#include "LLU/Containers/Generic/DataStore.hpp"
#include "LLU/Containers/Passing/Automatic.hpp"
#include "LLU/Containers/Passing/Manual.hpp"
#include "LLU/ErrorLog/ErrorManager.h"
#include "LLU/LibraryData.h"
#include "LLU/MArgument.h"

namespace LLU {

	/**
	 * @class	DataNode
	 * @brief	Wrapper over DataStoreNode structure from LibraryLink.
	 * 			It stores node name in std::string and node value as MArgument, getters for both are provided.
	 * @tparam 	MArgType - type of the argument stored in the node, see enum MArgumentType
	 */
	template<MArgumentType MArgType>
	class DataNode {
	public:
		using T = MType_t<MArgType>;

	public:
		/**
		 * @brief 	Create DataNode from raw DataStoreNode structure
		 * @param 	dsn - raw node
		 */
		explicit DataNode(DataStoreNode dsn);

		/**
		 * @brief 	Get node name
		 * @return 	const reference to std::string holding node name
		 */
		const std::string& getName() const {
			return name;
		}

		/**
		 * @brief 	Get the address of node name
		 * @return	Address of the string holding node name
		 */
		const std::string* getNameAddress() const {
			return &name;
		}

		/**
		 * @brief 	Get node value
		 * @return 	Returns a reference to node value
		 */
		T& getValue() {
			return *getValueAddress();
		}

		/**
		 * @brief 	Get the address of node value
		 * @return	Address of the node value
		 */
		T* getValueAddress() {
			return Argument<MArgType>(nodeArg).getAddress();
		}

		/**
		 * @brief 	Get the actual type of node value stored in MArgument.
		 * 			This is useful when working on a "generic" DataList of type MArgumentType::MArgument, otherwise it should always return MArgType
		 * @return	Actual type of node value
		 */
		MArgumentType getRawType() {
			return static_cast<MArgumentType>(LibraryData::DataStoreAPI()->DataStoreNode_getDataType(rawNode));
		}

		/// static data member holding the template parameter
		static constexpr MArgumentType type = MArgType;

	private:
		std::string name;
		MArgument nodeArg {};
		DataStoreNode rawNode {};
	};

	/**
	 * @class	DataList
	 * @brief 	Top-level wrapper over LibraryLink's DataStore.
	 * 			Designed to be strongly typed i.e. to wrap only homogeneous DataStores but by passing MArgumentType::MArgument as template parameter it will
	 * work with arbitrary DataStores.
	 * @tparam 	T - type of data stored in each node, see the enum type \c MArgumentType
	 * @tparam 	PassingMode - policy for memory management of the internal container
	 */
	template<MArgumentType T, class PassingMode = Passing::Manual>
	class DataList : public MContainer<MArgumentType::DataStore, PassingMode> {

		/// private proxy list with top-level wrappers of each node of the internal DataStore
		std::list<DataNode<T>> proxyList;

	public:
		using iterator = typename decltype(proxyList)::iterator;
		using const_iterator = typename decltype(proxyList)::const_iterator;
		using reverse_iterator = typename decltype(proxyList)::reverse_iterator;
		using const_reverse_iterator = typename decltype(proxyList)::const_reverse_iterator;
		using value_type = MType_t<T>;
		using GenericDataStore = MContainer<MArgumentType::DataStore, PassingMode>;

		template<MArgumentType U>
		static constexpr bool ValidNodeTypeQ = (T == MArgumentType::MArgument || U == T);

		template<MArgumentType MArgT>
		using ValidNodeType = std::enable_if_t<ValidNodeTypeQ<MArgT>>;

		template<MArgumentType MArgT>
		using InvalidNodeType = std::enable_if_t<!ValidNodeTypeQ<MArgT>>;

		template<MArgumentType MArgT>
		using IsMArgument = std::enable_if_t<MArgT == MArgumentType::MArgument>;

	public:
		/**
		 * @brief	Create empty DataList
		 */
		DataList() : DataList(LibraryData::DataStoreAPI()->createDataStore()) {}

		/**
		 * @brief	Create DataList wrapping around an existing DataStore with nodes of matching type
		 * @param 	ds - DataStore
		 */
		explicit DataList(DataStore ds);

		/**
		 * @brief	Create DataList wrapping around an existing GenericDataStore with matching passing policy
		 * @param 	gds - GenericDataStore
		 */
		explicit DataList(GenericDataStore gds);

		/**
		 * @brief	Create DataList from list of values. Keys will be set to empty strings.
		 * @param 	initList - list of values to put in the DataList
		 */
		DataList(std::initializer_list<value_type> initList);

		/**
		 * @brief	Create DataList from list of keys and corresponding values.
		 * @param 	initList - list of pairs key - value to put in the DataList
		 */
		DataList(std::initializer_list<std::pair<std::string, value_type>> initList);

		/**
		 * @brief	Create DataList copying a different DataList with the same type but possible different Passing policy
		 * @tparam 	P - arbitrary passing policy
		 * @param 	other - a DataList with matching node type
		 */
		template<class P>
		explicit DataList(const DataList<T, P>& other);

		/**
		 * @brief 	Copy constructor is deleted because it doesn't make sense for some passing policies.
		 * 			Use DataList(const DataList<T, P>&) instead.
		 */
		DataList(const DataList&) = delete;

		/**
		 * @brief	Create a DataList by moving contents of another DataList
		 * @param 	other - DataList to move from
		 */
		DataList(DataList&& other) noexcept : GenericDataStore(std::move(other)), proxyList(std::move(other.proxyList)) {};

		/**
		 * @brief	Delete copy-assignment operator for similar reasons to why copy constructor is deleted.
		 */
		DataList& operator=(const DataList&) = delete;

		/**
		 * @brief	Move-assignment operator. Move contents of another DataList into this one.
		 * @param 	other - DataList to move from
		 * @return	*this
		 */
		DataList& operator=(DataList&& other) noexcept;

		/**
		 *   @brief         Copy-assignment operator with passing mode change
		 *   @param[in]     other - const reference to a DataList of matching type
		 **/
		template<class P>
		DataList& operator=(const DataList<T, P>& other) {
			GenericDataStore::operator=(other);
			proxyList = other.proxyList;
			return *this;
		}

		/**
		 * @brief	Perform a deep copy of the DataList returning a new DataList with Manual passing policy
		 */
		DataList<T, Passing::Manual> clone() const {
			return DataList<T, Passing::Manual>(GenericDataStore::clone());
		}

		/**
		 * @brief 	Get size of the DataList, which is the number of nodes in the list
		 */
		std::size_t size() const {
			return proxyList.size();
		}

		/**
		 *	@brief Get iterator at the beginning of underlying data
		 **/
		iterator begin() {
			return proxyList.begin();
		}

		/**
		 *	@brief Get constant iterator at the beginning of underlying data
		 **/
		const_iterator begin() const {
			return proxyList.begin();
		}

		/**
		 *	@brief Get constant iterator at the beginning of underlying data
		 **/
		const_iterator cbegin() const {
			return proxyList.cbegin();
		}

		/**
		 *	@brief Get iterator after the end of underlying data
		 **/
		iterator end() {
			return proxyList.end();
		}

		/**
		 *	@brief Get constant iterator after the end of underlying data
		 **/
		const_iterator end() const {
			return proxyList.end();
		}

		/**
		 *	@brief Get constant reverse iterator after the end of underlying data
		 **/
		const_iterator cend() const {
			return proxyList.cend();
		}

		/**
		 *	@brief Get constant reverse iterator at the beginning of underlying data
		 **/
		reverse_iterator rbegin() {
			return proxyList.rbegin();
		}

		/**
		 *	@brief Get constant reverse iterator at the beginning of underlying data
		 **/
		const_reverse_iterator rbegin() const {
			return proxyList.rbegin();
		}

		/**
		 *	@brief Get reverse iterator after the end of underlying data
		 **/
		reverse_iterator rend() {
			return proxyList.rend();
		}

		/**
		 *	@brief Get constant reverse iterator after the end of underlying data
		 **/
		const_reverse_iterator rend() const {
			return proxyList.rend();
		}

		/**
		 * @brief 	Add new node to the DataList of type MArgument. This overload is considered only for "generic" DataLists.
		 * @tparam 	U - dummy template parameter, should never be explicitly specified
		 * @param 	nodeData - actual data to store in the new node
		 * @param 	MArgT - type of value stored in \c nodeData. It must be passed because the low-level DataStore API requires it.
		 * @return	nothing
		 */
		template<MArgumentType U = T>
		IsMArgument<U> push_back(MArgument& nodeData, MArgumentType MArgT);

		/**
		 * @brief 	Add new node to the DataList of type MArgument. This overload is considered only for "generic" DataLists.
		 * @tparam 	U - dummy template parameter, should never be explicitly specified
		 * @param 	name - name to be stored in the new node
		 * @param 	nodeData - actual data to store in the new node
		 * @param 	MArgT - type of value stored in \c nodeData. It must be passed because the low-level DataStore API requires it.
		 * @return 	nothing
		 */
		template<MArgumentType U = T>
		IsMArgument<U> push_back(const std::string& name, MArgument& nodeData, MArgumentType MArgT);

		/**
		 * @brief	Add new node to the DataList. This overload is considered only if T is MArgumentType::MArgument or MArgT is equal to T.
		 * @tparam 	MArgT - type of MArgument data
		 * @param 	nodeData - actual data to store in the new node
		 * @return	nothing
		 */
		template<MArgumentType MArgT>
		ValidNodeType<MArgT> push_back(const MType_t<MArgT>& nodeData);

		/**
		 * @brief	Issue compile-time error message. This overload is considered only if T is not MArgumentType::MArgument and MArgT is not equal to T.
		 * @tparam 	MArgT - type of MArgument data
		 */
		template<MArgumentType MArgT>
		InvalidNodeType<MArgT> push_back(const MType_t<MArgT>&) {
			static_assert(alwaysFalse<MArgT>, "Trying to add DataList node of incorrect type.");
		}

		/**
		 * @brief 	Add new named node to the DataList. This overload is considered only if T is MArgumentType::MArgument or MArgT is equal to T.
		 * @tparam 	MArgT - type of MArgument data
		 * @param 	name - name for the new node
		 * @param 	nodeData - actual data to store in the new node
		 * @return 	nothing
		 */
		template<MArgumentType MArgT>
		ValidNodeType<MArgT> push_back(const std::string& name, const MType_t<MArgT>& nodeData);

		/**
		 * @brief	Issue compile-time error message. This overload is considered only if T is not MArgumentType::MArgument and MArgT is not equal to T.
		 * @tparam 	MArgT - type of MArgument data
		 */
		template<MArgumentType MArgT>
		InvalidNodeType<MArgT> push_back(const std::string&, const MType_t<MArgT>&) {
			static_assert(alwaysFalse<MArgT>, "Trying to add DataList node of incorrect type.");
		}

		template<MArgumentType MArgT, class P>
		ValidNodeType<MArgT> push_back(const MContainer<MArgT, P>& nodeData) {
			push_back("", nodeData);
		}

		template<MArgumentType MArgT, class P>
		ValidNodeType<MArgT> push_back(const std::string& name, const MContainer<MArgT, P>& nodeData) {
			push_back<MArgT>(name, nodeData.abandonContainer());
		}

		/**
		 * @brief 	Add new node to the DataList.
		 * @param 	nodeData - actual data to store in the new node
		 */
		void push_back(const value_type& nodeData);

		/**
		 * @brief 	Add new named node to the DataList.
		 * @param 	name - name for the new node
		 * @param 	nodeData - actual data to store in the new node
		 */
		void push_back(const std::string& name, const value_type& nodeData);

	private:
		/**
		 * @brief 	Recreate private proxy list from the internal DataStore
		 */
		void makeProxy();
	};

	/* Definitions od DataNode methods */

	template<MArgumentType MArgType>
	DataNode<MArgType>::DataNode(DataStoreNode dsn) : rawNode(dsn) {
		if (!rawNode) {
			ErrorManager::throwException(ErrorName::DLNullRawNode);
		}
		if (MArgType != MArgumentType::MArgument && MArgType != getRawType()) {
			ErrorManager::throwException(ErrorName::DLInvalidNodeType);
		}
		char* rawName = nullptr;
		LibraryData::DataStoreAPI()->DataStoreNode_getName(rawNode, &rawName);
		if (rawName != nullptr) {
			name = rawName;
		}
		if (LibraryData::DataStoreAPI()->DataStoreNode_getData(rawNode, &nodeArg) != 0) {
			ErrorManager::throwException(ErrorName::DLGetNodeDataError);
		}
	}

	/* Definitions od DataList methods */

	template<MArgumentType T, class PassingMode>
	DataList<T, PassingMode>::DataList(DataStore ds) : GenericDataStore(ds) {
		if (!this->getContainer()) {
			ErrorManager::throwException(ErrorName::DLNullRawDataStore);
		}
		makeProxy();
	}

	template<MArgumentType T, class PassingMode>
	DataList<T, PassingMode>::DataList(GenericDataStore gds) : GenericDataStore(std::move(gds)) {
		if (!this->getContainer()) {
			ErrorManager::throwException(ErrorName::DLNullRawDataStore);
		}
		makeProxy();
	}

	template<MArgumentType T, class PassingMode>
	DataList<T, PassingMode>::DataList(std::initializer_list<value_type> initList) : DataList() {
		for (auto&& elem : initList) {
			push_back(elem);
		}
	}

	template<MArgumentType T, class PassingMode>
	DataList<T, PassingMode>::DataList(std::initializer_list<std::pair<std::string, value_type>> initList) : DataList() {
		for (auto&& elem : initList) {
			push_back(elem.first, elem.second);
		}
	}

	template<MArgumentType T, class PassingMode>
	auto DataList<T, PassingMode>::operator=(DataList&& other) noexcept -> DataList& {
		GenericDataStore::operator=(std::move(other));
		proxyList = std::move(other.proxyList);
		return *this;
	}

	template<MArgumentType T, class PassingMode>
	void DataList<T, PassingMode>::makeProxy() {
		auto size = LibraryData::DataStoreAPI()->DataStore_getLength(this->getContainer());
		auto currentNode = LibraryData::DataStoreAPI()->DataStore_getFirstNode(this->getContainer());
		for (mint i = 0; i < size; ++i) {
			proxyList.emplace_back(currentNode);
			currentNode = LibraryData::DataStoreAPI()->DataStoreNode_getNextNode(currentNode);
		}
	}

	template<MArgumentType T, class PassingMode>
	template<MArgumentType U>
	auto DataList<T, PassingMode>::push_back(MArgument& nodeData, MArgumentType MArgT) -> IsMArgument<U> {
		push_back("", nodeData, MArgT);
	}

	template<MArgumentType T, class PassingMode>
	template<MArgumentType U>
	auto DataList<T, PassingMode>::push_back(const std::string& name, MArgument& nodeData, MArgumentType MArgT) -> IsMArgument<U> {
		Argument<MArgumentType::MArgument>(nodeData).addToDataStore(this->getContainer(), name, MArgT);
		proxyList.emplace_back(this->getLastNode());
	}

	template<MArgumentType T, class PassingMode>
	template<MArgumentType MArgT>
	auto DataList<T, PassingMode>::push_back(const MType_t<MArgT>& nodeData) -> ValidNodeType<MArgT> {
		push_back<MArgT>("", nodeData);
	}

	template<MArgumentType T, class PassingMode>
	template<MArgumentType MArgT>
	auto DataList<T, PassingMode>::push_back(const std::string& name, const MType_t<MArgT>& nodeData) -> ValidNodeType<MArgT> {
		Argument<MArgT>::addDataStoreNode(this->getContainer(), name, nodeData);
		proxyList.emplace_back(this->getLastNode());
	}

	template<MArgumentType T, class PassingMode>
	void DataList<T, PassingMode>::push_back(const DataList::value_type& nodeData) {
		push_back("", nodeData);
	}

	template<MArgumentType T, class PassingMode>
	void DataList<T, PassingMode>::push_back(const std::string& name, const DataList::value_type& nodeData) {
		Argument<T>::addDataStoreNode(this->getContainer(), name, nodeData);
		proxyList.emplace_back(this->getLastNode());
	}

	template<MArgumentType T, class PassingMode>
	template<class P>
	DataList<T, PassingMode>::DataList(const DataList<T, P>& other) : GenericDataStore(other) {
		makeProxy();
	}

}

#endif	  // LLUTILS_DATALIST_H
