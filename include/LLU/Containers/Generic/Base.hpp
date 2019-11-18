/**
 * @file	Base.hpp
 * @brief   Definitions of MContainerBase and MContainer class templates.
 */

#ifndef LLU_CONTAINERS_GENERIC_BASE_HPP
#define LLU_CONTAINERS_GENERIC_BASE_HPP

#include <type_traits>

#include "LLU/Containers/Passing/Manual.hpp"
#include "LLU/ErrorLog/ErrorManager.h"
#include "LLU/LibraryData.h"
#include "LLU/MArgument.h"
#include "LLU/Utilities.hpp"

namespace LLU {

	/**
	 * @brief Template of the base class for all generic containers.
	 * MContainerBase stores the raw LibraryLink container and defines a common interface for all generic containers.
	 *
	 * @tparam Type - container type
	 * @tparam PassingMode - passing mode (Automatic, Manual, etc.)
	 */
	template<MArgumentType Type, class PassingMode>
	class MContainerBase : public PassingMode {
	public:
		using Container = MType_t<Type>;

	public:
		/**
		 * @brief Default constructor.
		 * @details Triggers compile-time error unless PassingMode is Manual. Wrappers with all other passing modes can only be constructed given existing
		 * raw containers, so they cannot be empty.
		 */
		MContainerBase() {
			static_assert(std::is_same<PassingMode, Passing::Manual>::value, "New MContainer can only be created with passing mode Manual.");
		}

		/**
		 * @brief Create MContainerBase from a raw container
		 * @param c - raw LibraryLink container (MTensor, MNumericArray, etc.), passing a nullptr will trigger an exception
		 */
		/* implicit */ MContainerBase(Container c) : container(c) {
			if (!c) {
				ErrorManager::throwException(ErrorName::CreateFromNullError);
			}
		}

		/**
		 * @brief Create MContainerBase from an MContainerBase of the same \c Type but different passing policy.
		 * This will perform a deep copy of the underlying raw container.
		 * @tparam P - passing policy
		 * @param mc - MContainerBase that will be copied
		 */
		template<class P>
		explicit MContainerBase(const MContainerBase<Type, P>& mc) : PassingMode(mc), container(mc.clone()) {}

		/**
		 * @brief Copy-constructor, performs a deep copy of the raw container.
		 * @param mc - MContainerBase to be copied
		 */
		MContainerBase(const MContainerBase& mc) : PassingMode(mc), container(mc.clone()) {}

		/**
		 * @brief Move-constructor
		 * @param mc - MContainerBase to be moved-from, it's internal container becomes nullptr
		 */
		MContainerBase(MContainerBase&& mc) noexcept : PassingMode(std::move(mc)), container(mc.container) {
			mc.container = nullptr;
		}

		/**
		 * @brief Copy-assignment operator, performs a deep copy of the raw container.
		 * @param mc - MContainerBase to be copied
		 * @return reference to this object
		 */
		MContainerBase& operator=(const MContainerBase& mc) {
			PassingMode::operator=(mc);
			setContainer(mc.clone());
			return *this;
		}

		/**
		 * @brief Move-assignment operator
		 * @param mc - MContainerBase to be moved-from, it's internal container becomes nullptr
		 * @return reference to this object
		 */
		MContainerBase& operator=(MContainerBase&& mc) noexcept {
			PassingMode::operator=(std::move(mc));
			setContainer(mc.container);
			mc.container = nullptr;
			return *this;
		}

		/**
		 * @brief Copy-assignment with passing mode conversion
		 * @tparam P - passing mode of the MContainerBase to be copied
		 * @param mc - MContainerBase to be copied
		 * @return reference to this object which is now a deep copy of the input MContainerBase
		 */
		template<class P>
		MContainerBase& operator=(const MContainerBase<Type, P>& mc) {
			PassingMode::operator=(mc);
			setContainer(mc.clone());
			return *this;
		}

		/// Default destructor
		~MContainerBase() override = default;

		/**
		 * @brief Clone the raw container, if it's present
		 * @return cloned container or nullptr if there is no internal container
		 */
		Container clone() const {
			if (container == nullptr) {
				return nullptr;
			}
			return cloneImpl();
		}

		/**
		 * @brief Get internal container
		 * @return a handle to the internal container
		 */
		Container getContainer() const noexcept {
			return container;
		}

		/**
		 * @brief Give a handle to internal container and stop owning it.
		 * Should be used with caution as it may potentially result with resource leak.
		 *
		 * @return a handle to the internal container
		 */
		Container abandonContainer() const noexcept {
			this->setOwner(false);
			return container;
		}

		/**
		 *   @brief Return share count of internal container.
		 **/
		mint shareCount() const noexcept {
			if (container) {
				return shareCountImpl();
			}
			return 0;
		}

		/**
		 * @brief   Pass the internal container as result of a LibraryLink function.
		 * @param   res - MArgument which will hold internal container of this MContainerBase
		 */
		void pass(MArgument& res) const override {
			if (container) {
				passImpl(res);
			}
		}

	protected:
		/// Disown internal container if present
		void disown() const noexcept override {
			if (container) {
				disownImpl();
			}
		}

		/// Free internal container if present
		void free() const noexcept override {
			if (container) {
				freeImpl();
			}
		}

		/**
		 * @brief   Set new internal container. The ownership state of this wrapper is not changed
		 * @param   newCont - new internal container
		 */
		void setContainer(Container newCont) noexcept {
			container = newCont;
		}

	private:
		virtual Container cloneImpl() const = 0;

		virtual void freeImpl() const = 0;

		virtual void disownImpl() const = 0;

		virtual mint shareCountImpl() const = 0;

		virtual void passImpl(MArgument& res) const = 0;

		/// Raw LibraryLink container (MTensor, MImage, DataStore, etc.)
		Container container {};
	};

	/**
	 * @class   MContainer
	 * @brief   MContainer is an abstract class template for generic containers. Only specializations shall be used.
	 * @tparam  Type - container type (see MArgumentType definition)
	 * @tparam  PassingMode - passing policy (Shared, Manual, etc.)
	 */
	template<MArgumentType Type, class PassingMode>
#ifdef _WIN32
	class MContainer;	 // On Windows we cannot provide a body with static_assert because of ridiculous MSVC compiler errors (probably a bug).
#else					 // On other platforms we get a nice, compile-time error.
	class MContainer {
		static_assert(dependent_false_v<PassingMode>, "Trying to instantiate unspecialized MContainer template.");
	};
#endif
}

#endif	  // LLU_CONTAINERS_GENERIC_BASE_HPP
