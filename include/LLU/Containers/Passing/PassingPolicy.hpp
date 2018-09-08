/**
 * @file	PassingPolicy.hpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	September 02, 2018
 * @brief	Abstract class describing a passing mode used in LibraryLink. PassingPolicy handles LibraryLink container's lifetime, copying, moving, etc.
 */
#ifndef LLUTILS_PASSINGPOLICY_HPP
#define LLUTILS_PASSINGPOLICY_HPP

#include "WolframLibrary.h"

namespace LibraryLinkUtils::Passing {

	template<typename LLContainer>
	class PassingPolicy {
	public:
		PassingPolicy(LLContainer newCont, bool ownerQ) : internalContainer(newCont), argumentOwnerQ(ownerQ) {}

		PassingPolicy(const PassingPolicy&) = default;
		PassingPolicy(PassingPolicy&&) noexcept = default;
		PassingPolicy& operator=(const PassingPolicy&) = default;
		PassingPolicy& operator=(PassingPolicy&&) noexcept = default;
		virtual ~PassingPolicy() = default;

		LLContainer disownInternal() const {
			setOwner(false);
			return getInternal();
		}

		LLContainer getInternal() const {
			return internalContainer;
		};

		/**
		 *	@brief 		Pass the container as a result to LibraryLink via MArgument
		 *	@param[out]	res - MArgument that will carry the internal container
		 **/
		virtual void passAsResult(MArgument& res) const noexcept = 0;

		/**
		 *	@brief 		Pass the container as a result to LibraryLink via a shared MArgument
		 *	@param[out]	res - MArgument that will carry the internal container
		 **/
//		virtual void passAsSharedResult(MArgument& res) noexcept = 0;

		/**
		 * 	@brief		Check whether this object owns the underlying data structure from WolframLibrary. If it does, it is responsible for freeing the resources.
		 * 	@return		true if and only if the object owns the underlying data structure from WolframLibrary
		 */
		bool isOwner() const {
			return argumentOwnerQ;
		}

		/**
		 * 	@brief		Set the ownership of the underlying data structure from WolframLibrary.
		 * 	@param 		argumentOwnerQ - whether the object is now the owner of the underlying data structure from WolframLibrary
		 */
		void setOwner(bool ownerQ) const {
			argumentOwnerQ = ownerQ;
		}

	protected:
		/**
		 *   @brief 	Free internal container
		 **/
		virtual void freeInternal() noexcept {};

		/**
		 *   @brief 		Set internal container as result for LibraryLink.
		 *   @param[out]	res - MArgument that will carry the internal container
		 **/
		virtual void passInternal(MArgument& res) const noexcept = 0;

		virtual LLContainer cloneInternal() const = 0;

	private:
		LLContainer internalContainer {};

		/// Determines if MArray should free the underlying container
		mutable bool argumentOwnerQ = false;

	};
}

#endif //LLUTILS_PASSINGPOLICY_HPP
