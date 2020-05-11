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

			DataListIteratorPrimitive(DataStoreNode n) : node{n} {}

			DataListIteratorPrimitive& operator++() {
				node = node.next();
				return *this;
			}

			DataListIteratorPrimitive operator++(int) {
				DataListIteratorPrimitive tmp {node.node};
				++(*this);
				return tmp;
			}

			friend bool operator==(const DataListIteratorPrimitive& lhs, const DataListIteratorPrimitive& rhs) {
				return lhs.node == rhs.node;
			}
			friend bool operator!=(const DataListIteratorPrimitive& lhs, const DataListIteratorPrimitive& rhs) {
				return !(lhs == rhs);
			}
		};
	}


	template<typename T>
	struct DataListIterator : Detail::DataListIteratorPrimitive {
		using value_type = DataNode<T>;
		using reference = value_type;

		using DataListIteratorPrimitive::DataListIteratorPrimitive;

		reference operator*() const {
			return reference {node.node};
		}
	};

	struct DataListNameIterator : Detail::DataListIteratorPrimitive {
		using value_type = std::string_view ;
		using reference = value_type;

		using DataListIteratorPrimitive::DataListIteratorPrimitive;

		reference operator*() const {
			return node.name();
		}
	};

	template<typename T>
	struct DataListValueIterator : Detail::DataListIteratorPrimitive {
		using value_type = T;
		using reference = value_type;

		using DataListIteratorPrimitive::DataListIteratorPrimitive;

		reference operator*() const {
			auto v = node.value();
			if constexpr (std::is_same_v<T, Argument::Typed::Any>) {
				return v;
			} else {
				return *std::get_if<T>(std::addressof(v));
			}
		}
	};

}	 // namespace LLU

#endif	  // LLU_ITERATORS_DATALIST_HPP
