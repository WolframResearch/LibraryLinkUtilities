/**
 * @file	Shared.hpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	February 05, 2019
 * @brief	Implementation of Shared passing mode.
 */
#ifndef LLUTILS_SHARED_HPP
#define LLUTILS_SHARED_HPP

#include "PassingPolicy.hpp"

namespace LLU {
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
				cleanup();
				setOwner(other.isOwner());
				other.setOwner(false);
				return *this;
			}

			~Shared() override = default;

			void cleanup() const noexcept override {
				if (isOwner()) {
					disown();
				}
			}

			void passAsResult(MArgument &res) const noexcept override {
				// If the MTensor is shared between the library and the Wolfram Language,
				// then automatic return does not change anything in the ownership of the MTensor.
				pass(res);
			}
		};

	}
}
#endif //LLUTILS_SHARED_HPP
