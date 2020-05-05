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

		TypedArgument value() const;

		operator bool();
	};

	class DataStoreIterator {
		GenericDataNode node;

	public:
		using value_type = GenericDataNode;
		using reference = value_type;
		using iterator_category = std::forward_iterator_tag;
		using pointer = GenericDataNode*;
		using difference_type = void;

		DataStoreIterator(DataStoreNode n) : node{n} {}

		DataStoreIterator(GenericDataNode n) : node{n} {}

		reference operator*() const {
			return node;
		}

		DataStoreIterator& operator++() {
			node = node.next();
			return *this;
		}

		DataStoreIterator operator++(int) {
			DataStoreIterator tmp {node};
			++(*this);
			return tmp;
		}

		friend bool operator==(const DataStoreIterator& lhs, const DataStoreIterator& rhs) {
			return lhs.node.node == rhs.node.node;
		}
		friend bool operator!=(const DataStoreIterator& lhs, const DataStoreIterator& rhs) {
			return !(lhs == rhs);
		}
	};
}

#endif	  // LIBRARYLINKUTILITIES_CONTAINERS_ITERATORS_DATASTORE_HPP