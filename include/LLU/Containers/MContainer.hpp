/**
 * @file	MContainer.hpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	February 08, 2019
 * @brief	
 */
#ifndef LLU_MCONTAINER_HPP
#define LLU_MCONTAINER_HPP

#include <iostream>
#include <type_traits>
#include <LLU/Containers/Passing/Shared.hpp>

#include "LLU/LibraryData.h"
#include "LLU/Containers/Passing/Manual.hpp"
#include "LLU/MArgument.h"
#include "LLU/Utilities.hpp"

namespace LLU {

	template<MArgumentType Type, class PassingMode>
	class MContainerBase : public PassingMode {
	public:
		using Container = MType_t<Type>;

	public:
		MContainerBase() {
			static_assert(std::is_same<PassingMode, Passing::Manual>::value, "New MContainer can only be created with passing mode Manual.");
		}

		explicit MContainerBase(Container c) : container(c) {}

		template<class P>
		MContainerBase(const MContainerBase<Type, P>& mc) : PassingMode(mc), container(mc.clone()) {
		}

        template<class P>
		MContainerBase(MContainerBase<Type, P>&& mc) noexcept : PassingMode(std::move(mc)), container(mc.container) {
			mc.container = nullptr;
		}

        template<class P>
		MContainerBase& operator=(const MContainerBase<Type, P>& mc) {
			PassingMode::operator=(mc);
			setContainer(mc.clone());
			return *this;
		}

        template<class P>
		MContainerBase& operator=(MContainerBase<Type, P>&& mc) noexcept {
			PassingMode::operator=(std::move(mc));
			setContainer(mc.container);
			mc.container = nullptr;
			return *this;
		}

		virtual ~MContainerBase() = default;

		Container clone() const {
			if (container == nullptr) {
			// TODO
			}
			return cloneImpl();
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

		void pass(MArgument& res) const override {
			if (container) {
				passImpl(res);
			}
		}

		Container getContainer() const noexcept {
			return container;
		}

		Container abandonContainer() const noexcept {
			this->setOwner(false);
			return container;
		}

	protected:
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
	class MContainer {
		static_assert(dependent_false_v<PassingMode>, "Trying to instantiate unspecialized MContainer template.");
	};

	template<class PassingMode>
	class MContainer<MArgumentType::Tensor, PassingMode> : public MContainerBase<MArgumentType::Tensor, PassingMode> {
	public:
		using Base = MContainerBase<MArgumentType::Tensor, PassingMode>;
		using RawContainer = typename Base::Container;

		MContainer(mint type, mint rank, const mint* dims) {
			RawContainer tmp {};
			if (LibraryData::API()->MTensor_new(type, rank, dims, &tmp)) {
				ErrorManager::throwException(ErrorName::TensorNewError);
			}
			this->setContainer(tmp);
		}

		MContainer(RawContainer t) : Base(t) {}

        template<class P>
        MContainer(const MContainer<MArgumentType::Tensor, P>& mc) : Base(mc) {}

        template<class P>
        MContainer(MContainer<MArgumentType::Tensor, P>&& mc) noexcept : Base(std::move(mc)) {
        }

        template<class P>
        MContainer& operator=(const MContainer<MArgumentType::Tensor, P>& mc) {
            Base::operator=(mc);
            return *this;
        }

        template<class P>
        MContainer& operator=(MContainer<MArgumentType::Tensor, P>&& mc) noexcept {
            Base::operator=(std::move(mc));
            return *this;
        }

        ~MContainer() {
            this->cleanup();
        };

        mint getRank() const noexcept {
            return LibraryData::API()->MTensor_getRank(this->getContainer());
		}

        mint const* getDimensions() const {
            return LibraryData::API()->MTensor_getDimensions(this->getContainer());
        }

        mint getFlattenedLength( ) const {
            return LibraryData::API()->MTensor_getFlattenedLength(this->getContainer());
        }

        mint type() const {
            return LibraryData::API()->MTensor_getType(this->getContainer());
        }

	private:

		mint shareCountImpl() const noexcept override {
			return LibraryData::API()->MTensor_shareCount(this->getContainer());
		}

		void disownImpl() const noexcept override {
			LibraryData::API()->MTensor_disown(this->getContainer());
		}

		void freeImpl() const noexcept override {
			LibraryData::API()->MTensor_free(this->getContainer());
		}

		void passImpl(MArgument& res) const noexcept override {
			MArgument_setMTensor(res, this->abandonContainer());
		}

		RawContainer cloneImpl() const override {
			RawContainer tmp {};
			if(LibraryData::API()->MTensor_clone(this->getContainer(), &tmp)) {
				ErrorManager::throwException(ErrorName::TensorCloneError);
			}
			return tmp;
		}
	};

	template<class PassingMode>
	class MContainer<MArgumentType::Image, PassingMode> : public MContainerBase<MArgumentType::Image, PassingMode> {
	public:
		using Base = MContainerBase<MArgumentType::Image, PassingMode>;
		using RawContainer = typename Base::Container;

		MContainer(mint width, mint height, mint channels, imagedata_t type, colorspace_t colorSpace, mbool interleaving) :
			MContainer(0, width, height, channels, type, colorSpace, interleaving) {}

		MContainer(mint slices, mint width, mint height, mint channels, imagedata_t type, colorspace_t colorSpace, mbool interleaving) {
			RawContainer tmp{};
			if (slices ? LibraryData::ImageAPI()->MImage_new3D(slices, width, height, channels, type, colorSpace, interleaving, &tmp) :
					LibraryData::ImageAPI()->MImage_new2D(width, height, channels, type, colorSpace, interleaving, &tmp)) {
				ErrorManager::throwException(ErrorName::ImageNewError);
			}
			this->setContainer(tmp);
		}

		MContainer(RawContainer i) : Base(i) {}

		template<class P>
		MContainer(const MContainer<MArgumentType::Image, P> &mc) : Base(mc) {}

		template<class P>
		MContainer(MContainer<MArgumentType::Image, P> &&mc) noexcept : Base(std::move(mc)) {
		}

        ~MContainer() {
            this->cleanup();
        };

		MContainer<MArgumentType::Image, Passing::Manual> convert(imagedata_t t, mbool interleavingQ) const {
			auto newImage = LibraryData::ImageAPI()->MImage_convertType(this->getContainer(), t, interleavingQ);
			if (!newImage) {
				ErrorManager::throwException(ErrorName::ImageNewError, "Conversion to type " + std::to_string(static_cast<int>(t)) + " failed.");
			}
			return newImage;
		}

		MContainer<MArgumentType::Image, Passing::Manual> convert(imagedata_t t) const {
			return convert(t, interleavedQ());
		}

		/**
		 *   @brief Get colorspace of internal MImage
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getColorSpace.html>
		 **/
		colorspace_t colorspace() const noexcept {
			return LibraryData::ImageAPI()->MImage_getColorSpace(this->getContainer());
		}

		/**
		 *   @brief Get number of rows in internal MImage
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getRowCount.html>
		 **/
		mint rows() const noexcept {
			return LibraryData::ImageAPI()->MImage_getRowCount(this->getContainer());
		}

		/**
		 *   @brief Get number of columns in internal MImage
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getColumnCount.html>
		 **/
		mint columns() const noexcept {
			return LibraryData::ImageAPI()->MImage_getColumnCount(this->getContainer());
		}

		/**
		 *   @brief Get number of slices in internal MImage
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getSliceCount.html>
		 **/
		mint slices() const noexcept {
			return LibraryData::ImageAPI()->MImage_getSliceCount(this->getContainer());
		}

		/**
		 *   @brief Get number of channels in internal MImage
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getChannels.html>
		 **/
		mint channels() const noexcept {
			return LibraryData::ImageAPI()->MImage_getChannels(this->getContainer());
		}

		/**
		 *   @brief Check if there is an alpha channel in internal MImage
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_alphaChannelQ.html>
		 **/
		bool alphaChannelQ() const noexcept {
			return LibraryData::ImageAPI()->MImage_alphaChannelQ(this->getContainer());
		}

		/**
		 *   @brief Check if internal MImage is interleaved
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_interleavedQ.html>
		 **/
		bool interleavedQ() const noexcept {
			return LibraryData::ImageAPI()->MImage_interleavedQ(this->getContainer());
		}

		/**
		 *   @brief Check if internal MImage is 3D
		 **/
		bool is3D() const noexcept {
			return LibraryData::ImageAPI()->MImage_getRank(this->getContainer()) == 3;
		}

        imagedata_t type() const noexcept {
            return LibraryData::ImageAPI()->MImage_getDataType(this->getContainer());
        }

        void* rawData() const noexcept {
            return LibraryData::ImageAPI()->MImage_getRawData(this->getContainer());
        }
	private:

		RawContainer cloneImpl() const override {
			RawContainer tmp{};
			if(LibraryData::ImageAPI()->MImage_clone(this->getContainer(), &tmp)) {
				ErrorManager::throwException(ErrorName::ImageCloneError);
			}
			return tmp;
		}

		mint shareCountImpl() const noexcept override {
			return LibraryData::ImageAPI()->MImage_shareCount(this->getContainer());
		}

		void disownImpl() const noexcept override {
			LibraryData::ImageAPI()->MImage_disown(this->getContainer());
		}

		void freeImpl() const noexcept override {
			LibraryData::ImageAPI()->MImage_free(this->getContainer());
		}

		void passImpl(MArgument& res) const noexcept override {
			MArgument_setMImage(res, this->getContainer());
		}

	};

	template<class PassingMode>
	class MContainer<MArgumentType::NumericArray, PassingMode> : public MContainerBase<MArgumentType::NumericArray, PassingMode> {
	public:
		using Base = MContainerBase<MArgumentType::NumericArray, PassingMode>;
		using RawContainer = typename Base::Container;

		MContainer(numericarray_data_t type, mint rank, const mint* dims) {
			RawContainer tmp {};
			if (LibraryData::NumericArrayAPI()->MNumericArray_new(type, rank, dims, &tmp)) {
				ErrorManager::throwException(ErrorName::NumericArrayNewError);
			}
			this->setContainer(tmp);
		}

		MContainer(RawContainer t) : Base(t) {};

        template<class P>
        MContainer(const MContainer<MArgumentType::NumericArray, P>& mc) : Base(mc) {}

        template<class P>
        MContainer(MContainer<MArgumentType::NumericArray, P>&& mc) noexcept : Base(std::move(mc)) {
        }

        template<class P>
        MContainer& operator=(const MContainer<MArgumentType::NumericArray, P>& mc) {
            Base::operator=(mc);
            return *this;
        }

        template<class P>
        MContainer& operator=(MContainer<MArgumentType::NumericArray, P>&& mc) noexcept {
            Base::operator=(std::move(mc));
            return *this;
        }

        ~MContainer() {
            this->cleanup();
        };

		MContainer<MArgumentType::NumericArray, Passing::Manual> convert(numericarray_data_t t, NA::ConversionMethod method, double param) const {
			MNumericArray newNA = nullptr;
			auto err = LibraryData::NumericArrayAPI()->MNumericArray_convertType(&newNA, this->getContainer(), t,
																											  static_cast<numericarray_convert_method_t>(method), param);
			if (err) {
				ErrorManager::throwException(ErrorName::NumericArrayConversionError, "Conversion to type " + std::to_string(static_cast<int>(t)) + " failed.");
			}
			return newNA;
		}

		mint getRank() const noexcept {
			return LibraryData::NumericArrayAPI()->MNumericArray_getRank(this->getContainer());
		}

		mint const *getDimensions() const {
			return LibraryData::NumericArrayAPI()->MNumericArray_getDimensions(this->getContainer());
		}

		mint getFlattenedLength() const {
			return LibraryData::NumericArrayAPI()->MNumericArray_getFlattenedLength(this->getContainer());
		}

		mint type() const {
			return LibraryData::NumericArrayAPI()->MNumericArray_getType(this->getContainer());
		}
	private:

		RawContainer cloneImpl() const override {
			RawContainer tmp {};
			if(LibraryData::NumericArrayAPI()->MNumericArray_clone(this->getContainer(), &tmp)) {
				ErrorManager::throwException(ErrorName::NumericArrayCloneError);
			}
			return tmp;
		}

		mint shareCountImpl() const noexcept override {
			return LibraryData::NumericArrayAPI()->MNumericArray_shareCount(this->getContainer());
		}

		void disownImpl() const noexcept override {
			LibraryData::NumericArrayAPI()->MNumericArray_disown(this->getContainer());
		}

		void freeImpl() const noexcept override {
			LibraryData::NumericArrayAPI()->MNumericArray_free(this->getContainer());
		}

		void passImpl(MArgument& res) const noexcept override {
			MArgument_setMNumericArray(res, this->getContainer());
		}
	};

	template<class PassingMode>
	class MContainer<MArgumentType::DataStore, PassingMode> : public MContainerBase<MArgumentType::DataStore, PassingMode> {
		static_assert(!std::is_same<PassingMode, Passing::Shared>::value, "DataStore cannot be Shared.");
	public:

		using Base = MContainerBase<MArgumentType::DataStore, PassingMode>;
		using RawContainer = typename Base::Container;

		MContainer() : MContainer(LibraryData::DataStoreAPI()->createDataStore()) {
		}

		MContainer(RawContainer t) : Base(t) {};

		template<class P>
		MContainer(const MContainer<MArgumentType::DataStore, P> &mc) : Base(mc) {}

		template<class P>
		MContainer(MContainer<MArgumentType::DataStore, P> &&mc) noexcept : Base(std::move(mc)) {
		}

		template<class P>
		MContainer &operator=(const MContainer<MArgumentType::DataStore, P> &mc) {
			Base::operator=(mc);
			return *this;
		}

		template<class P>
		MContainer &operator=(MContainer<MArgumentType::DataStore, P> &&mc) noexcept {
			Base::operator=(std::move(mc));
			return *this;
		}

        ~MContainer() {
            this->cleanup();
        };

	private:

		RawContainer cloneImpl() const override {
			return LibraryData::DataStoreAPI()->copyDataStore(this->getContainer());
		}

		mint shareCountImpl() const noexcept override {
			return 0;
		}

		void disownImpl() const noexcept override {
		}

		void freeImpl() const noexcept override {
			LibraryData::DataStoreAPI()->deleteDataStore(this->getContainer());
		}

		void passImpl(MArgument& res) const noexcept override {
			MArgument_setDataStore(res, this->getContainer());
		}
	};

}

#endif //LLU_MCONTAINER_HPP
