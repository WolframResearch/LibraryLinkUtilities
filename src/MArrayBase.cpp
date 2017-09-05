/*
 * MArrayBase.cpp
 *
 *  Created on: 04.09.2017
 *      Author: RaV
 */

#include "MArrayBase.h"

#include <algorithm>
#include <functional>
#include <iterator>
#include <limits>
#include <numeric>

namespace LibraryLinkUtils {

	WolframLibraryData MArrayBase::libData = nullptr;
	WolframRawArrayLibrary_Functions MArrayBase::raFuns = nullptr;
	WolframImageLibrary_Functions MArrayBase::imgFuns = nullptr;

	MArrayBase::MArrayBase(std::initializer_list<mint> dimensions) {
		if (!libData || !raFuns || !imgFuns)
			initError();
		depth = checkContainerSize(dimensions);
		dims = dimensions;
		flattenedLength = totalLengthFromDims();
		fillOffsets();
	}



	void MArrayBase::fillOffsets() {
		offsets.assign(rank(), 1);
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
		if (v.size() > std::numeric_limits<mint>::max())
			sizeError();
		return static_cast<mint>(v.size());
	}

	mint MArrayBase::totalLengthFromDims() const noexcept {
		return std::accumulate(std::begin(dims), std::end(dims), static_cast<mint>(1), std::multiplies<mint>());
	}

} /* namespace LibraryLinkUtils */
