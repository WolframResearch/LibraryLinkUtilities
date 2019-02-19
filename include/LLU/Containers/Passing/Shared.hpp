/**
 * @file	Shared.hpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	February 05, 2019
 * @brief	Implementation of Shared passing mode.
 */
#ifndef LLUTILS_SHARED_HPP
#define LLUTILS_SHARED_HPP

#include "PassingPolicy.hpp"

namespace LibraryLinkUtils {
	namespace Passing {

		class Shared : public PassingPolicy {
		public:
			Shared() : PassingPolicy(true) {}

			Shared(const Shared&) = delete;

			Shared(Shared&& other) noexcept : PassingPolicy(true) {
				other.setOwner(false);
			}

			Shared& operator=(const Shared&) = delete;

			Shared& operator=(Shared&& other) noexcept {
				setOwner(true);
				other.setOwner(false);
				return *this;
			}

			~Shared() = default;

			void cleanup() const noexcept override {
				if (isOwner()) {
					disown();
				}
			}
		};

	}
}
#endif //LLUTILS_SHARED_HPP
