/**
 * @file	BitVector.h
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	March 11, 2024
 * @brief
 */
#ifndef LLU_CONTAINERS_BITVECTOR_H
#define LLU_CONTAINERS_BITVECTOR_H

#include "LLU/LibraryData.h"

namespace LLU {

	class BitVector {
	public:
		explicit BitVector(mint length, bool setQ = false);

		BitVector(const BitVector& other);

		BitVector& operator=(const BitVector& other);

		BitVector(BitVector&& other) noexcept;

		BitVector& operator=(BitVector&& other) noexcept;

		~BitVector();

		void clear(mint position);

		void set(mint position);

		void toggle(mint position);

		bool test(mint position) const;

		bitvector_t rawData();

		friend void swap(BitVector& lhs, BitVector& rhs) noexcept;

	private:
		bitvector_t bv;
	};

}
#endif	  // LLU_CONTAINERS_BITVECTOR_H
