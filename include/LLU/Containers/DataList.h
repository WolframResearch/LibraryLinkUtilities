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
#include "LLU/Containers/Iterators/DataList.hpp"
#include "LLU/ErrorLog/ErrorManager.h"
#include "LLU/LibraryData.h"
#include "LLU/MArgument.h"
#include "LLU/TypedMArgument.h"

namespace LLU {

	namespace NodeType = Argument::Typed;
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
		using iterator = NodeIterator<T>;
		using const_iterator = iterator;
		using value_iterator = NodeValueIterator<T>;
		using name_iterator = NodeNameIterator;
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
		 *	@brief Get constant iterator at the beginning of underlying data
		 **/
		iterator begin() const {
			return front();
		}

		/**
		 *	@brief Get constant iterator at the beginning of underlying data
		 **/
		const_iterator cbegin() const {
			return begin();
		}

		/**
		 *	@brief Get constant iterator after the end of underlying data
		 **/
		iterator end() const {
			return nullptr;
		}

		/**
		 *	@brief Get constant reverse iterator after the end of underlying data
		 **/
		const_iterator cend() const {
			return end();
		}

		value_iterator valueBegin() const {
			return front();
		}

		value_iterator valueEnd() const {
			return nullptr;
		}

		name_iterator nameBegin() const {
			return front();
		}

		name_iterator nameEnd() const {
			return nullptr;
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

		std::vector<T> values() const {
			return {valueBegin(), valueEnd()};
		}

		std::vector<std::string> names() const {
			return {nameBegin(), nameEnd()};
		}

		std::vector<DataNode<T>> toVector() const {
			return {cbegin(), cend()};
		}
	};

	/* Definitions od DataList methods */

	template<typename T>
	DataList<T>::DataList(GenericDataList gds) : GenericDataList(std::move(gds)) {
		if constexpr (!std::is_same_v<T, LLU::NodeType::Any>) {
			std::for_each(GenericDataList::cbegin(), GenericDataList::cend(), [](auto node) {
				if (node.type() != Argument::WrapperIndex<T>) {
					ErrorManager::throwException(ErrorName::DLInvalidNodeType);
				}
			});
		}
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
	DataList<T> DataList<T>::clone() const {
		return DataList {cloneContainer(), Ownership::Library};
	}

	template<typename T>
	void DataList<T>::push_back(value_type nodeData) {
		GenericDataList::push_back(std::move(nodeData));
	}

	template<typename T>
	void DataList<T>::push_back(std::string_view name, value_type nodeData) {
		GenericDataList::push_back(name, std::move(nodeData));
	}

}

#endif	  // LLUTILS_DATALIST_H
