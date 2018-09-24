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

		template<typename LLContainer>
		class Manual : public PassingPolicy<LLContainer> {
		public:
			using Super = PassingPolicy<LLContainer>;
		public:
			explicit Manual(LLContainer newCont) : Super(newCont, true) {}

			Manual(const Manual& other) : Super(other.cloneInternal(), true) {};

			Manual(Manual&&) noexcept = default;

			Manual& operator=(const Manual&) {
				this->setOwner(true);
				return *this;
			};

			Manual& operator=(Manual&&) noexcept = default;

			~Manual() {
				if (this->isOwner()) {
					this->freeInternal();
				}
			}

			void passAsResult(MArgument& res) const noexcept override {
				this->passInternal(res);
				this->setOwner(false);
			}
		};
	}
}
#endif //LLUTILS_MANUAL_HPP
