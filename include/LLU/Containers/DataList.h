/**
 * @file	DataList.h
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	September 01, 2018
 * @brief	Definition of templated DataStore wrapper called DataList.
 */

#ifndef LLUTILS_DATALIST_H
#define LLUTILS_DATALIST_H
#include <iostream>

#include <cstddef>
#include <iterator>
#include <list>
#include <type_traits>
#include <utility>

#include "WolframLibrary.h"
#include "WolframIOLibraryFunctions.h"

#include "LLU/LibraryLinkError.h"
#include "LLU/MArgument.h"
#include "LLU/Containers/LibDataHolder.h"
#include "LLU/Containers/Passing/Automatic.hpp"
#include "LLU/Containers/Passing/Manual.hpp"

namespace LibraryLinkUtils {

	template<MArgumentType MArgType>
	class DataNode : public LibDataHolder {
	public:
		using T = MType_t<MArgType>;
	public:

		explicit DataNode(DataStoreNode dsn);

		const std::string& getName() const {
			return name;
		}

		const std::string* getNameAddress() const {
			return &name;
		}

		T& getValue() {
			return *getValueAddress();
		}

		T* getValueAddress() {
			return Argument<MArgType>(nodeArg).getAddress();
		}

		MArgumentType getRawType() {
			return static_cast<MArgumentType>(ioFuns->DataStoreNode_getDataType(rawNode));
		}

		static constexpr MArgumentType type = MArgType;
	private:
		std::string name;
		MArgument nodeArg;
		DataStoreNode rawNode;
	};


	template<MArgumentType T, template<typename> class PassingMode = Passing::Manual>
	class DataList : public LibDataHolder, public PassingMode<DataStore> {

		std::list<DataNode<T>> proxyList;

	public:
		using iterator = typename decltype(proxyList)::iterator;
		using const_iterator = typename decltype(proxyList)::const_iterator;
		using reverse_iterator = typename decltype(proxyList)::reverse_iterator;
		using const_reverse_iterator = typename decltype(proxyList)::const_reverse_iterator;
		using value_type = MType_t<T>;
		using PassingM = PassingMode<DataStore>;

		template<MArgumentType U>
		static constexpr bool ValidNodeTypeQ = (T == MArgumentType::MArgument && U != T);

		template<MArgumentType MArgT>
		using ValidNodeType = std::enable_if_t<ValidNodeTypeQ<MArgT>>;

		template<MArgumentType MArgT>
		using InvalidNodeType = std::enable_if_t <!ValidNodeTypeQ<MArgT>>;

		template<MArgumentType MArgT>
		using IsMArgument = std::enable_if_t<MArgT == MArgumentType::MArgument>;

	public:
		DataList() : DataList(ioFuns->createDataStore()) {
		}

		explicit DataList(DataStore ds) : PassingM(ds) {
			if (!this->getInternal()) {
				ErrorManager::throwException(LLErrorName::DLNullRawDataStore);
			}
			makeProxy();
		}

		DataList(std::initializer_list<value_type> initList) : DataList() {
			for(auto&& elem : initList) {
				push_back(elem);
			}
		}

		DataList(std::initializer_list<std::pair<std::string, value_type>> initList) : DataList() {
			for(auto&& elem : initList) {
				push_back(elem.first, elem.second);
			}
		}

		template<template<typename> class P>
		DataList(const DataList<T, P>& other);

		DataList(const DataList&) = delete;

		DataList(DataList&& other) noexcept : PassingM(std::move(other)), proxyList(std::move(other.proxyList)) {
		};

		DataList& operator=(const DataList&) = delete;

		DataList& operator=(DataList&& other) noexcept {
			PassingM::operator=(std::move(other));
			proxyList = std::move(other.proxyList);
		};

		DataList<T, Passing::Manual> clone() const {
			return DataList<T, Passing::Manual>(cloneInternal());
		}

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


		template<MArgumentType U = T>
		IsMArgument<U> push_back(MArgument& nodeData, MArgumentType MArgT);

		template<MArgumentType U = T>
		IsMArgument<U> push_back(const std::string& name, MArgument& nodeData, MArgumentType MArgT);

		template<MArgumentType MArgT>
		ValidNodeType<MArgT> push_back(const MType_t<MArgT>& nodeData);

		template<MArgumentType MArgT>
		InvalidNodeType<MArgT> push_back(const MType_t<MArgT>&) {
			static_assert(alwaysFalse<MArgT>, "Trying to add DataList node of incorrect type.");
		}

		template<MArgumentType MArgT>
		ValidNodeType<MArgT> push_back(const std::string& name, const MType_t<MArgT>& nodeData);

		template<MArgumentType MArgT>
		InvalidNodeType<MArgT> push_back(const std::string&, const MType_t<MArgT>&) {
			static_assert(alwaysFalse<MArgT>, "Trying to add DataList node of incorrect type.");
		}

		void push_back(const value_type& nodeData);

		void push_back(const std::string& name, const value_type& nodeData);

	private:
		/**
		 *   @brief 	Free internal container
		 **/
		void freeInternal() noexcept override {
			this->ioFuns->deleteDataStore(this->getInternal());
		}

		/**
		 *   @brief 		Set internal container as result for LibraryLink.
		 *   @param[out]	res - MArgument that will carry the internal container
		 **/
		void passInternal(MArgument& res) const noexcept override {
			MArgument_setDataStore(res, this->getInternal());
		}

		/**
		 * @brief	Make a deep copy of the internally stored DataStore
		 * @return	Cloned DataStore
		 */
		DataStore cloneInternal() const override {
			return ioFuns->copyDataStore(this->getInternal());
		}

	private:
		void makeProxy();
	};

	template<MArgumentType MArgType>
	DataNode<MArgType>::DataNode(DataStoreNode dsn) : rawNode(dsn) {
		if (!rawNode) {
			ErrorManager::throwException(LLErrorName::DLNullRawNode);
		}
		if (MArgType != MArgumentType::MArgument && MArgType != getRawType()) {
			ErrorManager::throwException(LLErrorName::DLInvalidNodeType);
		}
		char* rawName = nullptr;
		ioFuns->DataStoreNode_getName(rawNode, &rawName);
		if (rawName != nullptr) {
			name = rawName;
		}
		if (ioFuns->DataStoreNode_getData(rawNode, &nodeArg) != 0) {
			ErrorManager::throwException(LLErrorName::DLGetNodeDataError);
		}
	}

	template<MArgumentType T, template<typename> class PassingMode>
	void DataList<T, PassingMode>::makeProxy() {
		auto size = ioFuns->DataStore_getLength(this->getInternal());
		auto currentNode = ioFuns->DataStore_getFirstNode(this->getInternal());
		for (mint i = 0; i < size; ++i) {
			proxyList.emplace_back(currentNode);
			currentNode = ioFuns->DataStoreNode_getNextNode(currentNode);
		}
	}

	template<MArgumentType T, template<typename> class PassingMode>
	template<MArgumentType U>
	auto DataList<T, PassingMode>::push_back(MArgument& nodeData, MArgumentType MArgT) -> IsMArgument<U>  {
		push_back("", nodeData, MArgT);
	}

	template<MArgumentType T, template<typename> class PassingMode>
	template<MArgumentType U>
	auto DataList<T, PassingMode>::push_back(const std::string& name, MArgument& nodeData, MArgumentType MArgT) -> IsMArgument<U>  {
		Argument<MArgumentType::MArgument>(nodeData).addToDataStore(this->getInternal(), name, MArgT);
	}

	template<MArgumentType T, template<typename> class PassingMode>
	template<MArgumentType MArgT>
	auto DataList<T, PassingMode>::push_back(const MType_t<MArgT>& nodeData) -> ValidNodeType<MArgT> {
		push_back<MArgT>("", nodeData);
	}

	template<MArgumentType T, template<typename> class PassingMode>
	template<MArgumentType MArgT>
	auto DataList<T, PassingMode>::push_back(const std::string& name, const MType_t<MArgT>& nodeData) -> ValidNodeType<MArgT> {
		Argument<MArgT>::addDataStoreNode(this->getInternal(), name, nodeData);
	}

	template<MArgumentType T, template<typename> class PassingMode>
	void DataList<T, PassingMode>::push_back(const DataList::value_type& nodeData) {
		push_back("", nodeData);
	}

	template<MArgumentType T, template<typename> class PassingMode>
	void DataList<T, PassingMode>::push_back(const std::string& name, const DataList::value_type& nodeData) {
		Argument<T>::addDataStoreNode(this->getInternal(), name, nodeData);
		proxyList.emplace_back(ioFuns->DataStore_getLastNode(this->getInternal()));
	}

	template<MArgumentType T, template<typename> class PassingMode>
	template<template<typename> class P>
	DataList<T, PassingMode>::DataList(const DataList<T, P>& other) : PassingM(other) {
		makeProxy();
	}

}

#endif //LLUTILS_DATALIST_H
