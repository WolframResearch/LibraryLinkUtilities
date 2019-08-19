/**
 * @file	Automatic.hpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	September 02, 2018
 * @brief	Implementation of Automatic passing mode.
 */

#ifndef LLUTILS_AUTOMATIC_HPP
#define LLUTILS_AUTOMATIC_HPP

#include "PassingPolicy.hpp"

namespace LLU {
	namespace Passing {

		class Automatic : public PassingPolicy {
		public:
			Automatic() : PassingPolicy(false) {}

			Automatic(const Automatic&) = delete;

			Automatic(Automatic&&) noexcept = default;

			Automatic& operator=(const Automatic&) = delete;

			Automatic& operator=(Automatic&&) noexcept = default;

			~Automatic() override = default;

			void cleanup() const noexcept override {
				// Automatic never owns the container so no cleanup
			}
		};

		/// We don't have a separate class for Constant passing because from resource management point of view it is the same as Automatic.
		/// We only define an alias for convenience.
		using Constant = Automatic;
	}
}
#endif //LLUTILS_AUTOMATIC_HPP
