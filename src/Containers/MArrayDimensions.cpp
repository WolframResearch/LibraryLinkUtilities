/**
 * @file	MArrayBase.cpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	4/09/2017
 *
 * @brief	Definitions of non-template member functions and static data members from MArrayBase class
 *
 */

#include "LLU/Containers/MArrayDimensions.h"

#include <algorithm>
#include <limits>
#include <numeric>

namespace LLU {

	MArrayDimensions::MArrayDimensions(std::initializer_list<mint> dimensions) {
		dims = dimensions;
		checkContainerSize(dimensions);
		flattenedLength = totalLengthFromDims();
		fillOffsets();
	}

	void MArrayDimensions::fillOffsets() {
		offsets.assign(dims.size(), 1);
		if (rank() >= 2) {
			std::transform(std::rbegin(offsets), std::rend(offsets) - 1, std::crbegin(dims), std::rbegin(offsets) + 1, [](auto off, auto dim) {
				return off * dim;
			});
		}
	}

	mint MArrayDimensions::getIndexChecked(const std::vector<mint>& indices) const {
		if (indices.size() != dims.size()) {
			indexError();
		}
		auto dimsIt = dims.cbegin();
		for (auto idx : indices) {
			if (idx < 0 || idx >= *dimsIt++) {
				indexError();
			}
		}
		return getIndex(indices);
	}

	mint MArrayDimensions::getIndexChecked(mint index) const {
		if (index < 0 || index >= flatCount()) {
			indexError();
		}
		return index;
	}

	mint MArrayDimensions::getIndex(const std::vector<mint>& indices) const {
		mint flatIndex = 0;
		auto offset = offsets.cbegin();
		for (auto idx : indices) {
			flatIndex += idx * (*offset++);
		}
		return flatIndex;
	}


	mint MArrayDimensions::checkContainerSize(std::initializer_list<mint> v) const {
		if (v.size() <= 0 || v.size() > static_cast<decltype(v)::size_type>(std::numeric_limits<mint>::max())) {
			ErrorManager::throwException(ErrorName::DimensionsError);
		}
		return static_cast<mint>(v.size());
	}

	mint MArrayDimensions::totalLengthFromDims() const noexcept {
		return std::accumulate(std::begin(dims), std::end(dims), static_cast<mint>(1), std::multiplies<>());
	}

	void MArrayDimensions::indexError() const {
		// TODO throw better error
		ErrorManager::throwException(ErrorName::DimensionsError);
	}


} /* namespace LLU */
