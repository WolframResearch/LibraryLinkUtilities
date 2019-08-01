/**
 * @file	Base.hpp
 * @brief
 */

#ifndef LLU_CONTAINERS_GENERIC_BASE_HPP
#define LLU_CONTAINERS_GENERIC_BASE_HPP

#include <type_traits>

#include "LLU/Containers/Passing/Manual.hpp"
#include "LLU/LibraryData.h"
#include "LLU/MArgument.h"
#include "LLU/Utilities.hpp"

namespace LLU {

	template<MArgumentType Type, class PassingMode>
	class MContainerBase : public PassingMode {

		template<MArgumentType, class>
		friend class MContainerBase;

	public:
		using Container = MType_t<Type>;

	public:
		MContainerBase() {
			static_assert(std::is_same<PassingMode, Passing::Manual>::value, "New MContainer can only be created with passing mode Manual.");
		}

		explicit MContainerBase(Container c) : container(c) {
		}

		template<class P>
		MContainerBase(const MContainerBase<Type, P>& mc) : PassingMode(mc), container(mc.clone()) {
		}

		MContainerBase(const MContainerBase& mc) : PassingMode(mc), container(mc.clone()) {
		}

		MContainerBase(MContainerBase&& mc) noexcept : PassingMode(std::move(mc)), container(mc.container) {
			mc.container = nullptr;
		}

		template<class P>
		MContainerBase& operator=(const MContainerBase<Type, P>& mc) {
			PassingMode::operator=(mc);
			setContainer(mc.clone());
			return *this;
		}

		MContainerBase& operator=(MContainerBase&& mc) noexcept {
			PassingMode::operator=(std::move(mc));
			setContainer(mc.container);
			mc.container = nullptr;
			return *this;
		}

		~MContainerBase() override = default;

		Container clone() const {
			if (container == nullptr) {
				// TODO
			}
			return cloneImpl();
		}


		Container getContainer() const noexcept {
			return container;
		}

		Container abandonContainer() const noexcept {
			this->setOwner(false);
			return container;
		}

		/**
		 *   @brief 	Return share count of internal MTensor.
		 *   Use this to manually manage shared MTensors.
		 *
		 *   @see 		<http://reference.wolfram.com/language/LibraryLink/ref/callback/MTensor_shareCount.html>
		 *   @note		LibraryLink does not provide any way to check whether MTensor was passed or will be returned as "Shared".
		 *   Therefore passing or returning MTensors as "Shared" is discouraged and if you do that you are responsible for managing MTensor memory.
		 **/
		mint shareCount() const noexcept {
			if (container) {
				return shareCountImpl();
			}
			return 0;
		}

		void pass(MArgument& res) const override {
			if (container) {
				passImpl(res);
			}
		}

	protected:
		/**
		 *   @brief 	Disown internal MTensor that is shared with Mathematica.
		 *   Use this to manually manage shared MTensors.
		 *
		 *   @see 		<http://reference.wolfram.com/language/LibraryLink/ref/callback/MTensor_disown.html>
		 *   @note		LibraryLink does not provide any way to check whether MTensor was passed or will be returned as "Shared".
		 *   Therefore passing or returning MTensors as "Shared" is discouraged and if you do that you are responsible for managing MTensor memory.
		 **/
		void disown() const noexcept override {
			if (container) {
				disownImpl();
			}
		}

		void free() const noexcept override {
			if (container) {
				freeImpl();
			}
		}

		void setContainer(Container newCont) noexcept {
			container = newCont;
		}

	private:
		virtual Container cloneImpl() const = 0;

		virtual void freeImpl() const = 0;

		virtual void disownImpl() const = 0;

		virtual mint shareCountImpl() const = 0;

		virtual void passImpl(MArgument& res) const = 0;

		Container container {};
	};

	template<MArgumentType Type, class PassingMode>
#ifdef _WIN32
	class MContainer; // On Windows we cannot provide a body with static_assert because of ridiculous MSVC compiler errors (probably a bug).
#else // On other platforms we get a nice, compile-time error.
	class MContainer {
		static_assert(dependent_false_v<PassingMode>, "Trying to instantiate unspecialized MContainer template.");
	};
#endif
}

#endif //LLU_CONTAINERS_GENERIC_BASE_HPP
