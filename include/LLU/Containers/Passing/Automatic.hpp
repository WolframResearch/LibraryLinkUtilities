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

		/**
		 * @brief   Class representing Automatic passing policy, never owns the underlying container
		 * @see     <https://reference.wolfram.com/language/LibraryLink/tutorial/InteractionWithWolframLanguage.html#97446640>
		 */
		class Automatic : public PassingPolicy {
		public:
			/// Default constructor. Do not own the underlying container
			Automatic() : PassingPolicy(false) {}

			/// Automatic containers cannot be copied, because the copy would not be Automatic any more (LibraryLink would not know about it)
			Automatic(const Automatic&) = delete;

			Automatic(Automatic&&) noexcept = default;

			/// Copy-assignment is disabled for the same reason as copy-constructor
			Automatic& operator=(const Automatic&) = delete;

			Automatic& operator=(Automatic&&) noexcept = default;

			/// Default destructor
			~Automatic() override = default;

			/// Cleanup routine does nothing for Automatic
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
