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
		MContainerBase(Container c, Passing mode) : MContainerBase(c, ownerFromPassingMode(mode)) {}

		explicit MContainerBase(Container c) : MContainerBase(c, Ownership::Library) {}

		MContainerBase(Container c, Ownership owner) : container {c}, owner {owner} {
			if (!c) {
				ErrorManager::throwException(ErrorName::CreateFromNullError);
			}
		}

		/**
		 * @brief Copy-constructor, performs a deep copy of the raw container.
		 * @param mc - MContainerBase to be copied
		 */
		MContainerBase(const MContainerBase& mc) = delete;

		/**
		 * @brief Move-constructor
		 * @param mc - MContainerBase to be moved-from, it's internal container becomes nullptr
		 */
		MContainerBase(MContainerBase&& mc) noexcept : container {mc.container}, owner { mc.owner} {
			mc.container = nullptr;
		}

		/**
		 * @brief Copy-assignment operator, performs a deep copy of the raw container.
		 * @param mc - MContainerBase to be copied
		 * @return reference to this object
		 */
		MContainerBase& operator=(const MContainerBase& mc) = delete;

		/**
		 * @brief Move-assignment operator
		 * @param mc - MContainerBase to be moved-from, it's internal container becomes nullptr
		 * @return reference to this object
		 */
		MContainerBase& operator=(MContainerBase&& mc) noexcept {
			reset(mc.container, mc.owner);
			mc.container = nullptr;
			return *this;
		}

		/// Default destructor
		virtual ~MContainerBase() noexcept {
			if (owner == Ownership::Shared) {
				disown();
			} else if (owner == Ownership::Library) {
				free();
			}
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
			owner = Ownership::LibraryLink;
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
		void pass(MArgument& res) const {
			if (container) {
				passImpl(res);
			}
			// Per LibraryLink documentation: returning a Shared container does not affect the memory management, so we only need to cover
			// the case where the library owns the container. In such case the ownership is passed to the LibraryLink
			if (owner == Ownership::Library) {
				owner = Ownership::LibraryLink;
			}
		}

		[[nodiscard]] Ownership getOwner() const noexcept {
			return owner;
		}

	protected:
		/**
		 * @brief Clone the raw container, if it's present
		 * @return cloned container or nullptr if there is no internal container
		 */
		Container cloneContainer() const {
			if (container == nullptr) {
				return nullptr;
			}
			return cloneImpl();
		}

		/// Disown internal container if present
		void disown() const noexcept {
			if (!container || !LibraryData::hasLibraryData()) {
				return;
			}
			if constexpr (Type == MArgumentType::DataStore) {
				// Disowning does nothing for DataStore as it cannot be shared.
			} else if constexpr (Type == MArgumentType::Image) {
				LibraryData::ImageAPI()->MImage_disown(container);
			} else if constexpr (Type == MArgumentType::NumericArray) {
				LibraryData::NumericArrayAPI()->MNumericArray_disown(container);
			} else if constexpr (Type == MArgumentType::SparseArray) {
				LibraryData::SparseArrayAPI()->MSparseArray_disown(container);
			} else if constexpr (Type == MArgumentType::Tensor) {
				LibraryData::API()->MTensor_disown(container);
			} else {
				static_assert(alwaysFalse<Type>, "Unsupported MContainer type.");
			}
		}

		/// Free internal container if present
		void free() const noexcept {
			if (!container || !LibraryData::hasLibraryData()) {
				return;
			}
			if constexpr (Type == MArgumentType::DataStore) {
				LibraryData::DataStoreAPI()->deleteDataStore(container);
			} else if constexpr (Type == MArgumentType::Image) {
				LibraryData::ImageAPI()->MImage_free(container);
			} else if constexpr (Type == MArgumentType::NumericArray) {
				LibraryData::NumericArrayAPI()->MNumericArray_free(container);
			} else if constexpr (Type == MArgumentType::SparseArray) {
				LibraryData::SparseArrayAPI()->MSparseArray_free(container);
			} else if constexpr (Type == MArgumentType::Tensor) {
				LibraryData::API()->MTensor_free(container);
			} else {
				static_assert(alwaysFalse<Type>, "Unsupported MContainer type.");
			}
		}

		/**
		 * @brief   Set new internal container. The ownership state of this wrapper is not changed
		 * @param   newCont - new internal container
		 */
		void reset(Container newCont, Ownership newOwnerMode = Ownership::Library) noexcept {
			switch (owner) {
				case Ownership::Shared:
					disown();
					break;
				case Ownership::Library:
					free();
					break;
				case Ownership::LibraryLink: break;
			}
			owner = newOwnerMode;
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

		virtual mint shareCountImpl() const = 0;

		virtual void passImpl(MArgument& res) const = 0;

		/// Raw LibraryLink container (MTensor, MImage, DataStore, etc.)
		Container container {};

		mutable Ownership owner = Ownership::Library;
	};

	/**
	 * @class   MContainer
	 * @brief   MContainer is an abstract class template for generic containers. Only specializations shall be used.
	 * @tparam  Type - container type (see MArgumentType definition)
	 */
	template<MArgumentType Type, typename std::enable_if_t<isContainerType<Type>, int> = 0>
#ifdef _WIN32
	class MContainer;	 // On Windows we cannot provide a body with static_assert because of ridiculous MSVC compiler errors (probably a bug).
#else					 // On other platforms we get a nice, compile-time error.
	class MContainer {
		static_assert(alwaysFalse<Type>, "Trying to instantiate unspecialized MContainer template.");
	};
#endif
}

#endif	  // LLU_CONTAINERS_GENERIC_BASE_HPP
