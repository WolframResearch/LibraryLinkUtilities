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

#include "LLU/MArgument.h"
#include "LLU/Containers/DataList.h"

namespace LibraryLinkUtils {

	/**
	 * @class	NodeNameIterator
	 * @brief 	Iterator that traverses a DataList as if it was a container with keys only.
	 * @tparam 	T - type of nodes in the DataList.
	 * @tparam 	BaseIter - iterator to inherit from, it must return DataNode<T>& when dereferenced.
	 */
	template<MArgumentType T, typename BaseIter = typename std::list<DataNode<T>>::iterator>
	class NodeNameIterator : public std::list<DataNode<T>>::iterator {
	public:
		using base_iter = BaseIter;
		using difference_type = typename base_iter::difference_type;
		using pointer = const std::string*;
		using value_type = const std::string;
		using reference = const std::string&;
		using iterator_category = typename base_iter::iterator_category;

	public:
		///	Default constructor
		NodeNameIterator() = default;

		/**
		 * @brief Construct the NodeNameIterator pointing to given DataNode
		 * @param dataNode - pointer to the node which the iterator should point to
		 */
		explicit NodeNameIterator(DataNode<T>* dataNode) : base_iter(dataNode) {};

		/**
		 * @brief Construct the NodeNameIterator from an iterator of base type
		 * @param listIterator - iterator of base type
		 */
		NodeNameIterator(base_iter listIterator) : base_iter(listIterator) {};

		/**
		 * @brief 	Get name of the currently pointed to node
		 * @return	Reference to the name of the currently pointed to node
		 */
		reference operator*() {
			return base_iter::operator->()->getName();
		}

		/**
		 * @brief 	Get pointer to the name of the currently pointed to node
		 * @return	Pointer to the name of the currently pointed to node
		 */
		pointer operator->() {
			return base_iter::operator->()->getNameAddress();
		}

		/**
		 * @brief 	Get reference to the node currently pointed to by the base iterator.
		 * 			You should rarely need this function.
		 * @return 	Reference to the node currently pointed to by the base iterator.
		 */
		DataNode<T>& getNode() {
			return base_iter::operator*();
		}
	};

	/**
	 * @class	NodeValueIterator
	 * @brief 	Iterator that traverses a DataList as if it was a container with values only (without keys).
	 * @tparam 	T - type of nodes in the DataList.
	 * @tparam 	BaseIter - iterator to inherit from, it must return DataNode<T>& when dereferenced.
	 */
	template<MArgumentType T, typename BaseIter = typename std::list<DataNode<T>>::iterator>
	class NodeValueIterator : public BaseIter {
	public:
		using base_iter = BaseIter;
		using difference_type = typename base_iter::difference_type;
		using value_type = MType_t<T>;
		using pointer = value_type*;
		using reference = value_type&;
		using iterator_category = typename base_iter::iterator_category;

	public:
		///	Default constructor
		NodeValueIterator() = default;

		/**
		 * @brief Construct NodeValueIterator pointing to given DataNode
		 * @param dataNode - pointer to the node which the iterator should point to
		 */
		explicit NodeValueIterator(DataNode<T>* dataNode) : base_iter(dataNode) {};

		/**
		 * @brief Construct the NodeValueIterator from an iterator of base type
		 * @param listIterator - iterator of base type
		 */
		NodeValueIterator(base_iter listIterator) : base_iter(listIterator) {};

		/**
		 * @brief 	Get value of the currently pointed to node
		 * @return	Reference to the value of the currently pointed to node
		 */
		reference operator*() {
			return base_iter::operator->()->getValue();
		}

		/**
		 * @brief 	Get pointer to the value of the currently pointed to node
		 * @return	Pointer to the value of the currently pointed to node
		 */
		pointer operator->() {
			return base_iter::operator->()->getValueAddress();
		}

		/**
		 * @brief 	Get reference to the node currently pointed to by the base iterator.
		 * 			You should rarely need this function.
		 * @return 	Reference to the node currently pointed to by the base iterator.
		 */
		DataNode<T>& getNode() {
			return base_iter::operator*();
		}

	};

	/**
	 * @brief Reversed version of NodeNameIterator
	 */
	template<MArgumentType T>
	using ReverseNameIterator = NodeNameIterator<T, typename std::list<DataNode<T>>::reverse_iterator>;

	/**
	 * @brief Reversed version of NodeValueIterator
	 */
	template<MArgumentType T>
	using ReverseValueIterator = NodeValueIterator<T, typename std::list<DataNode<T>>::reverse_iterator>;

} // namespace LibraryLinkUtils

#endif //LLU_ITERATORS_DATALIST_HPP
