/**
 * @file	DataStore.hpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	May 04, 2020
 * @brief
 */
#ifndef LIBRARYLINKUTILITIES_CONTAINERS_ITERATORS_DATASTORE_HPP
#define LIBRARYLINKUTILITIES_CONTAINERS_ITERATORS_DATASTORE_HPP

#include <iterator>

#include <LLU/LibraryData.h>
#include <LLU/MArgument.h>
#include <LLU/TypedMArgument.h>

namespace LLU {

	/**
	 * @struct  GenericDataNode
	 * @brief   Basic wrapper over DataStoreNode, provides class-like interface and conversion of the underlying value from MArgument to TypedArgument.
	 */
	struct GenericDataNode {
		DataStoreNode node;

		[[nodiscard]] GenericDataNode next() const noexcept;

		[[nodiscard]] MArgumentType type() const noexcept;

		[[nodiscard]] std::string_view name() const noexcept;

		[[nodiscard]] Argument::TypedArgument value() const;

		// defined in Containers/GenericDataStore.hpp because the definition of GenericDataList must be available
		template<typename T>
		T as() const;

		explicit operator bool() const;

		GenericDataNode* operator->() {
			return this;
		}
	};

	/**
	 * @class   DataStoreIterator
	 * @brief   Proxy input iterator over DataStoreNodes, when dereferenced yields GeneridDataNode proxy objects.
	 */
	class DataStoreIterator {
		DataStoreNode node;

	public:
		using value_type = GenericDataNode;
		using reference = value_type;
		using iterator_category = std::input_iterator_tag;
		using pointer = value_type;
		using difference_type = mint;

		explicit DataStoreIterator(DataStoreNode n) : node{n} {}

		reference operator*() const {
			return reference {node};
		}

		pointer operator->() const {
			return pointer {node};
		}

		DataStoreIterator& operator++() {
			node = LLU::LibraryData::DataStoreAPI()->DataStoreNode_getNextNode(node);
			return *this;
		}

		DataStoreIterator operator++(int) {
			DataStoreIterator tmp {node};
			++(*this);
			return tmp;
		}

		friend bool operator==(const DataStoreIterator& lhs, const DataStoreIterator& rhs) {
			return lhs.node == rhs.node;
		}
		friend bool operator!=(const DataStoreIterator& lhs, const DataStoreIterator& rhs) {
			return !(lhs == rhs);
		}
	};
}

#endif	  // LIBRARYLINKUTILITIES_CONTAINERS_ITERATORS_DATASTORE_HPP
