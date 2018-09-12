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
	 * @class	SpecialIterator
	 * @brief 	Iterator that traverses a DataList as if it was a container with values only (without keys).
	 * @tparam 	T - type of nodes in the DataList.
	 * @tparam 	BaseIter - iterator to inherit from, it must be a bidirectional iterator that returns DataNode<T>& when dereferenced.
	 */
	template<MArgumentType T, typename ValType, typename BaseIter>
	class SpecialIterator {
	public:
		using difference_type = typename BaseIter::difference_type;
		using value_type = ValType;
		using pointer = value_type*;
		using reference = value_type&;
		using iterator_category = std::bidirectional_iterator_tag;

	public:
		///	Default constructor
		SpecialIterator() = default;

		/**
		 * @brief Construct SpecialIterator pointing to given DataNode
		 * @param dataNode - pointer to the node which the iterator should point to
		 */
		explicit SpecialIterator(DataNode<T>* dataNode) : baseIter(dataNode) {};

		/**
		 * @brief Construct the SpecialIterator from an iterator of base type
		 * @param listIterator - iterator of base type
		 */
		SpecialIterator(BaseIter listIterator) : baseIter(listIterator) {};

		// test for iterator equality
		bool operator==(const SpecialIterator& other) const {
			return (baseIter == other.baseIter);
		}

		// test for iterator inequality
		bool operator!=(const SpecialIterator& other) const {
			return !(*this == other);
		}

		/**
		 * @brief 	Get reference to the node currently pointed to by the base iterator.
		 * 			You should rarely need this function.
		 * @return 	Reference to the node currently pointed to by the base iterator.
		 */
		DataNode<T>& getNode() {
			return *baseIter;
		}

	protected:
		BaseIter baseIter;
	};

	template<MArgumentType T, typename BaseIter = typename std::list<DataNode<T>>::iterator>
	class NodeValueIterator : public SpecialIterator<T, MType_t<T>, BaseIter> {
	public:
		using Super = SpecialIterator<T, MType_t<T>, BaseIter>;
		using pointer = typename Super::pointer;
		using reference = typename Super::reference;
	public:
		using Super::Super;

		/**
		 * @brief 	Get value of the currently pointed to node
		 * @return	Reference to the value of the currently pointed to node
		 */
		reference operator*() {
			return baseIter->getValue();
		}

		/**
		 * @brief 	Get value of the currently pointed to node
		 * @return	Reference to the value of the currently pointed to node
		 */
		const reference operator*() const {
			return baseIter->getValue();
		}

		/**
		 * @brief 	Get pointer to the value of the currently pointed to node
		 * @return	Pointer to the value of the currently pointed to node
		 */
		pointer operator->() {
			return baseIter->getValueAddress();
		}

		/**
		 * @brief 	Get pointer to the value of the currently pointed to node
		 * @return	Pointer to the value of the currently pointed to node
		 */
		const pointer operator->() const {
			return baseIter->getValueAddress();
		}

		NodeValueIterator& operator++() {
			baseIter++;
			return (*this);
		}

		NodeValueIterator operator++(int) {
			NodeValueIterator tmp = *this;
			++*this;
			return tmp;
		}

		NodeValueIterator& operator--() {
			--baseIter;
			return (*this);
		}

		NodeValueIterator operator--(int) {
			NodeValueIterator tmp = *this;
			--*this;
			return tmp;
		}
	};

	/**
	 * @class	NodeNameIterator
	 * @brief 	Iterator that traverses a DataList as if it was a container with keys only.
	 * @tparam 	T - type of nodes in the DataList.
	 * @tparam 	BaseIter - iterator to inherit from, it must return DataNode<T>& when dereferenced.
	 */
	template<MArgumentType T, typename BaseIter = typename std::list<DataNode<T>>::iterator>
	class NodeNameIterator : public SpecialIterator<T, const std::string, BaseIter> {
	public:
		using Super = SpecialIterator<T, const std::string, BaseIter>;
		using pointer = typename Super::pointer;
		using reference = typename Super::reference;
	public:
		using Super::Super;

		/**
		 * @brief 	Get name of the currently pointed to node
		 * @return	Reference to the name of the currently pointed to node
		 */
		reference operator*() const {
			return baseIter->getName();
		}

		/**
		 * @brief 	Get pointer to the name of the currently pointed to node
		 * @return	Pointer to the name of the currently pointed to node
		 */
		pointer operator->() const {
			return baseIter->getNameAddress();
		}

		NodeNameIterator& operator++() {
			baseIter++;
			return (*this);
		}

		NodeNameIterator operator++(int) {
			NodeNameIterator tmp = *this;
			++*this;
			return tmp;
		}

		NodeNameIterator& operator--() {
			--baseIter;
			return (*this);
		}

		NodeNameIterator operator--(int) {
			NodeNameIterator tmp = *this;
			--*this;
			return tmp;
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
