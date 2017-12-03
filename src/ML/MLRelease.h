/** 
 * @file	MLRelease.h
 * @date	Nov 28, 2017
 * @author	rafal
 * @brief	<brief description>
 */
#ifndef MLRELEASE_H_
#define MLRELEASE_H_

#include <functional>

#include "mathlink.h"

namespace LibraryLinkUtils {

	namespace ML {

		template<typename T>
		struct ReleaseList {
			using Func = std::function<void(MLINK, T*, int)>;

			ReleaseList() = default;
			ReleaseList(MLINK m, int l) : m(m), length(l) {}

			void operator()(T* data) {
				Release(m, data, length);
			}

			int getListLength() const {
				return length;
			}

		private:
			static Func Release;

			MLINK m = nullptr;
			int length = 0;
		};


		template<typename T>
		struct ReleaseString {
			using Func = std::function<void(MLINK, const T*, int)>;

			ReleaseString() = default;
			ReleaseString(MLINK m, int l, int c) : m(m), length(l), chars(c) {}

			void operator()(const T* data) {
				Release(m, data, length);
			}

			int getLength() const {
				return length;
			}

			int getCharacters() const {
				return chars;
			}
		private:
			static Func Release;

			MLINK m = nullptr;
			int length = 0;
			int chars = 0;
		};

		template<typename T>
		struct ReleaseArray {
			using Func = std::function<void(MLINK, T*, int*, char**, int)>;

			ReleaseArray() = default;
			ReleaseArray(MLINK m, int* d, char** h, int r) : m(m), dims(d), heads(h), rank(r) {}

			void operator()(T* data) {
				Release(m, data, dims, heads, rank);
			}

			int* getDims() const {
				return dims;
			}

			char** getHeads() const {
				return heads;
			}

			int getRank() const {
				return rank;
			}
		private:
			static Func Release;

			MLINK m = nullptr;
			int* dims = nullptr;
			char** heads = nullptr;
			int rank = 0;
		};

	}
} /* namespace LibraryLinkUtils */

#endif /* MLRELEASE_H_ */
