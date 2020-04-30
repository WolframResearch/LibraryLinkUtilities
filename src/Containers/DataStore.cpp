/**
 * @file	DataStore.cpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	April 28, 2020
 * @brief
 */

#include "LLU/Containers/Generic/DataStore.hpp"

namespace LLU {

	MContainer<MArgumentType::DataStore>::MContainer(Container c, Ownership owner) : MContainerBase {c, owner} {
		if (owner == Ownership::Shared) {
			ErrorManager::throwException(ErrorName::DLSharedDataStore);
		}
	}

	void MContainer<MArgumentType::DataStore>::push_back(const std::string& name, TypedArgument node) {
		switch (static_cast<MArgumentType>(node.index())) {
			case MArgumentType::MArgument: ErrorManager::throwException(ErrorName::FunctionError);	   // TODO add specific error
			case MArgumentType::Boolean: Argument<MArgumentType::Boolean>::addDataStoreNode(getContainer(), name, *std::get_if<bool>(&node)); break;
			case MArgumentType::Integer: Argument<MArgumentType::Integer>::addDataStoreNode(getContainer(), name, *std::get_if<mint>(&node)); break;
			case MArgumentType::Real: Argument<MArgumentType::Real>::addDataStoreNode(getContainer(), name, *std::get_if<double>(&node)); break;
			case MArgumentType::Complex: {
				auto c = *std::get_if<std::complex<double>>(&node);
				mcomplex mc {c.real(), c.imag()};
				Argument<MArgumentType::Complex>::addDataStoreNode(getContainer(), name, mc);
			} break;
			case MArgumentType::Tensor:
				Argument<MArgumentType::Tensor>::addDataStoreNode(getContainer(), name, std::get_if<GenericTensor>(&node)->abandonContainer());
				break;
			case MArgumentType::SparseArray:
				Argument<MArgumentType::SparseArray>::addDataStoreNode(getContainer(), name, *std::get_if<MSparseArray>(&node));
				break;
			case MArgumentType::NumericArray:
				Argument<MArgumentType::NumericArray>::addDataStoreNode(getContainer(), name, std::get_if<GenericNumericArray>(&node)->abandonContainer());
				break;
			case MArgumentType::Image:
				Argument<MArgumentType::Image>::addDataStoreNode(getContainer(), name, std::get_if<GenericImage>(&node)->abandonContainer());
				break;
			case MArgumentType::UTF8String: {
				auto* data = std::get_if<std::string_view>(&node)->data();
				Argument<MArgumentType::UTF8String>::addDataStoreNode(getContainer(), name, const_cast<char*>(data));
			} break;
			case MArgumentType::DataStore:
				Argument<MArgumentType::DataStore>::addDataStoreNode(getContainer(), name, std::get_if<GenericDataList>(&node)->abandonContainer());
				break;
		}
	}
}