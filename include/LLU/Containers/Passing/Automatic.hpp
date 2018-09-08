/**
 * @file	Automatic.hpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	September 02, 2018
 * @brief	Implementation of Automatic passing mode.
 */

#ifndef LLUTILS_AUTOMATIC_HPP
#define LLUTILS_AUTOMATIC_HPP

#include "PassingPolicy.hpp"

namespace LibraryLinkUtils::Passing {

	template<typename LLContainer>
	class Automatic : public PassingPolicy<LLContainer> {
	public:
		using Super = PassingPolicy<LLContainer>;
	public:
		explicit Automatic(LLContainer newCont) : Super(newCont, false) {}

		Automatic(const Automatic&) = delete;

		Automatic(Automatic&&) noexcept = default;

		Automatic& operator=(const Automatic&) = delete;

		Automatic& operator=(Automatic&&) noexcept = default;

		~Automatic() = default;

		void passAsResult(MArgument& res) const noexcept override {
			this->passInternal(res);
			this->setOwner(false);
		}
	};

}
#endif //LLUTILS_AUTOMATIC_HPP
