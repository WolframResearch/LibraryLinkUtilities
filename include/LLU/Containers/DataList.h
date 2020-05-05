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
#include "LLU/ErrorLog/ErrorManager.h"
#include "LLU/LibraryData.h"
#include "LLU/MArgument.h"
#include "LLU/TypedMArgument.h"

namespace LLU {

	/**
	 * @class	DataNode
	 * @brief	Wrapper over DataStoreNode structure from LibraryLink.
	 * 			It stores node name in std::string and node value as MArgument, getters for both are provided.
	 */
	template<typename T>
	class DataNode {
		static constexpr bool isGeneric = std::is_same_v<T, TypedArgument>;
		static_assert(isTypedArgument<T> || isGeneric, "DataNode type is not a valid MArgument wrapper type.");

	public:
		/**
		 * @brief 	Create DataNode from raw DataStoreNode structure
		 * @param 	dsn - raw node
		 */
		explicit DataNode(DataStoreNode dsn);

		DataNode(DataStoreNode dsn, TypedArgument arg);

		/**
		 * @brief 	Get node value
		 * @return 	Returns a reference to node value
		 */
		T& value() {
			if constexpr (isGeneric) {
				return nodeArg;
			} else {
				// After the constructor is done, nodeArg will always hold an object of type T, so no validation needed
				return *std::get_if<T>(&nodeArg);
			}
		}

		/**
		 * @brief 	Get node value
		 * @return 	Returns a reference to node value
		 */
		const T& value() const {
			if constexpr (isGeneric) {
				return nodeArg;
			} else {
				// After the constructor is done, nodeArg will always hold an object of type T, so no validation needed
				return *std::get_if<T>(&nodeArg);
			}
		}

		T* valuePtr() {
			return std::addressof(value());
		}

		const T* valuePtr() const {
			return std::addressof(value());
		}

		std::string_view name() const {
			return node.name();
		}

		/**
		 * @brief 	Get the actual type of node value stored in MArgument.
		 * 			This is useful when working on a "generic" DataList of type MArgumentType::MArgument, otherwise it should always return MArgType
		 * @return	Actual type of node value
		 */
		static constexpr MArgumentType valueType() noexcept {
			if constexpr (isGeneric) {
				return MArgumentType::MArgument;
			} else {
				typedArgumentIndex<T>;
			}
		}

		template <std::size_t N>
		decltype(auto) get() const {
			static_assert(N < 2, "Bad structure binding attempt to a DataNode.");
			if constexpr (N == 0) {
				return name();
			} else {
				return (value());
			}
		}
	private:
		GenericDataNode node;
		TypedArgument nodeArg;
	};


	/**
	 * @class	DataList
	 * @brief 	Top-level wrapper over LibraryLink's DataStore.
	 * 			Designed to be strongly typed i.e. to wrap only homogeneous DataStores but by passing MArgumentType::MArgument as template parameter it will
	 * work with arbitrary DataStores.
	 * @tparam 	T - type of data stored in each node, see the enum type \c MArgumentType
	 * @tparam 	PassingMode - policy for memory management of the internal container
	 */
	template<typename T>
	class DataList : public MContainer<MArgumentType::DataStore> {
	public:
		using ProxyContainer = std::vector<DataNode<T>>;
		using iterator = typename ProxyContainer::iterator;
		using const_iterator = typename ProxyContainer::const_iterator;
		using reverse_iterator = typename ProxyContainer::reverse_iterator;
		using const_reverse_iterator = typename ProxyContainer::const_reverse_iterator;
		using size_type = typename ProxyContainer::size_type;
		using value_type = T;
		using GenericDataStore = MContainer<MArgumentType::DataStore>;

	public:
		using GenericDataStore::GenericDataStore;

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

		DataList clone() const;

		/**
		 * @brief 	Get size of the DataList, which is the number of nodes in the list
		 */
		size_type size() const {
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

		DataNode<T>& operator[](size_type index) {
			return proxyList[index];
		}

		const DataNode<T>& operator[](size_type index) const {
			return proxyList[index];
		}

		DataNode<T>& at(size_type index) {
			if (index >= size()) {
				ErrorManager::throwException(ErrorName::DimensionsError);
			}
			return proxyList[index];
		}

		const DataNode<T>& at(size_type index) const {
			if (index >= size()) {
				ErrorManager::throwException(ErrorName::DimensionsError);
			}
			return proxyList[index];
		}

		/**
		 * @brief 	Add new node to the DataList.
		 * @param 	nodeData - actual data to store in the new node
		 */
		void push_back(value_type nodeData);

		/**
		 * @brief 	Add new named node to the DataList.
		 * @param 	name - name for the new node
		 * @param 	nodeData - actual data to store in the new node
		 */
		void push_back(std::string_view name, value_type nodeData);

	private:
		/**
		 * @brief 	Recreate private proxy list from the internal DataStore
		 */
		void makeProxy();

		/// private proxy list with top-level wrappers of each node of the internal DataStore
		ProxyContainer proxyList;
	};

	/* Definitions od DataNode methods */
	template<typename T>
	DataNode<T>::DataNode(DataStoreNode dsn) : node {dsn} {
		if (!dsn) {
			ErrorManager::throwException(ErrorName::DLNullRawNode);
		}
		nodeArg = node.value();
		if constexpr (!isGeneric) {
			if (!std::holds_alternative<T>(nodeArg)) {
				ErrorManager::throwException(ErrorName::DLInvalidNodeType);
			}
		}
	}

	template<typename T>
	DataNode<T>::DataNode(DataStoreNode dsn, TypedArgument arg) : node {dsn}, nodeArg {std::move(arg)} {
		if constexpr (!isGeneric) {
			if (!std::holds_alternative<T>(nodeArg)) {
				ErrorManager::throwException(ErrorName::DLInvalidNodeType);
			}
		}
	}

	/* Definitions od DataList methods */

	template<typename T>
	DataList<T>::DataList(GenericDataStore gds) : GenericDataStore(std::move(gds)) {
		makeProxy();
	}

	template<typename T>
	DataList<T>::DataList(std::initializer_list<value_type> initList) : DataList() {
		for (auto&& elem : initList) {
			push_back(std::move(elem));
		}
	}

	template<typename T>
	DataList<T>::DataList(std::initializer_list<std::pair<std::string, value_type>> initList) : DataList() {
		for (auto&& elem : initList) {
			push_back(elem.first, std::move(elem.second));
		}
	}

	template<typename T>
	void DataList<T>::makeProxy() {
		auto size = length();
		auto currentNode = front();
		for (mint i = 0; i < size; ++i) {
			proxyList.emplace_back(currentNode);
			currentNode = LibraryData::DataStoreAPI()->DataStoreNode_getNextNode(currentNode);
		}
	}

	template<typename T>
	DataList<T> DataList<T>::clone() const {
		return DataList {cloneContainer(), Ownership::Library};
	}

	template<typename T>
	void DataList<T>::push_back(value_type nodeData) {
		TypedArgument t {std::move(nodeData)};
		GenericDataStore::push_back(t);
		proxyList.emplace_back(this->back(), std::move(t));
	}

	template<typename T>
	void DataList<T>::push_back(std::string_view name, value_type nodeData) {
		TypedArgument t {std::move(nodeData)};
		GenericDataStore::push_back(name, t);
		proxyList.emplace_back(this->back(), std::move(t));
	}

} /* namespace LLU */

namespace std {
	template<typename T>
	struct tuple_size<LLU::DataNode<T>> : std::integral_constant<std::size_t, 2> {};

	template<std::size_t N, typename T>
	struct tuple_element<N, LLU::DataNode<T>> {
		using type = decltype(std::declval<LLU::DataNode<T>>().template get<N>());
	};
}

#endif	  // LLUTILS_DATALIST_H
