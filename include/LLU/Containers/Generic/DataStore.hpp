/**
 * @file    DataStore.hpp
 * @brief   Definition and implementation of generic DataStore wrapper.
 */

#ifndef LLU_INCLUDE_LLU_CONTAINERS_GENERIC_DATASTORE
#define LLU_INCLUDE_LLU_CONTAINERS_GENERIC_DATASTORE

#include <LLU/Containers/Generic/Base.hpp>
#include <LLU/Containers/Iterators/DataStore.hpp>

#include <LLU/MArgument.h>
#include <LLU/TypedMArgument.h>

namespace LLU {

	/// MContainer specialization for DataStore is called GenericDataList
	using GenericDataList = MContainer<MArgumentType::DataStore>;

	/**
	 *  @brief  MContainer specialization for DataStore
	 *  @tparam PassingMode - passing policy
	 */
	template<>
	class MContainer<MArgumentType::DataStore> : public MContainerBase<MArgumentType::DataStore> {

		template<typename T>
		using EnableIfArgumentType = std::enable_if_t<Argument::PrimitiveQ<std::decay_t<T>> || Argument::WrapperQ<std::decay_t<T>>, int>;

		template<MArgumentType T>
		using EnableIfUnambiguousWrapperType = std::enable_if_t<!Argument::PrimitiveQ<Argument::WrapperType<T>>, int>;

	public:
		using iterator = DataStoreIterator;
		using const_iterator = iterator;

		/// Inherit constructors from MContainerBase
		using MContainerBase<MArgumentType::DataStore>::MContainerBase;

		/**
		 * @brief   Default constructor, creates empty DataStore owned by the Library
		 */
		MContainer() : MContainer(LibraryData::DataStoreAPI()->createDataStore(), Ownership::Library) {}

		MContainer(Container c, Ownership owner);

		MContainer clone() const {
			return MContainer {cloneContainer(), Ownership::Library};
		}

		/**
		 * @brief   Get the length of the DataStore.
		 * @return  total number of nodes in the DataStore
		 */
		mint length() const {
			return LibraryData::DataStoreAPI()->DataStore_getLength(this->getContainer());
		}

		/**
		 * @brief   Get the first node of the DataStore.
		 * @return  first node, if it doesn't exist the behavior is undefined
		 */
		DataStoreNode front() const {
			return LibraryData::DataStoreAPI()->DataStore_getFirstNode(this->getContainer());
		};

		/**
		 * @brief   Get the last node of the DataStore.
		 * @return  last node, if it doesn't exist the behavior is undefined
		 */
		DataStoreNode back() const {
			return LibraryData::DataStoreAPI()->DataStore_getLastNode(this->getContainer());
		};

		iterator begin() const {
			return front();
		}

		iterator end() const {
			return nullptr;
		}

		const_iterator cbegin() const {
			return begin();
		}

		const_iterator cend() const {
			return end();
		}

		template<typename T, EnableIfArgumentType<T> = 0>
		void push_back(T nodeValue) {
			// MTensor and MNumericArray are actually the same type, so this function cannot handle them correctly.
			// Use push_back templated with MArgumentType instead.
			static_assert(!std::is_same_v<T, MTensor>, "Do not use push_back templated on the argument type with MTensor or MNumericArray.");
			if constexpr (Argument::PrimitiveQ<T>) {
				constexpr MArgumentType Type = Argument::PrimitiveIndex<T>;
				PrimitiveWrapper<Type>::addDataStoreNode(getContainer(), nodeValue);
			} else if constexpr (Argument::WrapperQ<T>) {
				constexpr MArgumentType Type = Argument::WrapperIndex<T>;
				PrimitiveWrapper<Type>::addDataStoreNode(getContainer(), Argument::toPrimitiveType<Type>(nodeValue));
			}
		}

		template<typename T, EnableIfArgumentType<T> = 0>
		void push_back(std::string_view name, T nodeValue) {
			static_assert(!std::is_same_v<T, MTensor>, "Do not use push_back templated on the argument type with MTensor or MNumericArray.");
			if constexpr (Argument::PrimitiveQ<T>) {
				constexpr MArgumentType Type = Argument::PrimitiveIndex<T>;
				PrimitiveWrapper<Type>::addDataStoreNode(getContainer(), name, nodeValue);
			} else if constexpr (Argument::WrapperQ<T>) {
				constexpr MArgumentType Type = Argument::WrapperIndex<T>;
				PrimitiveWrapper<Type>::addDataStoreNode(getContainer(), name, Argument::toPrimitiveType<Type>(nodeValue));
			}
		}

		template<MArgumentType Type, EnableIfUnambiguousWrapperType<Type> = 0>
		void push_back(Argument::WrapperType<Type> nodeValue) {
			PrimitiveWrapper<Type>::addDataStoreNode(getContainer(), Argument::toPrimitiveType<Type>(nodeValue));
		}

		template<MArgumentType Type, EnableIfUnambiguousWrapperType<Type> = 0>
		void push_back(std::string_view name, Argument::WrapperType<Type> nodeValue) {
			PrimitiveWrapper<Type>::addDataStoreNode(getContainer(), name, Argument::toPrimitiveType<Type>(nodeValue));
		}

		template<MArgumentType Type>
		void push_back(Argument::CType<Type> nodeValue) {
			PrimitiveWrapper<Type>::addDataStoreNode(getContainer(), nodeValue);
		}

		template<MArgumentType Type>
		void push_back(std::string_view name, Argument::CType<Type> nodeValue) {
			PrimitiveWrapper<Type>::addDataStoreNode(getContainer(), name, nodeValue);
		}

		void push_back(const Argument::TypedArgument& node);
		void push_back(std::string_view name, const Argument::TypedArgument& node);

	private:
		/// Make a deep copy of the raw container
		Container cloneImpl() const override {
			return LibraryData::DataStoreAPI()->copyDataStore(this->getContainer());
		}

		/**
		 * @brief   Get a share count.
		 * @return  always 0 to indicate that DataStore cannot be shared
		 */
		mint shareCountImpl() const noexcept override {
			return 0;
		}

		/**
		 * @brief   Pass the raw container as result of a library function.
		 * @param   res - MArgument that will store the result of library function
		 */
		void passImpl(MArgument& res) const noexcept override {
			MArgument_setDataStore(res, this->getContainer());
		}
	};
}

#endif	  // LLU_INCLUDE_LLU_CONTAINERS_GENERIC_DATASTORE
