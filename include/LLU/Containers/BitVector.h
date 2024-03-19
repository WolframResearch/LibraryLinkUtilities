/**
 * @file	BitVector.h
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	March 11, 2024
 * @brief
 */
#ifndef LLU_CONTAINERS_BITVECTOR_H
#define LLU_CONTAINERS_BITVECTOR_H

#include <vector>

#include "LLU/Containers/Generic/Base.hpp"
#include "LLU/LibraryData.h"

namespace LLU {

	class BitVector {
	public:
		BitVector() = default;

		explicit BitVector(mint length, bool setQ = false);

		explicit BitVector(const std::vector<bool>& vector);

		BitVector(bitvector_t raw, Ownership owner = Ownership::LibraryLink);

		BitVector(const BitVector& other);

		BitVector& operator=(const BitVector& other);

		BitVector(BitVector&& other) noexcept;

		BitVector& operator=(BitVector&& other) noexcept;

		~BitVector();

		void clear(mint position);

		void set(mint position);

		void toggle(mint position);

		bool test(mint position) const;

		bitvector_t rawData() const;

		friend void swap(BitVector& lhs, BitVector& rhs) noexcept;

	private:
		bitvector_t bv = nullptr;
		Ownership owner = Ownership::Library;
	};

}
#endif	  // LLU_CONTAINERS_BITVECTOR_H
