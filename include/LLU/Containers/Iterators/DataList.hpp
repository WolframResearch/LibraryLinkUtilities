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

#include "LLU/Containers/DataList.h"
#include "LLU/MArgument.h"

namespace LLU {

	/**
	 * @class	SpecialIterator
	 * @brief 	Iterator that traverses a DataList having custom value_type and an arbitrary iterator as a base.
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
		 * @param dataNode - raw pointer to the node which the iterator should point to
		 */
		explicit SpecialIterator(DataNode<T>* dataNode) : baseIter(dataNode) {};

		/**
		 * @brief Construct the SpecialIterator from an iterator of base type
		 * @param listIterator - iterator of base type
		 */
		SpecialIterator(BaseIter listIterator) : baseIter(listIterator) {};

		/**
		 * @brief test for iterator equality
		 */
		bool operator==(const SpecialIterator& other) const {
			return (baseIter == other.baseIter);
		}

		/**
		 * @brief test for iterator inequality
		 */
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
		/// an instance of the base iterator
		BaseIter baseIter;
	};

	/**
	 * @class 	NodeValueIterator
	 * @brief 	Special iterator class that traverses a DataList as if it only contained node values. Inherits from SpecialIterator.
	 * @tparam 	T - type of nodes in the DataList.
	 * @tparam 	BaseIter - iterator class to pass as template argument to SpecialIterator
	 */
	template<MArgumentType T, typename BaseIter = typename std::list<DataNode<T>>::iterator>
	class NodeValueIterator : public SpecialIterator<T, MType_t<T>, BaseIter> {
	public:
		using Super = SpecialIterator<T, MType_t<T>, BaseIter>;
		using pointer = typename Super::pointer;
		using reference = typename Super::reference;

	public:
		/// Inherited constructors
		using Super::Super;

		/**
		 * @brief 	Get value of the currently pointed to node
		 * @return	Reference to the value of the currently pointed to node
		 */
		reference operator*() {
			return this->baseIter->getValue();
		}

		/**
		 * @brief 	Get value of the currently pointed to node
		 * @return	Reference to the value of the currently pointed to node
		 */
		const reference operator*() const {
			return this->baseIter->getValue();
		}

		/**
		 * @brief 	Get pointer to the value of the currently pointed to node
		 * @return	Pointer to the value of the currently pointed to node
		 */
		pointer operator->() {
			return this->baseIter->getValueAddress();
		}

		/**
		 * @brief 	Get pointer to the value of the currently pointed to node
		 * @return	Pointer to the value of the currently pointed to node
		 */
		const pointer operator->() const {
			return this->baseIter->getValueAddress();
		}

		/**
		 * @brief Preincrement operator: move the iterator one value forward
		 */
		NodeValueIterator& operator++() {
			this->baseIter++;
			return (*this);
		}

		/**
		 * @brief Postincrement operator: move the iterator one value forward
		 */
		NodeValueIterator operator++(int) {
			NodeValueIterator tmp = *this;
			++*this;
			return tmp;
		}

		/**
		 * @brief Predecrement operator: move the iterator one value back
		 */
		NodeValueIterator& operator--() {
			--this->baseIter;
			return (*this);
		}

		/**
		 * @brief Postdecrement operator: move the iterator one value back
		 */
		NodeValueIterator operator--(int) {
			NodeValueIterator tmp = *this;
			--*this;
			return tmp;
		}
	};

	/**
	 * @class	NodeNameIterator
	 * @brief 	Special iterator class that traverses a DataList as if it only contained node names (keys). Inherits from SpecialIterator.
	 * @tparam 	T - type of nodes in the DataList.
	 * @tparam 	BaseIter - iterator class to pass as template argument to SpecialIterator
	 */
	template<MArgumentType T, typename BaseIter = typename std::list<DataNode<T>>::iterator>
	class NodeNameIterator : public SpecialIterator<T, const std::string, BaseIter> {
	public:
		using Super = SpecialIterator<T, const std::string, BaseIter>;
		using pointer = typename Super::pointer;
		using reference = typename Super::reference;

	public:
		/// Inherited constructors
		using Super::Super;

		/**
		 * @brief 	Get name of the currently pointed to node
		 * @return	Reference to the name of the currently pointed to node
		 */
		reference operator*() const {
			return this->baseIter->getName();
		}

		/**
		 * @brief 	Get pointer to the name of the currently pointed to node
		 * @return	Pointer to the name of the currently pointed to node
		 */
		pointer operator->() const {
			return this->baseIter->getNameAddress();
		}

		/**
		 * @brief Preincrement operator: move the iterator one value forward
		 */
		NodeNameIterator& operator++() {
			this->baseIter++;
			return (*this);
		}

		/**
		 * @brief Postincrement operator: move the iterator one value forward
		 */
		NodeNameIterator operator++(int) {
			NodeNameIterator tmp = *this;
			++*this;
			return tmp;
		}

		/**
		 * @brief Predecrement operator: move the iterator one value back
		 */
		NodeNameIterator& operator--() {
			--this->baseIter;
			return (*this);
		}

		/**
		 * @brief Postdecrement operator: move the iterator one value back
		 */
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

}	 // namespace LLU

#endif	  // LLU_ITERATORS_DATALIST_HPP
