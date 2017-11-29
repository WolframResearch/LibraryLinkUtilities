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

			ReleaseList(MLINK m, int l) : m(m), length(l) {}

			void operator()(T* data) {
				Release(m, data, length);
			}

			int getListLength() const {
				return length;
			}

		private:
			static Func Release;

			MLINK m;
			int length;
		};


		template<typename T>
		struct ReleaseString {
			using Func = std::function<void(MLINK, const T*, int)>;

			ReleaseString(MLINK m, int l, int c) : m(m), length(l), chars(c) {}

			void operator()(const T* data) {
				Release(m, data, length);
			}

			int getStringLength() const {
				return length;
			}

			int getCharacters() const {
				return chars;
			}
		private:
			static Func Release;

			MLINK m;
			int length;
			int chars;
		};

	}
} /* namespace LibraryLinkUtils */

#endif /* MLRELEASE_H_ */
