/**
 * @file	PassingPolicy.hpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	September 02, 2018
 * @brief	Abstract class describing a passing mode used in LibraryLink. PassingPolicy handles LibraryLink container's lifetime, copying, moving, etc.
 */
#ifndef LLUTILS_PASSINGPOLICY_HPP
#define LLUTILS_PASSINGPOLICY_HPP

#include "WolframLibrary.h"

namespace LLU {
	/// Namespace containing classes representing different passing modes that exist in LibraryLink
	namespace Passing {

		/// Abstract class defining common interface and behavior for all predefined     passing policies
		class PassingPolicy {
		public:

			/**
			 * 	@brief		Check whether this object owns the underlying data structure from WolframLibrary. If it does, it is responsible for freeing the resources.
			 * 	@return		true if and only if the object owns the underlying data structure from WolframLibrary
			 */
			bool isOwner() const {
				return argumentOwnerQ;
			}

			/**
			 *	@brief 		Pass the container as a result to LibraryLink via MArgument
			 *	@param[out]	res - MArgument that will carry the internal container
			 **/
			virtual void passAsResult(MArgument& res) const noexcept {
				pass(res);
				setOwner(false);
			}

			virtual void cleanup() const noexcept = 0;

		protected:

			explicit PassingPolicy(bool ownerQ) : argumentOwnerQ(ownerQ) {}

			PassingPolicy() = default;
			PassingPolicy(const PassingPolicy&) = default;
			PassingPolicy(PassingPolicy&&) noexcept = default;

			virtual PassingPolicy& operator=(const PassingPolicy&) = default;
			PassingPolicy& operator=(PassingPolicy&&) noexcept = default;
			virtual ~PassingPolicy() = default;

			/**
			 * 	@brief		Set the ownership of the managed data structure
			 * 	@param 		ownerQ - whether the object is now the owner of the underlying data structure from WolframLibrary
			 */
			void setOwner(bool ownerQ) const {
				argumentOwnerQ = ownerQ;
			}

        protected:
            virtual void free() const noexcept = 0;

			virtual void pass(MArgument& res) const = 0;

            virtual void disown() const noexcept = 0;

        private:
			/// Determines if MArray should free the underlying container
			mutable bool argumentOwnerQ = false;
		};
	}
}

#endif //LLUTILS_PASSINGPOLICY_HPP
