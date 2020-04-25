/**
 * @file	Base.hpp
 * @brief   Definitions of MContainerBase and MContainer class templates.
 */

#ifndef LLU_CONTAINERS_GENERIC_BASE_HPP
#define LLU_CONTAINERS_GENERIC_BASE_HPP

#include <type_traits>

#include "LLU/ErrorLog/ErrorManager.h"
#include "LLU/LibraryData.h"
#include "LLU/MArgument.h"
#include "LLU/Utilities.hpp"

#include "LLU/Containers/Interfaces.h"

namespace LLU {

	enum struct Ownership : uint8_t {
		LibraryLink,
		Library,
		Shared
	};

	/**
	 * @brief Template of the base class for all generic containers.
	 * MContainerBase stores the raw LibraryLink container and defines a common interface for all generic containers.
	 *
	 * @tparam Type - container type
	 * @tparam PassingMode - passing mode (Automatic, Manual, etc.)
	 */
	template<MArgumentType Type>
	class MContainerBase {
	public:
		using Container = MType_t<Type>;

	public:
		/**
		 * @brief Default constructor.
		 * @details Triggers compile-time error unless PassingMode is Manual. Wrappers with all other passing modes can only be constructed given existing
		 * raw containers, so they cannot be empty.
		 */
		MContainerBase() = default;

		/**
		 * @brief Create MContainerBase from a raw container
		 * @param c - raw LibraryLink container (MTensor, MNumericArray, etc.), passing a nullptr will trigger an exception
		 */
		/* implicit */ MContainerBase(Container c, Passing mode) : MContainerBase(c, ownerFromPassingMode(mode)) {}

		/* implicit */ MContainerBase(Container c, Ownership owner) : container {c}, ownerMode {owner} {
			if (!c) {
				ErrorManager::throwException(ErrorName::CreateFromNullError);
			}
		}

		/**
		 * @brief Copy-constructor, performs a deep copy of the raw container.
		 * @param mc - MContainerBase to be copied
		 */
		MContainerBase(const MContainerBase& mc) : container(mc.clone()) {}

		/**
		 * @brief Move-constructor
		 * @param mc - MContainerBase to be moved-from, it's internal container becomes nullptr
		 */
		MContainerBase(MContainerBase&& mc) noexcept : container {mc.container}, ownerMode {mc.ownerMode} {
			mc.container = nullptr;
		}

		/**
		 * @brief Copy-assignment operator, performs a deep copy of the raw container.
		 * @param mc - MContainerBase to be copied
		 * @return reference to this object
		 */
		MContainerBase& operator=(const MContainerBase& mc) {
			reset(mc.clone());
			return *this;
		}

		/**
		 * @brief Move-assignment operator
		 * @param mc - MContainerBase to be moved-from, it's internal container becomes nullptr
		 * @return reference to this object
		 */
		MContainerBase& operator=(MContainerBase&& mc) noexcept {
			reset(mc.container, mc.ownerMode);
			mc.container = nullptr;
			return *this;
		}

		/// Default destructor
		virtual ~MContainerBase() {
			reset(nullptr);
		}

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
//		Container abandonContainer() const noexcept {
//			this->setOwner(false);
//			return container;
//		}

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
		void pass(MArgument& res) const {
			if (container) {
				passImpl(res);
			}
			if (ownerMode == Ownership::Library) {
				ownerMode = Ownership::LibraryLink;
			}
		}

	protected:
		/// Disown internal container if present
		void disown() const noexcept {
			if (container) {
				disownImpl();
			}
		}

		/// Free internal container if present
		void free() const noexcept {
			if (container) {
				freeImpl();
			}
		}

		/**
		 * @brief   Set new internal container. The ownership state of this wrapper is not changed
		 * @param   newCont - new internal container
		 */
		void reset(Container newCont, Ownership newOwnerMode = Ownership::Library) noexcept {
			switch (ownerMode) {
				case Ownership::Shared:
					disown();
					break;
				case Ownership::Library:
					free();
					break;
				case Ownership::LibraryLink: break;
			}
			ownerMode = newOwnerMode;
			container = newCont;
		}

		static Ownership ownerFromPassingMode(Passing m) noexcept {
			if (m == Passing::Manual) {
				return Ownership::Library;
			}
			if (m == Passing::Shared) {
				return Ownership::Shared;
			}
			return Ownership::LibraryLink;
		}

	private:

		virtual Container cloneImpl() const = 0;

		virtual void freeImpl() const = 0;

		virtual void disownImpl() const = 0;

		virtual mint shareCountImpl() const = 0;

		virtual void passImpl(MArgument& res) const = 0;

		/// Raw LibraryLink container (MTensor, MImage, DataStore, etc.)
		Container container;

		mutable Ownership ownerMode = Ownership::Library;
	};

	/**
	 * @class   MContainer
	 * @brief   MContainer is an abstract class template for generic containers. Only specializations shall be used.
	 * @tparam  Type - container type (see MArgumentType definition)
	 * @tparam  PassingMode - passing policy (Shared, Manual, etc.)
	 */
	template<MArgumentType Type>
#ifdef _WIN32
	class MContainer;	 // On Windows we cannot provide a body with static_assert because of ridiculous MSVC compiler errors (probably a bug).
#else					 // On other platforms we get a nice, compile-time error.
	class MContainer {
		static_assert(Type == MArgumentType::Tensor || Type == MArgumentType::Image || Type == MArgumentType::NumericArray || Type == MArgumentType::DataStore,
				"Trying to instantiate unspecialized MContainer template.");
	};
#endif
}

#endif	  // LLU_CONTAINERS_GENERIC_BASE_HPP
