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

		/**
		 * @brief   Class representing Shared passing policy, never frees the underlying container only disowns
		 * @see     <https://reference.wolfram.com/language/LibraryLink/tutorial/InteractionWithWolframLanguage.html#97446640>
		 */
		class Shared : public PassingPolicy {
		public:
			/// Default constructor. It owns the container but the ownership may be shared.
			Shared() : PassingPolicy(true) {}

			/// Shared containers cannot be copied, because the copy would not be Shared any more (LibraryLink would not know about it)
			Shared(const Shared&) = delete;

			Shared(Shared&& other) noexcept : PassingPolicy(true) {
				other.setOwner(false);
			}

			/// Copy-assignment is disabled for the same reason as copy-constructor
			Shared& operator=(const Shared&) = delete;

			/**
			 * @brief   Move-assignment operator, assigns another Shared object cleaning up first
			 * @param   other - another Shared object
			 * @return  this
			 */
			Shared& operator=(Shared&& other) noexcept {
				cleanup();
				setOwner(other.isOwner());
				other.setOwner(false);
				return *this;
			}

			/// Default destructor
			~Shared() override = default;

			/// For Shared containers cleanup means disowning
			void cleanup() const noexcept override {
				if (isOwner()) {
					disown();
				}
			}

			/**
			 * @copydoc PassingPolicy::passAsResult()
			 * @details According to LibraryLink docs, if the container is shared between the library and the Wolfram Language,
			 * then automatic return does not change anything in the ownership of the container.
			 */
			void passAsResult(MArgument& res) const noexcept override {
				pass(res);
			}
		};

	}
}
#endif	  // LLUTILS_SHARED_HPP
