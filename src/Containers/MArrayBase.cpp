/**
 * @file	MArrayBase.cpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	4/09/2017
 *
 * @brief	Definitions of non-template member functions and static data members from MArrayBase class
 *
 */

#include "LLU/Containers/MArrayBase.h"

#include <algorithm>
#include <functional>
#include <iterator>
#include <limits>
#include <numeric>

namespace LibraryLinkUtils {

	MArrayBase::MArrayBase(std::initializer_list<mint> dimensions) {
		if (!libData || !naFuns || !imgFuns)
			initError();
		depth = checkContainerSize(dimensions);
		dims = dimensions;
		flattenedLength = totalLengthFromDims();
		fillOffsets();
	}



	void MArrayBase::fillOffsets() {
		offsets.assign(static_cast<decltype(offsets)::size_type>(rank()), 1);
		if (rank() >= 2) {
			std::transform(std::rbegin(offsets), std::rend(offsets) - 1, std::crbegin(dims), std::rbegin(offsets) + 1, [](auto off, auto dim) {
				return off * dim;
			});
		}
	}


	mint MArrayBase::getIndex(const std::vector<mint>& indices) const {
		if (indices.size() != static_cast<std::make_unsigned_t<mint>>(rank()))
			indexError();
		mint flatIndex = 0;
		auto dimsIt = dims.cbegin();
		auto offset = offsets.cbegin();
		for (auto idx : indices) {
			if (idx < 0 || idx >= *dimsIt++)
				indexError();
			flatIndex += idx * (*offset++);
		}
		return flatIndex;
	}


	mint MArrayBase::checkContainerSize(std::initializer_list<mint> v) const {
		if (v.size() > static_cast<decltype(v)::size_type>(std::numeric_limits<mint>::max()))
			sizeError();
		return static_cast<mint>(v.size());
	}

	mint MArrayBase::totalLengthFromDims() const noexcept {
		return std::accumulate(std::begin(dims), std::end(dims), static_cast<mint>(1), std::multiplies<mint>());
	}


	bool MArrayBase::isOwner() const {
		return arrayOwnerQ;
	}

	void MArrayBase::setOwner(bool ownerQ) {
		this->arrayOwnerQ = ownerQ;
	}

} /* namespace LibraryLinkUtils */
