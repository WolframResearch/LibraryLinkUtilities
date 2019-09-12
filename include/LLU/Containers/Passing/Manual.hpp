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

		/**
		 * @brief   Class representing Manual passing policy, owns the underlying container until passed as result of library function
		 * @see     <https://reference.wolfram.com/language/LibraryLink/tutorial/InteractionWithWolframLanguage.html#97446640>
		 */
		class Manual : public PassingPolicy {
		public:
			/// Default constructor, owns from beginning.
			Manual() : PassingPolicy(true) {}

			/// Create Manual object from any other passing policy
			explicit Manual(const PassingPolicy &) : PassingPolicy(true) {};

			/// Copy constructor. Deep copy of the resource will be created so we own it.
			Manual(const Manual&) : PassingPolicy(true) {};

			/**
			 * @brief   Move constructor. Moved-from object no longer owns the container.
			 * @param   other - rvalue reference to Manual object
			 */
			Manual(Manual&& other) noexcept : PassingPolicy(true) {
				other.setOwner(false);
			}

			/**
			 * @brief   Copy-assignment operator, assigns another Manual object cleaning up first
			 * @return  this
			 */
			Manual& operator=(const Manual&) noexcept {
				cleanup();
				setOwner(true);
				return *this;
			};

			/**
			 * @brief   Copy-assignment operator, assigns object of any PassingPolicy cleaning up first
			 * @return  this
			 */
			Manual& operator=(const PassingPolicy&) noexcept override {
				cleanup();
				setOwner(true);
				return *this;
			};

			/**
			 * @brief   Move-assignment operator, assigns another Manual object cleaning up first
			 * @param   other - another Manual object
			 * @return  this
			 */
			Manual& operator=(Manual&& other) noexcept {
				cleanup();
				setOwner(other.isOwner());
				other.setOwner(false);
				return *this;
			}

			/// Default destructor
			~Manual() override = default;

			/// For Manual containers cleanup means freeing the container
			void cleanup() const noexcept override {
                if (isOwner()) {
					free();
				}
			}
		};
	}
}
#endif //LLUTILS_MANUAL_HPP
