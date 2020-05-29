/**
 * @file	DataList.hpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	September 07, 2018
 * @brief	Special iterators for DataLists. Iteration over keys, values, reversed iteration.
 */
#ifndef LLU_ITERATORS_DATALIST_HPP
#define LLU_ITERATORS_DATALIST_HPP

#include <iterator>
#include <list>
#include <string>

#include "LLU/Containers/Iterators/DataNode.hpp"
#include "LLU/Containers/Iterators/DataStore.hpp"
#include "LLU/MArgument.h"

namespace LLU {

	namespace Detail {
		struct DataListIteratorPrimitive {
			using iterator_category = std::input_iterator_tag;
			using pointer = void*;
			using difference_type = mint;

			GenericDataNode node;

			explicit DataListIteratorPrimitive(DataStoreNode n) : node{n} {}

			explicit DataListIteratorPrimitive(const DataStoreIterator& it) : node{*it} {}

			friend bool operator==(const DataListIteratorPrimitive& lhs, const DataListIteratorPrimitive& rhs) {
				return lhs.node.node == rhs.node.node;
			}
			friend bool operator!=(const DataListIteratorPrimitive& lhs, const DataListIteratorPrimitive& rhs) {
				return !(lhs == rhs);
			}
		};
	}

	/**
	 * @brief   Simple proxy input iterator that goes over a DataStore and returns proxy DataNodes when dereferenced
	 * @tparam  T - data node type, see LLU::NodeType namespace for supported node types
	 */
	template<typename T>
	struct NodeIterator : Detail::DataListIteratorPrimitive {
		using value_type = DataNode<T>;
		using reference = value_type;

		using DataListIteratorPrimitive::DataListIteratorPrimitive;

		reference operator*() const {
			return reference {node};
		}

		NodeIterator& operator++() {
			node = node.next();
			return *this;
		}

		NodeIterator operator++(int) {
			NodeIterator tmp {node};
			++(*this);
			return tmp;
		}
	};

	/**
	 * @brief   Simple proxy input iterator that goes over a DataStore and returns node names when dereferenced
	 * @tparam  T - data node type, see LLU::NodeType namespace for supported node types
	 */
	struct NodeNameIterator : Detail::DataListIteratorPrimitive {
		using value_type = std::string_view ;
		using reference = value_type;

		using DataListIteratorPrimitive::DataListIteratorPrimitive;

		template<typename T>
		explicit NodeNameIterator(const NodeIterator<T>& it) : DataListIteratorPrimitive {it} {}

		reference operator*() const {
			return node.name();
		}

		NodeNameIterator& operator++() {
			node = node.next();
			return *this;
		}

		NodeNameIterator operator++(int) {
			NodeNameIterator tmp {node.node};
			++(*this);
			return tmp;
		}
	};

	/**
	 * @brief   Simple proxy input iterator that goes over a DataStore and returns node values of requested type when dereferenced
	 * @tparam  T - data node type, see LLU::NodeType namespace for supported node types
	 */
	template<typename T>
	struct NodeValueIterator : Detail::DataListIteratorPrimitive {
		using value_type = T;
		using reference = value_type;

		using DataListIteratorPrimitive::DataListIteratorPrimitive;

		explicit NodeValueIterator(const NodeIterator<T>& it) : DataListIteratorPrimitive {it} {}

		reference operator*() const {
			if constexpr (std::is_same_v<T, Argument::Typed::Any>) {
				return  node.value();
			} else {
				return as<T>();
			}
		}

		NodeValueIterator& operator++() {
			node = node.next();
			return *this;
		}

		NodeValueIterator operator++(int) {
			NodeValueIterator tmp {node.node};
			++(*this);
			return tmp;
		}

		template<typename U>
		U as() const {
			auto v = node.value();
			auto* ptr = std::get_if<U>(std::addressof(v));
			if (!ptr) {
				ErrorManager::throwException(ErrorName::DLInvalidNodeType);
			}
			return std::move(*ptr);
		}
	};

}	 // namespace LLU

#endif	  // LLU_ITERATORS_DATALIST_HPP
