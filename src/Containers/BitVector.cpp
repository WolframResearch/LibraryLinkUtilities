/**
 * @file	BitVector.cpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	March 11, 2024
 * @brief
 */

#include "LLU/Containers/BitVector.h"

namespace LLU {
	BitVector::BitVector(mint length, bool setQ) {
		const auto error = LibraryData::DataVectorAPI()->BitVector_new(length, setQ, &bv);
		if (error) {
			ErrorManager::throwException(ErrorName::BitVectorNew, error);
		}
	}

	BitVector::BitVector(const std::vector<bool>& vector) : BitVector(vector.size(), false) {
		for (std::size_t i = 0; i < vector.size(); ++i) {
			if (vector[i]) {
				set(static_cast<mint>(i));
			}
		}
	}

	BitVector::BitVector(bitvector_t raw, Ownership owner) : bv{raw}, owner{owner} {
	}

	BitVector::BitVector(const BitVector& other) {
		const auto error = LibraryData::DataVectorAPI()->BitVector_clone(other.bv, &bv);
		if (error) {
			ErrorManager::throwException(ErrorName::BitVectorClone, error);
		}
	}

	BitVector::BitVector(BitVector&& other) noexcept : bv{other.bv}, owner{other.owner} {
		other.bv = nullptr;
	}

	BitVector& BitVector::operator=(const BitVector& other) {
		BitVector tmp {other};
		swap(*this, tmp);
		return *this;
	}

	BitVector& BitVector::operator=(BitVector&& other) noexcept {
		BitVector tmp {std::move(other)};
		swap(*this, tmp);
		return *this;
	}

	BitVector::~BitVector() {
		if (bv && owner != Ownership::LibraryLink) {
			LibraryData::DataVectorAPI()->BitVector_release(bv);
		}
	}

	void BitVector::clear(mint position) {
		LibraryData::DataVectorAPI()->BitVector_clear(bv, position);
	}

	void BitVector::set(mint position) {
		LibraryData::DataVectorAPI()->BitVector_set(bv, position);
	}

	void BitVector::toggle(mint position) {
		LibraryData::DataVectorAPI()->BitVector_toggle(bv, position);
	}

	bool BitVector::test(mint position) const {
		return LibraryData::DataVectorAPI()->BitVector_test(bv, position);
	}

	bitvector_t BitVector::rawData() const {
		return bv;
	}

	void swap(BitVector& lhs, BitVector& rhs) noexcept {
		using std::swap;
		swap(lhs.bv, rhs.bv);
		swap(lhs.owner, rhs.owner);
	}
}