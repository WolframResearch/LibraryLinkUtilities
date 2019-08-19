/**
 * @file	Manual.hpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	September 02, 2018
 * @brief	Implementation of Manual passing mode.
 */

#ifndef LLUTILS_MANUAL_HPP
#define LLUTILS_MANUAL_HPP

#include "PassingPolicy.hpp"

namespace LLU {
	namespace Passing {

		class Manual : public PassingPolicy {
		public:
			Manual() : PassingPolicy(true) {}

			explicit Manual(const PassingPolicy &) : PassingPolicy(true) {};

			Manual(const Manual&) : PassingPolicy(true) {};

			Manual(Manual&& other) noexcept : PassingPolicy(true) {
				other.setOwner(false);
			}

			Manual& operator=(const Manual&) noexcept {
				cleanup();
				setOwner(true);
				return *this;
			};

			Manual& operator=(const PassingPolicy&) noexcept override {
				cleanup();
				setOwner(true);
				return *this;
			};

			Manual& operator=(Manual&& other) noexcept {
				cleanup();
				setOwner(other.isOwner());
				other.setOwner(false);
				return *this;
			}

			~Manual() override = default;

			void cleanup() const noexcept override {
                if (isOwner()) {
					free();
				}
			}
		};
	}
}
#endif //LLUTILS_MANUAL_HPP
