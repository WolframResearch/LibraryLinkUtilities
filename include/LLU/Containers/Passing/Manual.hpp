/**
 * @file	Manual.hpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	September 02, 2018
 * @brief	Implementation of Manual passing mode.
 */

#ifndef LLUTILS_MANUAL_HPP
#define LLUTILS_MANUAL_HPP

#include "PassingPolicy.hpp"

namespace LibraryLinkUtils {
	namespace Passing {

		class Manual : public PassingPolicy {
		public:
			Manual() : PassingPolicy(true) {}

			Manual(const PassingPolicy&) : PassingPolicy(true) {};

			Manual(Manual&& other) noexcept : PassingPolicy(true) {
				other.setOwner(false);
			}

			Manual& operator=(const PassingPolicy&) noexcept override {
				setOwner(true);
				return *this;
			};

			Manual& operator=(Manual&& other) noexcept {
				setOwner(true);
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
