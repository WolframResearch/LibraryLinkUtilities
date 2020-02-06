/**
 * @file	ManagedExpression.hpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	July 17, 2019
 * @brief
 */
#ifndef LIBRARYLINKUTILITIES_MANAGEDEXPRESSION_HPP
#define LIBRARYLINKUTILITIES_MANAGEDEXPRESSION_HPP

#include <memory>
#include <unordered_map>
#include <utility>

#include <LLU/LibraryData.h>
#include <LLU/Utilities.hpp>

/**
 * Use this macro to define an instance of ManagedExpressionStore corresponding to your class
 * and a template specialization of manageInstanceCallback for the managed class.
 */
#define DEFINE_MANAGED_STORE_AND_SPECIALIZATION(ClassName)                                        \
	LLU::ManagedExpressionStore<ClassName> ClassName##Store;                                      \
                                                                                                  \
	template<>                                                                                    \
	inline void LLU::manageInstanceCallback<ClassName>(WolframLibraryData, mbool mode, mint id) { \
		ClassName##Store.manageInstance(mode, id);                                                \
	}

namespace LLU {

	/**
	 * A template for library callback used by LibraryLink to manage instances of ManagedLibraryExpressions
	 *
	 * Specializations should typically just call manageInstance method from the ManagedExpressionStore corresponding to class T.
	 *
	 * @tparam T - class to be managed
	 * @note This function must be explicitly specialized for any class that is supposed to be managed. Therefore instantiation of the general template
	 * will trigger compilation error.
	 */
	template<class T>
	void manageInstanceCallback(WolframLibraryData, mbool, mint) {
		static_assert(dependent_false_v<T>, "Use of unspecialized ManageInstance function.");
	}

	/**
	 * ManagedExpressionStore will keep track of instances of managed class T and will provide safe access to them
	 * @tparam T - managed class
	 */
	template<typename T>
	class ManagedExpressionStore {
		using iterator = typename std::unordered_map<mint, std::shared_ptr<T>>::iterator;
		using const_iterator = typename std::unordered_map<mint, std::shared_ptr<T>>::const_iterator;
		using size_type = typename std::unordered_map<mint, std::shared_ptr<T>>::size_type;

	public:
		/**
		 * Function that will actually be called by LibraryLink when an instance of Managed Expression is created or deleted
		 *
		 * Notice that this function does not actually create a new object of class T. This is because there is no way to pass constructor arguments here.
		 *
		 * @param mode - are we deleting existing instance (True) or creating new one (False)
		 * @param id - id of the instance of interest
		 */
		void manageInstance(mbool mode, mint id) {
			if (mode == False /* create new instance */) {
				store[id] = nullptr;
			} else {
				store.erase(id);
			}
		}

		/**
		 * Create new object of class T that will be managed from Wolfram Language and place it in the map of managed objects
		 * @tparam  DynamicType - actual type of the constructed object, it allows Store to keep objects of subclasses of T
		 * @tparam  Args - constructor arguments types
		 * @param   id - id of the newly created managed object
		 * @param   args - constructor arguments
		 * @return  reference to the newly created object
		 */
		template<class DynamicType = T, typename... Args>
		T& createInstance(mint id, Args&&... args) {
			checkID(id);	// at this point instance must already exist in store
			store[id] = std::make_shared<DynamicType>(std::forward<Args>(args)...);
			return *store[id];
		}

		size_type releaseInstance(mint id) {
			LibraryData::API()->releaseManagedLibraryExpression(expressionName.c_str(), id);
			return store.erase(id);
		}

		/**
		 * Check if instance with given \p id is present in the store.
		 * @param id - id to be checked
		 * @return true iff the instance with given id is in the store
		 */
		bool hasInstance(mint id) const {
			return store.count(id);
		}

		/**
		 * Get managed instance with given \p id. Throw if the \p id is invalid.
		 * @param id - id of instance of interest
		 * @return reference to the managed object
		 */
		T& getInstance(mint id) {
			checkID(id);
			return *store[id];
		}

		/**
		 * Get a shared pointer to a managed instance with given \p id. Throw if the \p id is invalid.
		 * @param id - id of instance of interest
		 * @return shared pointer to the managed object
		 */
		std::shared_ptr<T> getInstancePointer(mint id) {
			checkID(id);
			return store[id];
		}

		/**
		 * Get symbol name that is used in the WL to represent Managed Expressions stored in this Store
		 * @return symbol name
		 */
		const std::string& getExpressionName() const noexcept {
			return expressionName;
		}

		/**
		 * Get the number of currently managed expressions.
		 * @return size of the store
		 */
		size_type size() const noexcept {
			return store.size();
		}

		iterator begin() noexcept {
			return store.begin();
		}

		const_iterator begin() const noexcept {
			return store.cbegin();
		}

		const_iterator cbegin() const noexcept {
			return store.cbegin();
		}

		iterator end() noexcept {
			return store.end();
		}

		const_iterator end() const noexcept {
			return store.end();
		}

		const_iterator cend() const noexcept {
			return store.cend();
		}

		/**
		 * Register class T as managed expression under given \p name.
		 * @param name - name of the Wolfram Language symbol that will be used to manage class T
		 * @param libData - optionally specify WolframLibraryData instance
		 * @note This function should typically be called in \c WolframLibrary_initialize
		 */
		void registerType(std::string name, WolframLibraryData libData = LibraryData::API()) noexcept {
			expressionName = std::move(name);
			libData->registerLibraryExpressionManager(expressionName.c_str(), manageInstanceCallback<T>);
		}

		/**
		 * Unregister class T as managed expression
		 * @param libData - optionally specify WolframLibraryData instance
		 * @note This function should typically be called in \c WolframLibrary_uninitialize
		 */
		void unregisterType(WolframLibraryData libData = LibraryData::API()) const noexcept {
			libData->unregisterLibraryExpressionManager(expressionName.c_str());
		}

	private:
		/**
		 * Helper function that checks whether given ID is present in the store and throws otherwise
		 * @param id - id to be checked
		 */
		void checkID(mint id) const {
			if (!hasInstance(id)) {
				ErrorManager::throwException(ErrorName::ManagedExprInvalidID);
			}
		}

	private:
		/// A map that associates IDs (mints) with pointers to objects of class T which are managed by WolframLanguage
		std::unordered_map<mint, std::shared_ptr<T>> store;

		/// Symbol name which is used in WolframLanguage to represent managed instances of class T
		std::string expressionName;
	};

}

#endif	  // LIBRARYLINKUTILITIES_MANAGEDEXPRESSION_HPP
