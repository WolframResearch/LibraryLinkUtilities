/**
 * @file	DataNode.hpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	May 06, 2020
 * @brief
 */
#ifndef LIBRARYLINKUTILITIES_CONTAINERS_ITERATORS_DATANODE_HPP
#define LIBRARYLINKUTILITIES_CONTAINERS_ITERATORS_DATANODE_HPP

#include <type_traits>

#include <LLU/TypedMArgument.h>

namespace LLU {

	/**
	 * @class	DataNode
	 * @brief	Wrapper over DataStoreNode structure from LibraryLink.
	 * 			It stores node name in std::string and node value as MArgument, getters for both are provided.
	 */
	template<typename T>
	class DataNode {
		static constexpr bool isGeneric = std::is_same_v<T, Argument::TypedArgument>;
		static_assert(Argument::WrapperQ<T>, "DataNode type is not a valid MArgument wrapper type.");

	public:
		/**
		 * @brief 	Create DataNode from raw DataStoreNode structure
		 * @param 	dsn - raw node
		 */
		explicit DataNode(DataStoreNode dsn);

		DataNode(DataStoreNode dsn, Argument::TypedArgument arg);

		/**
		 * @brief 	Get node value
		 * @return 	Returns a reference to node value
		 */
		T& value() {
			if constexpr (isGeneric) {
				return nodeArg;
			} else {
				// After the constructor is done, nodeArg will always hold an object of type T, so no validation needed
				return *std::get_if<T>(&nodeArg);
			}
		}

		/**
		 * @brief 	Get node value
		 * @return 	Returns a reference to node value
		 */
		const T& value() const {
			if constexpr (isGeneric) {
				return nodeArg;
			} else {
				// After the constructor is done, nodeArg will always hold an object of type T, so no validation needed
				return *std::get_if<T>(&nodeArg);
			}
		}

		T* valuePtr() {
			return std::addressof(value());
		}

		const T* valuePtr() const {
			return std::addressof(value());
		}

		std::string_view name() const {
			return node.name();
		}

		bool hasNext() const {
			return node.next();
		}

		template<typename U = T>
		DataNode<U> next() const {
			return {node.next()};
		}

		/**
		 * @brief 	Get the actual type of node value stored in MArgument.
		 * 			This is useful when working on a "generic" DataList of type MArgumentType::MArgument, otherwise it should always return MArgType
		 * @return	Actual type of node value
		 */
		MArgumentType valueType() noexcept {
			return node.type();
		}

		template <std::size_t N>
		decltype(auto) get() const {
			static_assert(N < 2, "Bad structure binding attempt to a DataNode.");
			if constexpr (N == 0) {
				return name();
			} else {
				return (value());
			}
		}

	private:
		GenericDataNode node;
		Argument::TypedArgument nodeArg;
	};

	/* Definitions od DataNode methods */
	template<typename T>
	DataNode<T>::DataNode(DataStoreNode dsn) : node {dsn} {
		if (!dsn) {
			ErrorManager::throwException(ErrorName::DLNullRawNode);
		}
		nodeArg = node.value();
		if constexpr (!isGeneric) {
			if (!std::holds_alternative<T>(nodeArg)) {
				ErrorManager::throwException(ErrorName::DLInvalidNodeType);
			}
		}
	}

	template<typename T>
	DataNode<T>::DataNode(DataStoreNode dsn, Argument::TypedArgument arg) : node {dsn}, nodeArg {std::move(arg)} {
		if constexpr (!isGeneric) {
			if (!std::holds_alternative<T>(nodeArg)) {
				ErrorManager::throwException(ErrorName::DLInvalidNodeType);
			}
		}
	}

}/* namespace LLU */

namespace std {
	template<typename T>
	struct tuple_size<LLU::DataNode<T>> : std::integral_constant<std::size_t, 2> {};

	template<std::size_t N, typename T>
	struct tuple_element<N, LLU::DataNode<T>> {
		using type = decltype(std::declval<LLU::DataNode<T>>().template get<N>());
	};
}

#endif	  // LIBRARYLINKUTILITIES_CONTAINERS_ITERATORS_DATANODE_HPP
