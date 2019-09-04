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

#include "WolframLibrary.h"

#include <LLU/LibraryData.h>
#include <LLU/Utilities.hpp>

/**
 * Use this macro in place of the \b class keyword when defining a "managed" class.
 * It will declare the class, define an instance of ManagedExpressionStore corresponding to your class
 * and it will define a template specialization of manageInstanceCallback for your class.
 */
#define MANAGED_EXPRESSION_CLASS(ClassName) \
	class ClassName; \
	\
	LLU::ManagedExpressionStore<ClassName> ClassName##Store; \
	\
	template<> void LLU::manageInstanceCallback<ClassName>(WolframLibraryData, mbool mode, mint id) {\
		ClassName##Store.manageInstance(mode, id);\
	}\
	\
	class ClassName

/// Same as \c MANAGED_EXPRESSION_CLASS only for structures
#define MANAGED_EXPRESSION_STRUCT(StructName) \
    struct StructName; \
    \
    LLU::ManagedExpressionStore<StructName> StructName##Store; \
    \
    template<> void LLU::manageInstanceCallback<StructName>(WolframLibraryData, mbool mode, mint id) {\
        StructName##Store.manageInstance(mode, id);\
    }\
    \
    struct StructName

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
	struct ManagedExpressionStore {
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
		 * @tparam Args - constructor arguments types
		 * @param id - id of the newly created managed object
		 * @param args - constructor arguments
		 * @return reference to the newly created object
		 */
		template<typename... Args>
		T& createInstance(mint id, Args&&... args) {
			checkID(id); // at this point instance must already exist in store
			store[id] = std::make_shared<T>(std::forward<Args>(args)...);
			return *store[id];
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
		const std::string& getExpressionName() const {
			return expressionName;
		}

		/**
		 * Register class T as managed expression under given \p name.
		 * @param name - name of the Wolfram Language symbol that will be used to manage class T
		 * @param libData - optionally specify WolframLibraryData instance
		 * @note This function should typically be called in \c WolframLibrary_initialize
		 */
		void registerType(std::string name, WolframLibraryData libData = LibraryData::API()) {
			expressionName = std::move(name);
			libData->registerLibraryExpressionManager(expressionName.c_str(), manageInstanceCallback<T>);
		}

		/**
		 * Unregister class T as managed expression
		 * @param libData - optionally specify WolframLibraryData instance
		 * @note This function should typically be called in \c WolframLibrary_uninitialize
		 */
		void unregisterType(WolframLibraryData libData = LibraryData::API()) const {
			libData->unregisterLibraryExpressionManager(expressionName.c_str());
		}

	private:

		/**
		 * Helper function that checks whether given ID is present in the store and throws otherwise
		 * @param id - id to be checked
		 */
		void checkID(mint id) const {
			if (store.count(id) == 0) {
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

#endif //LIBRARYLINKUTILITIES_MANAGEDEXPRESSION_HPP
