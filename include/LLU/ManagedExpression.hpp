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
	template<class T>
	void manageInstanceCallback(WolframLibraryData, mbool, mint) {
		static_assert(dependent_false_v<T>, "Use of unspecialized ManageInstance function.");
	}

	template<typename T>
	struct ManagedExpressionStore {
		void manageInstance(mbool mode, mint id) {
			if (mode == 0 /* create new instance */) {
				store[id] = nullptr;
			} else {
				store.erase(id);
			}
		}

		template<typename... Args>
		T& createInstance(mint id, Args &&... args) {
			checkID(id); // at this point instance must already exist in store
			store[id] = std::make_shared<T>(std::forward<Args>(args)...);
			return *store[id];
		}

		T& getInstance(mint id) {
			checkID(id);
			return *store[id];
		}

		std::shared_ptr<T> getInstancePointer(mint id) {
			checkID(id);
			return store[id];
		}

		void registerType(std::string name, WolframLibraryData libData = LibraryData::API()) {
			expressionName = std::move(name);
			libData->registerLibraryExpressionManager(expressionName.c_str(), manageInstanceCallback<T>);
		}

		void unregisterType(WolframLibraryData libData = LibraryData::API()) const {
			libData->unregisterLibraryExpressionManager(expressionName.c_str());
		}

	private:

		void checkID(mint id) const {
			if (store.count(id) == 0) {
				ErrorManager::throwException(ErrorName::FunctionError); // TODO better error
			}
		}

	private:
		std::unordered_map<mint, std::shared_ptr<T>> store;
		std::string expressionName;
	};

}

#endif //LIBRARYLINKUTILITIES_MANAGEDEXPRESSION_HPP
