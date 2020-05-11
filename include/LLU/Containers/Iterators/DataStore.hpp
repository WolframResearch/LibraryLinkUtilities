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

	struct GenericDataNode {
		DataStoreNode node;

		GenericDataNode next() const noexcept;

		MArgumentType type() const noexcept;

		std::string_view name() const noexcept;

		Argument::TypedArgument value() const;

		operator bool() const;
	};

	class DataStoreIterator {
		DataStoreNode node;

	public:
		using value_type = GenericDataNode;
		using reference = value_type;
		using iterator_category = std::input_iterator_tag;
		using pointer = void*;
		using difference_type = mint;

		DataStoreIterator(DataStoreNode n) : node{n} {}

		reference operator*() const {
			return reference {node};
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
