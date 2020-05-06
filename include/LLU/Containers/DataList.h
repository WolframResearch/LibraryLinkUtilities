/**
 * @file	DataList.h
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	September 01, 2018
 * @brief	Definition of templated DataStore wrapper called DataList.
 */

#ifndef LLUTILS_DATALIST_H
#define LLUTILS_DATALIST_H

#include <initializer_list>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "LLU/Containers/Generic/DataStore.hpp"
#include "LLU/Containers/Iterators/DataNode.hpp"
#include "LLU/ErrorLog/ErrorManager.h"
#include "LLU/LibraryData.h"
#include "LLU/MArgument.h"
#include "LLU/TypedMArgument.h"

namespace LLU {

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

	public:
		using GenericDataList::GenericDataList;

		/**
		 * @brief	Create DataList wrapping around an existing GenericDataList with matching passing policy
		 * @param 	gds - GenericDataList
		 */
		explicit DataList(GenericDataList gds);

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
			return proxy.size();
		}

		/**
		 *	@brief Get iterator at the beginning of underlying data
		 **/
		iterator begin() {
			return proxy.begin();
		}

		/**
		 *	@brief Get constant iterator at the beginning of underlying data
		 **/
		const_iterator begin() const {
			return proxy.begin();
		}

		/**
		 *	@brief Get constant iterator at the beginning of underlying data
		 **/
		const_iterator cbegin() const {
			return proxy.cbegin();
		}

		/**
		 *	@brief Get iterator after the end of underlying data
		 **/
		iterator end() {
			return proxy.end();
		}

		/**
		 *	@brief Get constant iterator after the end of underlying data
		 **/
		const_iterator end() const {
			return proxy.end();
		}

		/**
		 *	@brief Get constant reverse iterator after the end of underlying data
		 **/
		const_iterator cend() const {
			return proxy.cend();
		}

		/**
		 *	@brief Get constant reverse iterator at the beginning of underlying data
		 **/
		reverse_iterator rbegin() {
			return proxy.rbegin();
		}

		/**
		 *	@brief Get constant reverse iterator at the beginning of underlying data
		 **/
		const_reverse_iterator rbegin() const {
			return proxy.rbegin();
		}

		/**
		 *	@brief Get reverse iterator after the end of underlying data
		 **/
		reverse_iterator rend() {
			return proxy.rend();
		}

		/**
		 *	@brief Get constant reverse iterator after the end of underlying data
		 **/
		const_reverse_iterator rend() const {
			return proxy.rend();
		}

		DataNode<T>& operator[](size_type index) {
			return proxy[index];
		}

		const DataNode<T>& operator[](size_type index) const {
			return proxy[index];
		}

		DataNode<T>& at(size_type index) {
			if (index >= size()) {
				ErrorManager::throwException(ErrorName::DimensionsError);
			}
			return proxy[index];
		}

		const DataNode<T>& at(size_type index) const {
			if (index >= size()) {
				ErrorManager::throwException(ErrorName::DimensionsError);
			}
			return proxy[index];
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
		ProxyContainer proxy;
	};

	/* Definitions od DataList methods */

	template<typename T>
	DataList<T>::DataList(GenericDataList gds) : GenericDataList(std::move(gds)) {
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
			proxy.emplace_back(currentNode);
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
		GenericDataList::push_back(t);
		proxy.emplace_back(this->back(), std::move(t));
	}

	template<typename T>
	void DataList<T>::push_back(std::string_view name, value_type nodeData) {
		TypedArgument t {std::move(nodeData)};
		GenericDataList::push_back(name, t);
		proxy.emplace_back(this->back(), std::move(t));
	}

}

#endif	  // LLUTILS_DATALIST_H
